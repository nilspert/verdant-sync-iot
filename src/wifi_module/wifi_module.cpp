/**
 * File: wifi_module.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of WifiModule.
 * Enables connection to WiFi.
 * Uses ESP8266WiFi library.
 */

#include "wifi_module.h"
#include "../../config/config.h"

void wifiModuleInit() {
    // WiFi initialization code
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");
}

// Function to get the unique identifier (MAC address) for the board
String getBoardId() {
    return WiFi.macAddress();
}

// Function to get current network name (SSID)
String getNetworkName() {
    return WiFi.SSID();
}

// Function to get device IP address
String getLocalIpAsString() {
    return WiFi.localIP().toString();
}
