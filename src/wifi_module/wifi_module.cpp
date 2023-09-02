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

String getNetworkName() {
    return WiFi.SSID();
}

String getLocalIpAsString() {
    return WiFi.localIP().toString();
}
