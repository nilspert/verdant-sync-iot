/**
 * File: time_module.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of TimeModule.
 * Enables the ESP8266 device to synchronize its time with an NTP (Network Time Protocol) server hosted at "pool.ntp.org" over WiFi.
 * Uses NTPClient library.
 */

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <AESLib.h>
#include "../../config/config.h"
#include "time_module.h"

const long TIMEZONE_OFFSET = 3 * 3600; // +3:00 hours

WiFiUDP ntpUDP; // Create a UDP client for NTP (Network Time Protocol) communication
NTPClient timeClient(ntpUDP, "pool.ntp.org");  // Create an NTPClient instance with the UDP client and set the NTP server

// Initialize the time module
void timeModuleInit() {
    timeClient.begin();
}

// Update the current time from the NTP server
void updateTime() {
    timeClient.update();
}

// Get the current epoch time in seconds
unsigned long getCurrentEpochTime() {
    return timeClient.getEpochTime();
}

// Get the formatted date as "YYYY/MM/DD" with the current time adjusted for the timezone offset
String getFormattedDate() {
    updateTime();

    // Get the current epoch time
    unsigned long epochTime = timeClient.getEpochTime() + TIMEZONE_OFFSET;

    // Convert epoch time to a time_t
    time_t now = static_cast<time_t>(epochTime);

    // Create a TimeElements structure
    TimeElements timeElements;
    breakTime(now, timeElements);

    // Extract year, month, and day
    int year = timeElements.Year + 1970; // Year count starts from 1970
    int month = timeElements.Month;
    int day = timeElements.Day;
    int hour = timeElements.Hour;

    // Format the date as "YYYY/MM/DD" and return it
    String formattedDate = String(year) + "/" + (month < 10 ? "0" : "") + String(month) + "/" + (day < 10 ? "0" : "") + String(day) + "/";
    return formattedDate;
}

// Get the current time as a formatted string (epoch time)
String getCurrentTimeAsString() {
    updateTime();
    // Get current epoch time
    time_t currentEpochTime = timeClient.getEpochTime();
    return String(currentEpochTime);
}
