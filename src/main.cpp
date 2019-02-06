#include <Arduino.h>
#include "main.h"
#include "serial.h"
#include "log.h"
#include "properties.h"
#include "settings.h"

void setup() {
  Serial.begin(DEFAULTBAUDRATE);
  delay(10);
  Serial.println();
  
  Serial.printf("%s V%s - %s\n", PGM_NAME, PGM_VERSION, PGM_COPYRIGHT);

  SettingsLoad();
  SettingsMigrate();
}

void loop() {
  // put your main code here, to run repeatedly:
}