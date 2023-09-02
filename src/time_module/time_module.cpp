#include <NTPClient.h>
#include <WiFiUdp.h> // Required for NTPClient
#include <TimeLib.h>
#include <AESLib.h>
#include "../../config/config.h"
#include "time_module.h"

const long TIMEZONE_OFFSET = 3 * 3600; // +3:00 in seconds

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void timeModuleInit() {
    timeClient.begin();
}

void updateTime() {
    timeClient.update();
}

unsigned long getCurrentEpochTime() {
    return timeClient.getEpochTime();
}

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

    // Format the date as YYYY/MM/DD
    String formattedDate = String(year) + "/" + (month < 10 ? "0" : "") + String(month) + "/" + (day < 10 ? "0" : "") + String(day) + "/";

    Serial.println("Date:");
    Serial.println(formattedDate);
    
    return formattedDate;
}

// Function to get current time as a formatted string
String getCurrentTimeAsString() {
    updateTime();
    // Get current epoch time
    time_t currentEpochTime = timeClient.getEpochTime();
    return String(currentEpochTime);
}
