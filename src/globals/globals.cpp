#include "globals.h"
// Function for event creation
void handleEvent(const char* severity, const char* message, const char* eventType) {
    // Call eventModule createAndEnqueueEvent to create and enqueue event for sending to firebase
    eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, severity, DEVICE, message, getNetworkName(), eventType);
}