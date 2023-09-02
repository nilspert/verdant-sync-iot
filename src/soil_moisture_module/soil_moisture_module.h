#ifndef SOIL_MOISTURE_MODULE_H
#define SOIL_MOISTURE_MODULE_H

#include <ESP8266WiFi.h>

class SoilMoistureModule {
public:
    bool encryptAndSendSoilMoisture(int soilMoisture, const String& boardId);
};

#endif
