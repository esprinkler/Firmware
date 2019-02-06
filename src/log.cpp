#include <Arduino.h>
#include "log.h"
#include "settings.h"

void AddLog(LogLevel level, const char* value)
{
    if ((Settings.serial_loglevel & level) != 0)
        Serial.println(value);
}