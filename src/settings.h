#include <modules.h>

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define DEFAULTTIMEZONE           1                 // [Timezone] +1 hour (Rome) (-13 .. 14 = hours from UTC, 99 = use TIME_DST/TIME_STD)


// Defaults
// -- Time
#define NTP_SERVER             "pool.ntp.org"       // [NtpServer] Select NTP server by name or IP address (129.250.35.250)

#define SAVE_DATA              1                    // [SaveData] Save changed parameters to Flash (0 = disable, 1 - 3600 seconds)
#define SAVE_STATE             1                 // [SetOption0] Save changed power state to Flash (0 = disable, 1 = enable)
#define APP_SLEEP              0                    // [Sleep] Sleep time to lower energy consumption (0 = Off, 1 - 250 mSec)

#define MODULE                 GENERIC              // [Module] Select default model

#define NETWORK_HOSTNAME       "es"
#define NETWORK_IP             "0.0.0.0"         // [IpAddress1] Set to 0.0.0.0 for using DHCP or enter a static IP address
#define NETWORK_GATEWAY        "192.168.1.1"     // [IpAddress2] If not using DHCP set Gateway IP address
#define NETWORK_SUBNETMASK     "255.255.255.0"   // [IpAddress3] If not using DHCP set Network mask
#define NETWORK_DNS            "192.168.1.1"     // [IpAddress4] If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)

#define WIFI_SSID              ""                // [Ssid] Wifi SSID
#define WIFI_PASS              ""                // [Password] Wifi password
#define WIFI_CONFIG_TOOL       WIFI_RETRY        // [WifiConfig] Default tool if wifi fails to connect
                                                 //   (WIFI_RESTART, WIFI_SMARTCONFIG, WIFI_MANAGER, WIFI_WPSCONFIG, WIFI_RETRY, WIFI_WAIT, WIFI_SERIAL)
#define WIFI_CONFIG_NO_SSID    WIFI_WPSCONFIG    // Default tool if wifi fails to connect and no SSID is configured
                                                 //   (WIFI_SMARTCONFIG, WIFI_MANAGER, WIFI_WPSCONFIG, WIFI_SERIAL)
                                                 //   *** NOTE: When WPS is disabled by USE_WPS below, WIFI_WPSCONFIG will execute WIFI_MANAGER ***
                                                 //   *** NOTE: When WIFI_MANAGER is disabled by USE_WEBSERVER below, WIFI_MANAGER will execute WIFI_SMARTCONFIG ***
                                                 //   *** NOTE: When WIFI_SMARTCONFIG is disabled by USE_SMARTCONFIG below, WIFI_SMARTCONFIG will execute WIFI_SERIAL ***

// -- Syslog --------------------------------------
#define SYSLOG_HOST            ""                // [LogHost] (Linux) syslog host
#define SYSLOG_PORT            514               // [LogPort] default syslog UDP port
#define SYSLOG_LOGLEVEL        LOG_LEVEL_NONE    // [SysLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)
#define SERIAL_LOGLEVEL        LOG_LEVEL_INFO    // [SerialLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)
#define WEB_LOGLEVEL           LOG_LEVEL_INFO    // [WebLog] (LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE)


// -- Ota -----------------------------------------
#define OTA_URL                "http://bit.ly/esprinlker/sonoff.bin"  // [OtaUrl]

// -- MQTT ----------------------------------------
#define MQTT_USE               1                 // [SetOption3] Select default MQTT use (0 = Off, 1 = On)

#define MQTT_HOST              ""                // [MqttHost]
#define MQTT_FINGERPRINT1      "A5 02 FF 13 99 9F 8B 39 8E F1 83 4F 11 23 65 0B 32 36 FC 07"  // [MqttFingerprint1]
#define MQTT_FINGERPRINT2      "A5 02 FF 13 99 9F 8B 39 8E F1 83 4F 11 23 65 0B 32 36 FC 07"  // [MqttFingerprint2]
#define MQTT_PORT              1883              // [MqttPort] MQTT port (10123 on CloudMQTT)
#define MQTT_USER              "DVES_USER"       // [MqttUser] MQTT user
#define MQTT_PASS              "DVES_PASS"       // [MqttPassword] MQTT password
#define MQTT_RETRY_SECS        10           // Minimum seconds to retry MQTT connection


#define MQTT_STATUS_OFF        "OFF"             // [StateText1] Command or Status result when turned off (needs to be a string like "0" or "Off")
#define MQTT_STATUS_ON         "ON"              // [StateText2] Command or Status result when turned on (needs to be a string like "1" or "On")
#define MQTT_CMND_TOGGLE       "TOGGLE"          // [StateText3] Command to send when toggling (needs to be a string like "2" or "Toggle")
#define MQTT_CMND_HOLD         "HOLD"            // [StateText4] Command to send when button is kept down for over KEY_HOLD_TIME * 0.1 seconds (needs to be a string like "HOLD")

// -- MQTT topics ---------------------------------
  // Example "tasmota/bedroom/%topic%/%prefix%/" up to 80 characers
#define MQTT_FULLTOPIC         "%prefix%/%topic%/" // [FullTopic] Subscribe and Publish full topic name - Legacy topic

// %prefix% token options
#define SUB_PREFIX             "cmnd"            // [Prefix1] Sonoff devices subscribe to %prefix%/%topic% being SUB_PREFIX/MQTT_TOPIC and SUB_PREFIX/MQTT_GRPTOPIC
#define PUB_PREFIX             "stat"            // [Prefix2] Sonoff devices publish to %prefix%/%topic% being PUB_PREFIX/MQTT_TOPIC
#define PUB_PREFIX2            "tele"            // [Prefix3] Sonoff devices publish telemetry data to %prefix%/%topic% being PUB_PREFIX2/MQTT_TOPIC/UPTIME, POWER and TIME
                                                 //   May be named the same as PUB_PREFIX
// %topic% token options (also ButtonTopic and SwitchTopic)
#define MQTT_TOPIC             "sprinkler"       // [Topic] (unique) MQTT device topic, set to 'PROJECT "_%06X"' for unique topic including device MAC address
#define MQTT_GRPTOPIC          "sonoffs"         // [GroupTopic] MQTT Group topic
#define MQTT_BUTTON_TOPIC      "0"               // [ButtonTopic] MQTT button topic, "0" = same as MQTT_TOPIC, set to 'PROJECT "_BTN_%06X"' for unique topic including device MAC address
#define MQTT_SWITCH_TOPIC      "0"               // [SwitchTopic] MQTT button topic, "0" = same as MQTT_TOPIC, set to 'PROJECT "_SW_%06X"' for unique topic including device MAC address
#define MQTT_CLIENT_ID         "DVES_%06X"       // [MqttClient] Also fall back topic using Chip Id = last 6 characters of MAC address

// -- MQTT - Telemetry ----------------------------
#define TELE_PERIOD            300               // [TelePeriod] Telemetry (0 = disable, 10 - 3600 seconds)

// -- MQTT - Domoticz -----------------------------
#define DOMOTICZ_UPDATE_TIMER  0                 // [DomoticzUpdateTimer] Send relay status (0 = disable, 1 - 3600 seconds)

// -- MQTT - Home Assistant Discovery -------------
#define HOME_ASSISTANT_DISCOVERY_ENABLE   0      // [SetOption19] Home Assistant Discovery (0 = Disable, 1 = Enable)

// -- HTTP ----------------------------------------
#define WEB_SERVER             2                 // [WebServer] Web server (0 = Off, 1 = Start as User, 2 = Start as Admin)
#define WEB_PASSWORD           ""                // [WebPassword] Web server Admin mode Password for WEB_USERNAME (empty string = Disable)
#define FRIENDLY_NAME          "Sonoff"          // [FriendlyName] Friendlyname up to 32 characters used by webpages and Alexa
#define EMULATION              EMUL_NONE         // [Emulation] Select Belkin WeMo (single relay/light) or Hue Bridge emulation (multi relay/light) (EMUL_NONE, EMUL_WEMO or EMUL_HUE)

// -- mDNS ----------------------------------------
#define MDNS_ENABLED           0                 // [SetOption55] Use mDNS (0 = Disable, 1 = Enable)

// -- Time - Up to three NTP servers in your region
#define NTP_SERVER1            "pool.ntp.org"       // [NtpServer1] Select first NTP server by name or IP address (129.250.35.250)
#define NTP_SERVER2            "nl.pool.ntp.org"    // [NtpServer2] Select second NTP server by name or IP address (5.39.184.5)
#define NTP_SERVER3            "0.nl.pool.ntp.org"  // [NtpServer3] Select third NTP server by name or IP address (93.94.224.67)

// -- Time - Start Daylight Saving Time and timezone offset from UTC in minutes
#define TIME_DST_HEMISPHERE    North             // [TimeDst] Hemisphere (0 or North, 1 or South)
#define TIME_DST_WEEK          Last              // Week of month (0 or Last, 1 or First, 2 or Second, 3 or Third, 4 or Fourth)
#define TIME_DST_DAY           Sun               // Day of week (1 or Sun, 2 or Mon, 3 or Tue, 4 or Wed, 5 or Thu, 6 or Fri, 7 or Sat)
#define TIME_DST_MONTH         Mar               // Month (1 or Jan, 2 or Feb, 3 or Mar, 4 or Apr, 5 or May, 6 or Jun, 7 or Jul, 8 or Aug, 9 or Sep, 10 or Oct, 11 or Nov, 12 or Dec)
#define TIME_DST_HOUR          2                 // Hour (0 to 23)
#define TIME_DST_OFFSET        +120              // Offset from UTC in minutes (-780 to +780)

// -- Time - Start Standard Time and timezone offset from UTC in minutes
#define TIME_STD_HEMISPHERE    North             // [TimeStd] Hemisphere (0 or North, 1 or South)
#define TIME_STD_WEEK          Last              // Week of month (0 or Last, 1 or First, 2 or Second, 3 or Third, 4 or Fourth)
#define TIME_STD_DAY           Sun               // Day of week (1 or Sun, 2 or Mon, 3 or Tue, 4 or Wed, 5 or Thu, 6 or Fri, 7 or Sat)
#define TIME_STD_MONTH         Oct               // Month (1 or Jan, 2 or Feb, 3 or Mar, 4 or Apr, 5 or May, 6 or Jun, 7 or Jul, 8 or Aug, 9 or Sep, 10 or Oct, 11 or Nov, 12 or Dec)
#define TIME_STD_HOUR          3                 // Hour (0 to 23)
#define TIME_STD_OFFSET        +60               // Offset from UTC in minutes (-780 to +780)

// -- Location ------------------------------------
#define LATITUDE               48.858360         // [Latitude] Your location to be used with sunrise and sunset
#define LONGITUDE              2.294442          // [Longitude] Your location to be used with sunrise and sunset



typedef union {                            // Restricted by MISRA-C Rule 18.4 but so useful...
  uint32_t data;                           // Allow bit manipulation using SetOption
  struct {                                 // SetOption0 .. SetOption31
    uint32_t save_state : 1;               // 00
    uint32_t button_restrict : 1;          // 01
    uint32_t value_units : 1;              // 02
    uint32_t mqtt_enabled : 1;             // 03
    uint32_t mqtt_response : 1;            // 04
    uint32_t mdns_enabled : 1;             // 05
    uint32_t stop_flash_rotate  : 1;       // 06 Flash rotate is implemented to preserve flash from writings on same locations
    uint32_t available7  : 1;              // 07
    uint32_t available8  : 1;              // 08
    uint32_t available9  : 1;              // 09
    uint32_t available10  : 1;             // 10
    uint32_t available11  : 1;             // 11 
    uint32_t available12  : 1;             // 12 
    uint32_t available13  : 1;             // 13 
    uint32_t available14  : 1;             // 14 
    uint32_t available15  : 1;             // 15 
    uint32_t available16  : 1;             // 16
    uint32_t available17  : 1;             // 17
    uint32_t available18  : 1;             // 18
    uint32_t available19  : 1;             // 19
    uint32_t available20  : 1;             // 20
    uint32_t available21  : 1;             // 21
    uint32_t available22  : 1;             // 22
    uint32_t available23  : 1;             // 23
    uint32_t available24  : 1;             // 24
    uint32_t available25  : 1;             // 25
    uint32_t available26  : 1;             // 26
    uint32_t available27  : 1;             // 27
    uint32_t available28  : 1;             // 28
    uint32_t available29  : 1;             // 29
    uint32_t available30  : 1;             // 30
    uint32_t available31  : 1;             // 31
  };
} SysBitfield;

struct SYSCFG {
  uint16_t cfg_holder;                     // 000 v6 header
  uint16_t cfg_size;                       // 002
  unsigned long save_flag;                 // 004
  unsigned long version;                   // 008
  uint16_t bootcount;                      // 00C
  uint16_t cfg_crc;                        // 00E

  SysBitfield   flag;                      // 010
  int16_t       save_data;                 // 014
  int8_t        timezone;                  // 016
  uint8_t       timezone_minutes;          // 66D

  char          ota_url[101];              // 017
  char          mqtt_prefix[3][11];        // 07C
  uint8_t       serial_baudrate;           // 09D
  byte          serial_loglevel;          // 09E

  uint8_t       wifi_config;                // 09F
  byte          wifi_active;                // 0A0
  char          wifi_ssid[33];           // 0A1 - Keep together with sta_pwd as being copied as one chunck with reset 4/5
  char          wifi_pwd[65];            // 0E3 - Keep together with sta_ssid as being copied as one chunck with reset 4/5

  char          network_hostname[33];       // 165
  uint32_t      network_ip;             
  uint32_t      network_gateway;        
  uint32_t      network_subnetmask;     
  uint32_t      network_dns;            

  char          syslog_host[33];           // 186
  uint16_t      syslog_port;               // 1A8
  byte          syslog_loglevel;           // 1AA

  uint8_t       webserver;                 // 1AB
  byte          web_loglevel;              // 1AC
  char          web_password[33];          // 4A9

  char          ntp_server[33];         // 4CE

  uint8_t       mqtt_fingerprint[2][20];   // 1AD

  char          mqtt_fulltopic[100];       // 558
  char          mqtt_host[33];             // 1E9
  uint16_t      mqtt_port;                 // 20A
  char          mqtt_client[33];           // 20C
  char          mqtt_user[33];             // 22D
  char          mqtt_pwd[33];              // 24E
  char          mqtt_topic[33];            // 26F
  char          mqtt_statetext[4][11];         // 313

  char          button_topic[33];          // 290
  char          mqtt_grptopic[33];         // 2B1


  uint16_t      mqtt_retry;                // 396

  char          switch_topic[33];          // 430
  char          serial_delimiter;          // 451
  uint8_t       sleep;                     // 453
  uint8_t       module;                    // 474
};

extern SYSCFG Settings;


void SettingsLoad(void);
void SettingsMigrate(void);


#endif