/**
 * File: globals.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains reusable functions.
 */

#include "globals.h"

// Function for event creation
void handleEvent(const char* severity, const char* message, const char* eventType) {
    // Call eventModule createAndEnqueueEvent to create and enqueue event for sending to firebase
    eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), DEVICE_NAME, severity, DEVICE, message, getNetworkName(), eventType);
}
