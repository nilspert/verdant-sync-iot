#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

#define DHT_TYPE DHT22

class DeviceManager {
public:
    void setup();
    void loop();

private:
    void initModules();
    void registerDeviceForAuthorization(String boardId);
    void readAndSendTemperature(String boardId, String networkName);
    void readAndSendHumidity(String boardId, String networkName);
    void readAndSendAirPressure(String boardId, String networkName);
    void readAndSendLuminosity(String boardId, String networkName);
    int readAndSendSoilMoisture(String boardId, String networkName);
    float readAndSendWaterTankLevel(String boardId, String networkName);
    int readSoilMoistureSensor();
    int readPhotoresistor();
    bool checkSoilStatus(int soilMoisture);
    void handleEvent(const char* severity, const char* message, const char* eventType); 
    void activateWaterPump(bool activate);
};

#endif