#include <Arduino.h>
#include "log.h"
#include "settings.h"

char log_data[LOGDATASIZE];

void AddLog(LogLevel level, const char* value)
{
    if ((Settings.serial_loglevel & level) != 0)
        Serial.println(value);
}

void AddLog(LogLevel level, String value)
{
    if ((Settings.serial_loglevel & level) != 0)
        Serial.println(value);
}

void AddLog(LogLevel level)
{
    if ((Settings.serial_loglevel & level) != 0)
        Serial.println(log_data);
}