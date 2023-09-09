#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

class DeviceManager {
public:
    void setup();
    void loop();

private:
    void initModules();
    void registerDeviceForAuthorization(String boardId);
    int readSoilMoistureSensor();
    void handleEvent(const char* severity, const char* message); 
    void activateWaterPump(bool activate);
};

#endif