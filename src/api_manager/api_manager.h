#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <ESP8266WiFi.h>
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../firebase_module/firebase_module.h"

class ApiManager {
public:
    bool encryptAndSendDeviceRegistration(const String& boardId, const String& networkName);
    bool encryptAndSendBoardInfo(const String& boardId, const String& networkName, const String& localIp);
    bool encryptAndSendTemperature(float temperature, const String& boardId, const String& networkName);
    bool encryptAndSendHumidity(float humidity, const String& boardId, const String& networkName);
    bool encryptAndSendAirPressure(float airPressure, const String& boardId, const String& networkName);
    bool encryptAndSendSoilMoisture(int soilMoisture, const String& boardId, const String& networkName);
    bool encryptAndSendLuminosity(float luminosity, const String& boardId, const String& networkName);
    bool encryptAndSendWaterTankLevel(float waterTankLevel, const String& boardId, const String& networkName);
private: 
    bool setupApiCallWithHistoryData(const String& boardId, const String& networkName, FirebaseJson json, const String& nodePathKey);
    bool handleApiCall(FirebaseJson json, const String& nodePath);
};

#endif
