/**
 * File: event_module.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of EventModule.
 * Provides functionality for creating and sending events to firebase.
 * Uses circular event buffer for enqueuing and dequeuing events.
 */

#include "event_module.h"
#include "../../config/config.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../firebase_module/firebase_module.h"
#include "../aes_module/aes_module.h"

// Function for enqueuing an event into the circular buffer
bool EventModule::enqueueEvent(const Event &event) {
    if (numEvents < MAX_EVENTS) {
        eventBuffer[bufferEnd] = event;
        bufferEnd = (bufferEnd + 1) % MAX_EVENTS;
        numEvents++;
        return true;
    }
    return false; // Buffer is full
}

// Function for dequeuing the next event from the circular buffer
bool EventModule::dequeueEvent(Event &event) {
    if (numEvents > 0) {
        event = eventBuffer[bufferStart];
        bufferStart = (bufferStart + 1) % MAX_EVENTS;
        numEvents--;
        return true;
    }
    return false; // No events in the buffer
}

// Function for creating and enqueuing event
void EventModule::createAndEnqueueEvent(
    const String& timestamp, 
    const String& hostname, 
    const String& severity, 
    const String& facility, 
    const String& message, 
    const String& ssid,
    const String& eventType
) {
    Event event = createEvent(timestamp, hostname, severity, facility, message, ssid, eventType);
    if (enqueueEvent(event)) {
        Serial.println("Event enqueued successfully.");
    } else {
        Serial.println("Event queue is full. Event not enqueued.");
    }
}

// Function for creating event struct
Event EventModule::createEvent(
  const String& timestamp, 
  const String& hostname, 
  const String& severity, 
  const String& facility, 
  const String& message, 
  const String& ssid,
  const String& eventType
  ) {
    Event event;
    event.timestamp = timestamp;
    event.hostname = hostname;
    event.severity = severity;
    event.facility = facility;
    event.message = message;
    event.messageId = generateMessageId(eventType);
    event.ssid = ssid;
    return event;
}

// Function for generating messageId
String EventModule::generateMessageId(const String& eventType) {
    String messageId = getCurrentTimeAsString() + ":" + eventType;
    return messageId;
}

// Function for sending event to firebase
void EventModule::sendEventToFirebase(const Event &event) {    
    char encryptedHostname[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted host name
    char encryptedSeverity[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted severity
    char encryptedFacility[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted facility
    char encryptedMessage[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted message
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted WiFi SSID

    // Gather and encrypt event information
    gatherAndEcryptEventInformation(
      event, 
      encryptedHostname, 
      encryptedSeverity, 
      encryptedFacility, 
      encryptedMessage, 
      encryptedWifiSSID
    );
    
    FirebaseJson json; // Create a FirebaseJson object to hold the data

    // Add encrypted data to the JSON object using identifiers
    json.set(event.messageId + "/timestamp", event.timestamp);
    json.set(event.messageId + "/hostname", encryptedHostname);
    json.set(event.messageId + "/severity", encryptedSeverity);
    json.set(event.messageId + "/facility", encryptedFacility);
    json.set(event.messageId + "/message", encryptedMessage);
    json.set(event.messageId + "/messageId", event.messageId.c_str());
    json.set(event.messageId + "/ssid", encryptedWifiSSID);

    String encryptedWifiSSIDString(encryptedWifiSSID); // Create String object to hold the encrypted WiFi SSID data

    // Create nodepath and send data to firebase
    String nodePath = "events/" + event.severity + "/" + getFormattedDate() + encryptedWifiSSIDString + "/" + getBoardId() + "/";
    if (sendFirebaseData(json, nodePath.c_str())) {
        Serial.println("Event data sent successfully.");
    } else {
        Serial.println("Failed to send event data.");
    }
}

// Function for encrypting event data
void EventModule::gatherAndEcryptEventInformation(
  const Event &event, 
  char* encryptedHostname, 
  char* encryptedSeverity, 
  char* encryptedFacility, 
  char* encryptedMessage, 
  char* encryptedWifiSSID
) {
    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector

    generateNewIV(temp_enc_iv, enc_ivs[9]); // Generate a new IV for encryption
    encryptAndConvertToHex(event.hostname.c_str(), encryptedHostname, temp_enc_iv); // Encrypt hostname and convert to hex

    generateNewIV(temp_enc_iv, enc_ivs[10]); // Generate a new IV for encryption
    encryptAndConvertToHex(event.severity.c_str(), encryptedSeverity, temp_enc_iv); // Encrypt severity and convert to hex

    generateNewIV(temp_enc_iv, enc_ivs[11]); // Generate a new IV for encryption
    encryptAndConvertToHex(event.facility.c_str(), encryptedFacility, temp_enc_iv); // Encrypt facility and convert to hex

    generateNewIV(temp_enc_iv, enc_ivs[12]); // Generate a new IV for encryption
    encryptAndConvertToHex(event.message.c_str(), encryptedMessage, temp_enc_iv); // Encrypt message and convert to hex

    generateNewIV(temp_enc_iv, enc_ivs[13]); // Generate a new IV for encryption
    encryptAndConvertToHex(event.ssid.c_str(), encryptedWifiSSID, temp_enc_iv); // Encrypt network name and convert to hex
}

// Function to process and send events to firebase
void EventModule::loop() {
    // Check if there are events in the queue
    if (numEvents > 0) {
        Serial.println("Event count: ");
        Serial.println(numEvents);
        Event nextEvent;
        if (dequeueEvent(nextEvent)) {
            // Send the next event
            sendEventToFirebase(nextEvent);
        }
    }
}
