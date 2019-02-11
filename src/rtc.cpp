#include <Arduino.h>
#include <NTPClientLib.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>

#include "main.h"
#include "log.h"
#include "settings.h"

bool rtcIsInitialized = false;

void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}
bool syncEventTriggered = false; // True if a time event has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void RtcInitialize()
{
  if (!WiFi.isConnected())
    return;

  rtcIsInitialized = true;

  ("NTP Server: " + String(Settings.ntp_server) + " Timezone: " + String(Settings.timezone) + ":" + String(Settings.timezone_minutes)).toCharArray(log_data, sizeof(log_data));
  AddLog(LOG_LEVEL_DEBUG);

  NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
  });
  NTP.begin(Settings.ntp_server, Settings.timezone, true, Settings.timezone_minutes);
  NTP.setInterval(63);
}

void RtcLoop()
{
  if (syncEventTriggered) {
    processSyncEvent (ntpEvent);
    syncEventTriggered = false;
  }

  if (!WiFi.isConnected())
  {
    if (rtcIsInitialized)
    {
      AddLog(LOG_LEVEL_INFO, "Wifi not connected. Stopping NTP client");
      rtcIsInitialized = false;
      NTP.stop();
    }
  }
  else
  {
    if (!rtcIsInitialized)
    {
      AddLog(LOG_LEVEL_INFO, "Wifi  connected. Initializing NTP client");
      rtcIsInitialized = true;
      RtcInitialize();
    }
  }
  

}

void RtcLogInfo()
{
  AddLog(LOG_LEVEL_INFO, NTP.getTimeDateString());
  AddLog(LOG_LEVEL_INFO, NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
  sprintf(log_data, "WiFi is %s", WiFi.isConnected() ? "connected" : "not connected");
  AddLog(LOG_LEVEL_INFO);
  ("Uptime: " + NTP.getUptimeString() + " since " + NTP.getTimeDateString(NTP.getFirstSync())).toCharArray(log_data, sizeof(log_data));
  AddLog(LOG_LEVEL_INFO);
}



