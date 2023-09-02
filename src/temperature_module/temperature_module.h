#ifndef TEMPERATURE_MODULE_H
#define TEMPERATURE_MODULE_H

#include <ESP8266WiFi.h>

class TemperatureModule {
public:
    bool encryptAndSendTemperature(float temperature, const String& boardId);
};

#endif
