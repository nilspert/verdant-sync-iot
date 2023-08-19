#include "event_module.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../firebase_module/firebase_module.h"
#include "../aes_module/aes_module.h"
#include "../../config/config.h"

// Enqueue an event into the circular buffer
bool EventModule::enqueueEvent(const Event &event) {
    if (numEvents < MAX_EVENTS) {
        eventBuffer[bufferEnd] = event;
        bufferEnd = (bufferEnd + 1) % MAX_EVENTS;
        numEvents++;
        return true;
    }
    return false; // Buffer is full
}

// Dequeue the next event from the circular buffer
bool EventModule::dequeueEvent(Event &event) {
    if (numEvents > 0) {
        event = eventBuffer[bufferStart];
        bufferStart = (bufferStart + 1) % MAX_EVENTS;
        numEvents--;
        return true;
    }
    return false; // No events in the buffer
}

void EventModule::createAndEnqueueEvent(
    const String& timestamp, 
    const String& hostname, 
    const String& severity, 
    const String& facility, 
    const String& message, 
    const String& ssid
) {
    Event event = createEvent(timestamp, hostname, severity, facility, message, ssid);
    if (enqueueEvent(event)) {
        Serial.println("Event enqueued successfully.");
    } else {
        Serial.println("Event queue is full. Event not enqueued.");
    }
}

Event EventModule::createEvent(
  const String& timestamp, 
  const String& hostname, 
  const String& severity, 
  const String& facility, 
  const String& message, 
  const String& ssid
  ) {
    Event event;
    event.timestamp = timestamp;
    event.hostname = hostname;
    event.severity = severity;
    event.facility = facility;
    event.message = message;
    event.messageId = generateMessageId();
    event.ssid = ssid;
    return event;
}

String EventModule::generateMessageId() {
    String messageId = getBoardId() + ":" + getCurrentTimeAsString();
    return messageId;
}

void EventModule::sendEventToFirebase(const Event &event) {
    // Generate a UUID string
    String messageId = generateMessageId();
    
    // Gather and encrypt event information
    char encryptedHostname[INPUT_BUFFER_LIMIT] = {0};
    char encryptedSeverity[INPUT_BUFFER_LIMIT] = {0};
    char encryptedFacility[INPUT_BUFFER_LIMIT] = {0};
    char encryptedMessage[INPUT_BUFFER_LIMIT] = {0};
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};

    gatherAndEcryptEventInformation(
      event, 
      encryptedHostname, 
      encryptedSeverity, 
      encryptedFacility, 
      encryptedMessage, 
      encryptedWifiSSID
    );
    
    // Create a FirebaseJson object to hold the data
    FirebaseJson json;

    // Add data to the JSON object using identifiers
    json.set(messageId + "/timestamp", event.timestamp);
    json.set(messageId + "/hostname", encryptedHostname);
    json.set(messageId + "/severity", encryptedSeverity);
    json.set(messageId + "/facility", encryptedFacility);
    json.set(messageId + "/message", encryptedMessage);
    json.set(messageId + "/messageId", messageId.c_str());
    json.set(messageId + "/ssid", encryptedWifiSSID);

    String encryptedWifiSSIDString(encryptedWifiSSID); 
    String nodePath = "events/" + event.severity + "/" + getFormattedDate() + encryptedWifiSSIDString + "/";
    if (sendFirebaseData(json, nodePath.c_str())) {
        Serial.println("Event data sent successfully.");
    } else {
        Serial.println("Failed to send event data.");
    }
}

void EventModule::gatherAndEcryptEventInformation(
  const Event &event, 
  char* encryptedHostname, 
  char* encryptedSeverity, 
  char* encryptedFacility, 
  char* encryptedMessage, 
  char* encryptedWifiSSID
) {
    Serial.println("Encrypting event:");
    Serial.println(event.message);

    byte temp_enc_iv[N_BLOCK]; // Temporary IV for each encryption

    generateNewIV(temp_enc_iv, enc_ivs[9]);
    encryptAndConvertToHex(event.hostname.c_str(), encryptedHostname, temp_enc_iv);

    generateNewIV(temp_enc_iv, enc_ivs[10]);
    encryptAndConvertToHex(event.severity.c_str(), encryptedSeverity, temp_enc_iv);

    generateNewIV(temp_enc_iv, enc_ivs[11]);
    encryptAndConvertToHex(event.facility.c_str(), encryptedFacility, temp_enc_iv);

    generateNewIV(temp_enc_iv, enc_ivs[12]);
    encryptAndConvertToHex(event.message.c_str(), encryptedMessage, temp_enc_iv);

    generateNewIV(temp_enc_iv, enc_ivs[13]);
    encryptAndConvertToHex(event.ssid.c_str(), encryptedWifiSSID, temp_enc_iv);
}

void EventModule::loop() {
    // Check if there are events in the queue
    if (numEvents > 0) {
        Event nextEvent;
        if (dequeueEvent(nextEvent)) {
            // Send the next event
            sendEventToFirebase(nextEvent);
        }
    }
}