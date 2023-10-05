/**
 * File: event_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of EventModule.
 * Holds function declarations and constants for event operations.
 */

#ifndef EVENT_MODULE_H
#define EVENT_MODULE_H

#include <ESP8266WiFi.h>

// Structure to represent an event
struct Event {
    String timestamp;
    String hostname;
    String severity;
    String facility;
    String message;
    String messageId;
    String ssid;
};

const int MAX_EVENTS = 10; // Maximum number of events that can be stored

class EventModule {
public:
    // Function declaration for loop
    void loop();
    
    // Function declaration for createAndEnqueueEvent
    void createAndEnqueueEvent(const String& timestamp, const String& hostname,
                               const String& severity, const String& facility,
                               const String& message, const String& ssid,
                               const String& eventType); 

private:
    Event eventBuffer[MAX_EVENTS]; // Circular buffer to store events
    int bufferStart = 0; // Index for the start of buffer
    int bufferEnd = 0; // Index for the end of buffer
    int numEvents = 0; // Number of events in buffer

    // Function declaration for enqueueEvent
    bool enqueueEvent(const Event &event);

    // Function declaration for dequeueEvent
    bool dequeueEvent(Event &event);

    // Function declaration for generateMessageId
    String generateMessageId(const String& eventType);

    // Function declaration for sendEventToFirebase
    void sendEventToFirebase(const Event &event);

    // Function declaration for createEvent
    Event createEvent(const String& timestamp, const String& hostname, const String& severity,
                      const String& facility, const String& message, const String& ssid,
                      const String& eventType);

    // Function declaration for gatherAndEcryptEventInformation
    void gatherAndEcryptEventInformation(const Event &event, char* encryptedHostname,
                                         char* encryptedSeverity, char* encryptedFacility,
                                         char* encryptedMessage, char* encryptedWifiSSID);
};

#endif
