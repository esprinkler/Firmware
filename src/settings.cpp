/*
  settings.ino - user settings for Sonoff-Tasmota

  Copyright (C) 2019  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "settings.h"
#include "log.h"
#include "properties.h"
#include "serial.h"
#include "network.h"

#define CFG_HOLDER 4617                     // Reset configuration
uint8_t stop_flash_rotate = 0;              // Allow flash configuration rotation

SYSCFG Settings;

void SettingsSave(byte rotate);
void SettingsDefault();
void SettingsDefaultSet1();
void SettingsDefaultSet2();

/*********************************************************************************************\
 * Config - Flash
\*********************************************************************************************/

extern "C" {
#include <spi_flash.h>
}
#include <eboot_command.h>

extern "C" uint32_t _SPIFFS_end;

// From libraries/EEPROM/EEPROM.cpp EEPROMClass
#define SPIFFS_END          ((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE

// Version 4.2 config = eeprom area
#define SETTINGS_LOCATION   SPIFFS_END  // No need for SPIFFS as it uses EEPROM area
// Version 5.2 allow for more flash space
#define CFG_ROTATES         8           // Number of flash sectors used (handles uploads)

/*********************************************************************************************\
 * EEPROM support based on EEPROM library and tuned for Tasmota
\*********************************************************************************************/

uint32_t eeprom_sector = SPIFFS_END;
uint8_t* eeprom_data = 0;
size_t eeprom_size = 0;
bool eeprom_dirty = false;

void EepromBegin(size_t size)
{
  if (size <= 0) { return; }
  if (size > SPI_FLASH_SEC_SIZE - sizeof(Settings) -4) { size = SPI_FLASH_SEC_SIZE - sizeof(Settings) -4; }
  size = (size + 3) & (~3);

  // In case begin() is called a 2nd+ time, don't reallocate if size is the same
  if (eeprom_data && size != eeprom_size) {
    delete[] eeprom_data;
    eeprom_data = new uint8_t[size];
  } else if (!eeprom_data) {
    eeprom_data = new uint8_t[size];
  }
  eeprom_size = size;

  size_t flash_offset = SPI_FLASH_SEC_SIZE - eeprom_size;
  uint8_t* flash_buffer;
  flash_buffer = new uint8_t[SPI_FLASH_SEC_SIZE];
  noInterrupts();
  spi_flash_read(eeprom_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(flash_buffer), SPI_FLASH_SEC_SIZE);
  interrupts();
  memcpy(eeprom_data, flash_buffer + flash_offset, eeprom_size);
  delete[] flash_buffer;

  eeprom_dirty = false;  // make sure dirty is cleared in case begin() is called 2nd+ time
}

size_t EepromLength(void)
{
  return eeprom_size;
}

uint8_t EepromRead(int const address)
{
  if (address < 0 || (size_t)address >= eeprom_size) { return 0; }
  if (!eeprom_data) { return 0; }

  return eeprom_data[address];
}

// Prototype needed for Arduino IDE - https://forum.arduino.cc/index.php?topic=406509.0
template<typename T> T EepromGet(int const address, T &t);
template<typename T> T EepromGet(int const address, T &t)
{
  if (address < 0 || address + sizeof(T) > eeprom_size) { return t; }
  if (!eeprom_data) { return 0; }

  memcpy((uint8_t*) &t, eeprom_data + address, sizeof(T));
  return t;
}

void EepromWrite(int const address, uint8_t const value)
{
  if (address < 0 || (size_t)address >= eeprom_size) { return; }
  if (!eeprom_data) { return; }

  // Optimise eeprom_dirty. Only flagged if data written is different.
  uint8_t* pData = &eeprom_data[address];
  if (*pData != value) {
    *pData = value;
    eeprom_dirty = true;
  }
}

// Prototype needed for Arduino IDE - https://forum.arduino.cc/index.php?topic=406509.0
template<typename T> void EepromPut(int const address, const T &t);
template<typename T> void EepromPut(int const address, const T &t)
{
  if (address < 0 || address + sizeof(T) > eeprom_size) { return; }
  if (!eeprom_data) { return; }

  // Optimise eeprom_dirty. Only flagged if data written is different.
  if (memcmp(eeprom_data + address, (const uint8_t*)&t, sizeof(T)) != 0) {
    eeprom_dirty = true;
    memcpy(eeprom_data + address, (const uint8_t*)&t, sizeof(T));
  }
}

bool EepromCommit(void)
{
  bool ret = false;
  if (!eeprom_size) { return false; }
  if (!eeprom_dirty) { return true; }
  if (!eeprom_data) { return false; }

  size_t flash_offset = SPI_FLASH_SEC_SIZE - eeprom_size;
  uint8_t* flash_buffer;
  flash_buffer = new uint8_t[SPI_FLASH_SEC_SIZE];
  noInterrupts();
  spi_flash_read(eeprom_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(flash_buffer), SPI_FLASH_SEC_SIZE);
  memcpy(flash_buffer + flash_offset, eeprom_data, eeprom_size);
  if (spi_flash_erase_sector(eeprom_sector) == SPI_FLASH_RESULT_OK) {
    if (spi_flash_write(eeprom_sector * SPI_FLASH_SEC_SIZE, reinterpret_cast<uint32_t*>(flash_buffer), SPI_FLASH_SEC_SIZE) == SPI_FLASH_RESULT_OK) {
      eeprom_dirty = false;
      ret = true;
    }
  }
  interrupts();
  delete[] flash_buffer;

  return ret;
}

uint8_t * EepromGetDataPtr()
{
  eeprom_dirty = true;
  return &eeprom_data[0];
}

void EepromEnd(void)
{
  if (!eeprom_size) { return; }

  EepromCommit();
  if (eeprom_data) {
    delete[] eeprom_data;
  }
  eeprom_data = 0;
  eeprom_size = 0;
  eeprom_dirty = false;
}

/********************************************************************************************/

uint16_t settings_crc = 0;
uint32_t settings_location = SETTINGS_LOCATION;
uint8_t *settings_buffer = NULL;

/********************************************************************************************/
/*
 * Based on cores/esp8266/Updater.cpp
 */
void SetFlashModeDout(void)
{
  uint8_t *_buffer;
  uint32_t address;

  eboot_command ebcmd;
  eboot_command_read(&ebcmd);
  address = ebcmd.args[0];
  _buffer = new uint8_t[FLASH_SECTOR_SIZE];

  if (ESP.flashRead(address, (uint32_t*)_buffer, FLASH_SECTOR_SIZE)) {
    if (_buffer[2] != 3) {  // DOUT
      _buffer[2] = 3;
      if (ESP.flashEraseSector(address / FLASH_SECTOR_SIZE)) ESP.flashWrite(address, (uint32_t*)_buffer, FLASH_SECTOR_SIZE);
    }
  }
  delete[] _buffer;
}

void SettingsBufferFree(void)
{
  if (settings_buffer != NULL) {
    free(settings_buffer);
    settings_buffer = NULL;
  }
}

bool SettingsBufferAlloc(void)
{
  SettingsBufferFree();
  if (!(settings_buffer = (uint8_t *)malloc(sizeof(Settings)))) {
    AddLog(LOG_LEVEL_ERROR, "Not enough memory space");
    return false;
  }
  return true;
}

uint16_t GetSettingsCrc(void)
{
  uint16_t crc = 0;
  uint8_t *bytes = (uint8_t*)&Settings;

  for (uint16_t i = 0; i < sizeof(SYSCFG); i++) {
    if ((i < 14) || (i > 15)) { crc += bytes[i]*(i+1); }  // Skip crc
  }
  return crc;
}

void SettingsSaveAll(void)
{
  EepromCommit();
  SettingsSave(0);
}

/*********************************************************************************************\
 * Config Save - Save parameters to Flash ONLY if any parameter has changed
\*********************************************************************************************/

uint32_t GetSettingsAddress(void)
{
  return settings_location * SPI_FLASH_SEC_SIZE;
}

void SettingsSave(byte rotate)
{
/* Save configuration in eeprom or one of 7 slots below
 *
 * rotate 0 = Save in next flash slot
 * rotate 1 = Save only in eeprom flash slot until SetOption12 0 or restart
 * rotate 2 = Save in eeprom flash slot, erase next flash slots and continue depending on stop_flash_rotate
 * stop_flash_rotate 0 = Allow flash slot rotation (SetOption12 0)
 * stop_flash_rotate 1 = Allow only eeprom flash slot use (SetOption12 1)
 */
  if ((GetSettingsCrc() != settings_crc) || rotate) {
    if (1 == rotate) {   // Use eeprom flash slot only and disable flash rotate from now on (upgrade)
      stop_flash_rotate = 1;
    }
    if (2 == rotate) {   // Use eeprom flash slot and erase next flash slots if stop_flash_rotate is off (default)
      settings_location = SETTINGS_LOCATION +1;
    }
    if (stop_flash_rotate) {
      settings_location = SETTINGS_LOCATION;
    } else {
      settings_location--;
      if (settings_location <= (SETTINGS_LOCATION - CFG_ROTATES)) {
        settings_location = SETTINGS_LOCATION;
      }
    }
    Settings.save_flag++;
    Settings.cfg_size = sizeof(SYSCFG);
    Settings.cfg_crc = GetSettingsCrc();

    if (SPIFFS_END == settings_location) {
      uint8_t* flash_buffer;
      flash_buffer = new uint8_t[SPI_FLASH_SEC_SIZE];
      if (eeprom_data && eeprom_size) {
        size_t flash_offset = SPI_FLASH_SEC_SIZE - eeprom_size;
        memcpy(flash_buffer + flash_offset, eeprom_data, eeprom_size);  // Write dirty EEPROM data
      } else {
        ESP.flashRead(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)flash_buffer, SPI_FLASH_SEC_SIZE);   // Read EEPROM area
      }
      memcpy(flash_buffer, &Settings, sizeof(Settings));
      ESP.flashEraseSector(settings_location);
      ESP.flashWrite(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)flash_buffer, SPI_FLASH_SEC_SIZE);
      delete[] flash_buffer;
    } else {
      ESP.flashEraseSector(settings_location);
      ESP.flashWrite(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)&Settings, sizeof(SYSCFG));
    }

    if (!stop_flash_rotate && rotate) {
      for (byte i = 1; i < CFG_ROTATES; i++) {
        ESP.flashEraseSector(settings_location -i);  // Delete previous configurations by resetting to 0xFF
        delay(1);
      }
    }
    AddLog(LOG_LEVEL_DEBUG, "Settings saved");

    settings_crc = Settings.cfg_crc;
  }

}

void SettingsLoad(void)
{
/* Load configuration from eeprom or one of 7 slots below if first load does not stop_flash_rotate
 */
  struct SYSCFGH {
    uint16_t cfg_holder;                     // 000
    uint16_t cfg_size;                       // 002
    unsigned long save_flag;                 // 004
  } _SettingsH;

  bool bad_crc = false;
  settings_location = SETTINGS_LOCATION +1;
  for (byte i = 0; i < CFG_ROTATES; i++) {
    settings_location--;
    ESP.flashRead(settings_location * SPI_FLASH_SEC_SIZE, (uint32*)&Settings, sizeof(SYSCFG));
    ESP.flashRead((settings_location -1) * SPI_FLASH_SEC_SIZE, (uint32*)&_SettingsH, sizeof(SYSCFGH));
    if (Settings.version > 0x06000000) { bad_crc = (Settings.cfg_crc != GetSettingsCrc()); }
    if (Settings.flag.stop_flash_rotate || bad_crc || (Settings.cfg_holder != _SettingsH.cfg_holder) || (Settings.save_flag > _SettingsH.save_flag)) {
      break;
    }
    delay(1);
  }
  AddLog(LOG_LEVEL_DEBUG, "Config correctly loaded");

  if (bad_crc || (Settings.cfg_holder != (uint16_t)CFG_HOLDER)) { SettingsDefault(); }
  settings_crc = GetSettingsCrc();

}

void SettingsErase(uint8_t type)
{
  /*
    0 = Erase from program end until end of physical flash
    1 = Erase SDK parameter area at end of linker memory model (0x0FDxxx - 0x0FFFFF) solving possible wifi errors
  */

  bool result;

  uint32_t _sectorStart = (ESP.getSketchSize() / SPI_FLASH_SEC_SIZE) + 1;
  uint32_t _sectorEnd = ESP.getFlashChipRealSize() / SPI_FLASH_SEC_SIZE;
  if (1 == type) {
    _sectorStart = SETTINGS_LOCATION +2;  // SDK parameter area above EEPROM area (0x0FDxxx - 0x0FFFFF)
    _sectorEnd = SETTINGS_LOCATION +5;
  }

  AddLog(LOG_LEVEL_DEBUG, "Erase settings");

  for (uint32_t _sector = _sectorStart; _sector < _sectorEnd; _sector++) {
    result = ESP.flashEraseSector(_sector);

    Serial.print("");
    Serial.print(_sector);
    if (result) {
      Serial.println(F(" OK"));
    } else {
      Serial.println(F(" Error"));
    }
    delay(10);
  }
}

// Copied from 2.4.0 as 2.3.0 is incomplete
bool SettingsEraseConfig(void) {
  const size_t cfgSize = 0x4000;
  size_t cfgAddr = ESP.getFlashChipSize() - cfgSize;

  for (size_t offset = 0; offset < cfgSize; offset += SPI_FLASH_SEC_SIZE) {
    if (!ESP.flashEraseSector((cfgAddr + offset) / SPI_FLASH_SEC_SIZE)) {
      return false;
    }
  }
  return true;
}

void SettingsSdkErase(void)
{
  WiFi.disconnect(true);    // Delete SDK wifi config
  SettingsErase(1);
  SettingsEraseConfig();
  delay(1000);
}

/********************************************************************************************/

void SettingsDefault(void)
{
  AddLog(LOG_LEVEL_DEBUG, "Reset settings to default");
  SettingsDefaultSet1();
  SettingsDefaultSet2();
  SettingsSave(2);
}

void SettingsDefaultSet1(void)
{
  memset(&Settings, 0x00, sizeof(SYSCFG));

  Settings.cfg_holder = (uint16_t)CFG_HOLDER;
  Settings.cfg_size = sizeof(SYSCFG);
//  Settings.save_flag = 0;
  Settings.version = PGM_VERSION_HEX;
//  Settings.bootcount = 0;
//  Settings.cfg_crc = 0;
}

void SettingsDefaultSet2(void)
{
  memset((char*)&Settings +16, 0x00, sizeof(SYSCFG) -16);

//  Settings.flag.value_units = 0;
//  Settings.flag.stop_flash_rotate = 0;
  Settings.save_data = SAVE_DATA;
  Settings.sleep = APP_SLEEP;
  if (Settings.sleep < 50) {
    Settings.sleep = 50;                // Default to 50 for sleep, for now
  }

  // Module
//  Settings.flag.interlock = 0;
  Settings.module = MODULE;
//  for (byte i = 0; i < sizeof(Settings.my_gp); i++) { Settings.my_gp.io[i] = GPIO_NONE; }
  strlcpy(Settings.ota_url, OTA_URL, sizeof(Settings.ota_url));

  // Power
  Settings.flag.save_state = SAVE_STATE;

  // Serial
  Settings.serial_baudrate = DEFAULTBAUDRATE / 1200;
  Settings.serial_delimiter = 0xff;
  Settings.serial_loglevel = LOG_LEVEL_INFO;

  // Wifi
  strlcpy(Settings.network_hostname, NETWORK_HOSTNAME, sizeof(Settings.network_hostname));
  ParseIp(&Settings.network_ip, NETWORK_IP);
  ParseIp(&Settings.network_gateway, NETWORK_GATEWAY);
  ParseIp(&Settings.network_subnetmask, NETWORK_SUBNETMASK);
  ParseIp(&Settings.network_dns, NETWORK_DNS);
  
  Settings.wifi_config = WIFI_CONFIG_TOOL;

  strlcpy(Settings.wifi_ssid, WIFI_SSID, sizeof(Settings.wifi_ssid));
  strlcpy(Settings.wifi_pwd, WIFI_PASS, sizeof(Settings.wifi_pwd));

  // Syslog
  strlcpy(Settings.syslog_host, SYSLOG_HOST, sizeof(Settings.syslog_host));
  Settings.syslog_port = SYSLOG_PORT;
  Settings.syslog_loglevel = SYSLOG_LOGLEVEL;

  // Webserver
  Settings.webserver = WEB_SERVER;
  Settings.web_loglevel = WEB_LOGLEVEL;
  strlcpy(Settings.web_password, WEB_PASSWORD, sizeof(Settings.web_password));
  
  Settings.flag.mdns_enabled = MDNS_ENABLED;


  // MQTT
  Settings.flag.mqtt_enabled = MQTT_USE;
//  Settings.flag.mqtt_response = 0;
  
  strlcpy(Settings.mqtt_host, MQTT_HOST, sizeof(Settings.mqtt_host));
  Settings.mqtt_port = MQTT_PORT;
  strlcpy(Settings.mqtt_client, MQTT_CLIENT_ID, sizeof(Settings.mqtt_client));
  strlcpy(Settings.mqtt_user, MQTT_USER, sizeof(Settings.mqtt_user));
  strlcpy(Settings.mqtt_pwd, MQTT_PASS, sizeof(Settings.mqtt_pwd));
  strlcpy(Settings.mqtt_topic, MQTT_TOPIC, sizeof(Settings.mqtt_topic));
  strlcpy(Settings.button_topic, MQTT_BUTTON_TOPIC, sizeof(Settings.button_topic));
  strlcpy(Settings.switch_topic, MQTT_SWITCH_TOPIC, sizeof(Settings.switch_topic));
  strlcpy(Settings.mqtt_grptopic, MQTT_GRPTOPIC, sizeof(Settings.mqtt_grptopic));
  strlcpy(Settings.mqtt_fulltopic, MQTT_FULLTOPIC, sizeof(Settings.mqtt_fulltopic));
  Settings.mqtt_retry = MQTT_RETRY_SECS;
  strlcpy(Settings.mqtt_prefix[0], SUB_PREFIX, sizeof(Settings.mqtt_prefix[0]));
  strlcpy(Settings.mqtt_prefix[1], PUB_PREFIX, sizeof(Settings.mqtt_prefix[1]));
  strlcpy(Settings.mqtt_prefix[2], PUB_PREFIX2, sizeof(Settings.mqtt_prefix[2]));
  strlcpy(Settings.mqtt_statetext[0], MQTT_STATUS_OFF, sizeof(Settings.mqtt_statetext[0]));
  strlcpy(Settings.mqtt_statetext[1], MQTT_STATUS_ON, sizeof(Settings.mqtt_statetext[1]));
  strlcpy(Settings.mqtt_statetext[2], MQTT_CMND_TOGGLE, sizeof(Settings.mqtt_statetext[2]));
  strlcpy(Settings.mqtt_statetext[3], MQTT_CMND_HOLD, sizeof(Settings.mqtt_statetext[3]));
  char fingerprint[60];
  strlcpy(fingerprint, MQTT_FINGERPRINT1, sizeof(fingerprint));
  char *p = fingerprint;
  for (byte i = 0; i < 20; i++) {
    Settings.mqtt_fingerprint[0][i] = strtol(p, &p, 16);
  }
  strlcpy(fingerprint, MQTT_FINGERPRINT2, sizeof(fingerprint));
  p = fingerprint;
  for (byte i = 0; i < 20; i++) {
    Settings.mqtt_fingerprint[1][i] = strtol(p, &p, 16);
  }

  // Time
  if (((DEFAULTTIMEZONE > -14) && (DEFAULTTIMEZONE < 15)) || (99 == DEFAULTTIMEZONE)) {
    Settings.timezone = DEFAULTTIMEZONE;
    Settings.timezone_minutes = 0;
  } else {
    Settings.timezone = DEFAULTTIMEZONE / 60;
    Settings.timezone_minutes = abs(DEFAULTTIMEZONE % 60);
  }
  strlcpy(Settings.ntp_server, NTP_SERVER, sizeof(Settings.ntp_server));
}





/********************************************************************************************/

void SettingsMigrate(void)
{
  if (Settings.version != PGM_VERSION_HEX) {      // Fix version dependent changes

    Settings.version = PGM_VERSION_HEX;
    SettingsSave(1);
  }

  // TODO: Fix other infos
}
