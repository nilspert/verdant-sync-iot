/**
 * File: event_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of EventModule.INFO
 * Holds function declarations and constants for event operations.
 */

#ifndef EVENT_MODULE_H
#define EVENT_MODULE_H

#include <ESP8266WiFi.h>

// Events
#define INFO "INFO"
#define WARNING "WARNING"
#define ERROR "ERROR"
#define DEVICE "DEVICE"

// Event types
#define REGISTRATION "0x0"
#define BOARD_INFO "0x1"
#define TEMPERATURE "0x2"
#define HUMIDITY "0x3"
#define AIR_PRESSURE "0x4"
#define SOIL_MOISTURE "0x5"
#define SOIL_MOISTURE_INFO "0x6"
#define LUMINOSITY "0x7"
#define WATER_TANK_LEVEL "0x8"
#define LATEST_WATERING_TIME "0x9"
#define WATER_TANK_REFILL_NOTIFICATION "0xA"

// Event messages
#define ADD_BOARD_INFO_ERROR_MESSAGE "Failed to add board information."
#define ADD_AUTHORIZED_DEVICE_PENDING_MESSAGE "Device is pending authorization. Data sending disabled."
#define ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE "Failed to add device to authorized devices"
#define SEND_TEMPERATURE_ERROR_MESSAGE "Failed to send temperature data."
#define SEND_HUMIDITY_ERROR_MESSAGE "Failed to send humidity data."
#define SEND_AIR_PRESSURE_ERROR_MESSAGE "Failed to send air pressure data."
#define SEND_SOIL_MOISTURE_ERROR_MESSAGE "Failed to send soil moisture data."
#define SEND_LUMINOSITY_ERROR_MESSAGE "Failed to send luminosity data."
#define SEND_WATER_TANK_LEVEL_ERROR_MESSAGE "Failed to send water tank level data."
#define SEND_LATEST_WATERING_TIME_ERROR_MESSAGE "Failed to send latest watering time."
#define SEND_WATER_TANK_REFILL_NOTIFICATION_ERROR_MESSAGE "Failed to send water tank refill notification."
#define SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE "Status: high soil moisture."
#define SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE "Status: optimal soil moisture."
#define SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE "Status: low soil moisture."

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
