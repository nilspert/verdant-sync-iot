/**
 * File: api_manager.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of ApiManager.
 * Holds function declarations and constants for API setup call operations.
 */

#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <ESP8266WiFi.h>
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../firebase_module/firebase_module.h"

class ApiManager {
public:
    // API operations
    bool encryptAndSendDeviceRegistration(const String& boardId, const String& networkName);
    bool encryptAndSendBoardInfo(const String& boardId, const String& networkName, const String& localIp);
    bool encryptAndSendTemperature(float temperature, const String& boardId, const String& networkName);
    bool encryptAndSendHumidity(float humidity, const String& boardId, const String& networkName);
    bool encryptAndSendAirPressure(float airPressure, const String& boardId, const String& networkName);
    bool encryptAndSendSoilMoisture(int soilMoisture, const String& boardId, const String& networkName);
    bool encryptAndSendLuminosity(float luminosity, const String& boardId, const String& networkName);
    bool encryptAndSendWaterTankLevel(float waterTankLevel, const String& boardId, const String& networkName);
    bool encryptAndSendLatestWateringTime(const String& currentTime, const String& boardId, const String& networkName);
    bool encryptAndSendWaterTankRefillNotification(const String& currentTime, const String& boardId, const String& networkName);
private: 
    // API node path keys
    const char* AIR_PRESSURE_KEY = "air_pressure";
    const char* HUMIDITY_KEY = "humidity";
    const char* LUMINOSITY_KEY = "luminosity";
    const char* SOIL_MOISTURE_KEY = "soil_moisture";
    const char* TEMPERATURE_KEY = "temperature";
    const char* WATER_TANK_LEVEL_KEY = "water_tank_level";
    const char* LATEST_WATERING_TIME_KEY = "latest_watering_time";
    const char* WATER_TANK_REFILL_NOTIFICATION_KEY = "refill_water_tank";

    // API setup functions
    bool setupApiCallWithHistoryData(const String& boardId, const String& networkName, FirebaseJson json, const String& nodePathKey);
    bool handleApiCall(FirebaseJson json, const String& nodePath);
};

#endif
