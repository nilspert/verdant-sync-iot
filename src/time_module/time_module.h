/**
 * File: time_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of TimeModule.
 * Holds function declarations for time operations.
 */

#ifndef TIME_MODULE_H
#define TIME_MODULE_H

#include <ESP8266WiFi.h>

void timeModuleInit(); // Initialize the TimeModule
void updateTime(); // Update the current time
unsigned long getCurrentEpochTime(); // Get the current epoch time
String getFormattedDate(); // Get the formatted date as a string (YYYY/MM/DD)
String getCurrentTimeAsString(); // Get the current time as a formatted string (epoch time)

#endif

