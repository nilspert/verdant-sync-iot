#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <ESP8266WiFi.h>

class DeviceManager {
public:
    void setup();
    void loop();

private:
    void initModules();
    void registerDeviceForAuthorization(String boardId);
    int readSoilMoistureSensor();
    void handleEvent(const char* severity, const char* message); 
};

#endif