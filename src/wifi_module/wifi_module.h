#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <ESP8266WiFi.h>

void wifiModuleInit();
String getBoardId();
String getNetworkName();
String getLocalIpAsString();

#endif
