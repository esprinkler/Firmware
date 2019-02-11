#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NtpClientLib.h>

#include "main.h"
#include "serial.h"
#include "log.h"
#include "properties.h"
#include "settings.h"
#include "rtc.h"
#include "wifi.h"

uint32_t uptime = 0;                        // Counting every second until 4294967295 = 130 year
uint8_t ntp_force_sync = 0;                 // Force NTP sync
StateBitfield global_state;                 // Global states (currently Wifi and Mqtt) (8 bits)

#define ONBOARDLED 2 // Built in LED on ESP-12/ESP-07


void loop001Second();
void loop010Seconds();
void loop030Seconds();
void loop120Seconds();


void setup() {
    Serial.begin(DEFAULTBAUDRATE);
    delay(10);
    Serial.println();

    Serial.printf("%s V%s - %s\n", PGM_NAME, PGM_VERSION, PGM_COPYRIGHT);

    SettingsLoad();
    SettingsMigrate();

    //Serial.end();
    //Serial.begin(Settings.serial_baudrate);

    //GpioInit();

    //WifiConnect();

    pinMode (ONBOARDLED, OUTPUT); // Onboard LED
    digitalWrite (ONBOARDLED, HIGH); // Switch off LED

    WiFiSetup();

}

uint lastMillis;
byte lastSecond = 0;

void loop() {
    if (millis() - lastMillis < 0 || millis() - lastMillis > 3000)
        lastMillis = millis();

    if (millis() - lastMillis > 1000)
    {
        loop001Second();
        lastMillis = millis();
    }
}


void loop001Second()
{
    lastSecond++;
    if (lastSecond % 10 == 0)
    {
        loop010Seconds();
    }
    if (lastSecond % 30 == 0)
    {
        loop030Seconds();
    }
    if (lastSecond % 120 == 0)
    {
        loop120Seconds();
        lastSecond = 0;
    }

    RtcLoop();
}

void loop010Seconds()
{
    AddLog(LOG_LEVEL_DEBUG, "10 seconds elapsed");

    WiFiLoop();
}

void loop030Seconds()
{
    AddLog(LOG_LEVEL_DEBUG, "30 seconds elapsed");

    RtcLogInfo();
}

void loop120Seconds()
{
    AddLog(LOG_LEVEL_DEBUG, "120 seconds elapsed");

}
