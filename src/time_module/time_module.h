#ifndef TIME_MODULE_H
#define TIME_MODULE_H

#include <ESP8266WiFi.h>

void timeModuleInit();
void updateTime();
unsigned long getCurrentEpochTime();
String getFormattedDate();
String getCurrentTimeAsString();

#endif
