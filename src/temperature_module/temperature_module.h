#ifndef TEMPERATURE_MODULE_H
#define TEMPERATURE_MODULE_H

#include <ESP8266WiFi.h>
#include "../event_module/event_module.h"

class TemperatureModule {
public:
    void readAndSendTemperatureData(const String& boardId, EventModule eventModule);
private:
    void encryptAndSendTemperature(float temperature, const String& boardId, EventModule eventModule);
};

#endif
