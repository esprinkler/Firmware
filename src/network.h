#include <Arduino.h>

#ifndef _NETWORK_H_
#define _NETWORK_H_

enum WifiConfigOptions {
    WIFI_RESTART, 
    WIFI_SMARTCONFIG, 
    WIFI_MANAGER, 
    WIFI_WPSCONFIG, 
    WIFI_RETRY, 
    WIFI_WAIT, 
    WIFI_SERIAL, 
    MAX_WIFI_OPTION};



bool ParseIp(uint32_t* addr, const char* str);

#endif