/**
 * File: wifi_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of WifiModule.
 * Holds function declarations for wifi operations.
 */

#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <ESP8266WiFi.h>

void wifiModuleInit(); // Initialize the WiFi module
String getBoardId(); // Get the unique board identifier
String getNetworkName(); // Get the network name (SSID)
String getLocalIpAsString(); // Get the local IP address as a string

#endif
