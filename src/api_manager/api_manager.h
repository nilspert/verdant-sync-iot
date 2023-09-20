#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <ESP8266WiFi.h>
#include "../firebase_module/firebase_module.h"

class ApiManager {
public:
    bool encryptAndSendDeviceRegistration(const String& boardId, const String& networkName);
    bool encryptAndSendBoardInfo(const String& boardId, const String& networkName, const String& localIp);
    bool encryptAndSendTemperature(float temperature, const String& boardId);
    bool encryptAndSendHumidity(float humidity, const String& boardId);
    bool encryptAndSendAirPressure(float airPressure, const String& boardId);
    bool encryptAndSendSoilMoisture(int soilMoisture, const String& boardId);
private: 
    bool handleApiCall(FirebaseJson json, const String& nodePath);
};

#endif
