#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NtpClientLib.h>
#include <TimeLib.h>
#include "settings.h"
#include "log.h"

#define WIFI_CHANGEMODE_SECONDS 6           // Seconds between swap from AP and STA

bool isSTAConnected = false;
bool isSTATryingConnect = false;
bool isAPConnected = false;
bool isAPMode = false;

time_t lastEvent;


void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
    Serial.printf ("Connected to %s\r\n", ipInfo.ssid.c_str ());
    isSTAConnected = true;
    isSTATryingConnect = false;
    isAPMode = false;
    isAPConnected = false;
}


void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
    Serial.printf ("Got IP: %s\r\n", ipInfo.ip.toString ().c_str ());
    Serial.printf ("Connected: %s\r\n", WiFi.status () == WL_CONNECTED ? "yes" : "no");
}

// Manage network disconnection
void onSTADisconnected (WiFiEventStationModeDisconnected event_info) {
    Serial.printf ("Disconnected from SSID: %s\n", event_info.ssid.c_str ());
    Serial.printf ("Reason: %d\n", event_info.reason);
    isSTAConnected = false;
    isSTATryingConnect = false;
    isAPMode = false;
    isAPConnected = false;
}

bool ssidIsConfigured()
{
    return strlen(Settings.wifi_ssid) != 0;
}

void WiFiSetup()
{
  static WiFiEventHandler e1, e2, e3;
  e1 = WiFi.onStationModeGotIP (onSTAGotIP);
  e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
  e3 = WiFi.onStationModeConnected (onSTAConnected);
}

void InitializeAP()
{

}


void InitializeSTA()
{
    AddLog(LOG_LEVEL_INFO, "Initializing WiFi in STA Mode");
    ("WiFi network ssid: " + String(Settings.wifi_ssid)).toCharArray(log_data, sizeof(log_data));
    AddLog(LOG_LEVEL_INFO);
    if (Settings.network_ip != 0)
    {
        WiFi.config(Settings.network_ip, Settings.network_gateway, Settings.network_subnetmask, Settings.network_dns);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(Settings.wifi_ssid, Settings.wifi_pwd);
}


void WiFiLoop()
{
    AddLog(LOG_LEVEL_DEBUG, "WiFiLoop");

    if (isSTAConnected)
    {
        // Do nothing
    }
    else if (!ssidIsConfigured())
    {
        if (!isAPMode)
        {
            isAPMode = true;
            InitializeAP();
        }
    }
    else if (isSTATryingConnect)
    {
        isSTATryingConnect = false;
        isAPMode = true;
        lastEvent = NTP.getUptime();
        InitializeAP();
    }
    else if (isAPMode && ssidIsConfigured())
    {
        isSTATryingConnect = true;
        isAPMode = false;
        lastEvent = NTP.getUptime();
        InitializeSTA();
    }

}