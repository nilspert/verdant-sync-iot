#ifndef EVENT_MODULE_H
#define EVENT_MODULE_H

#include <ESP8266WiFi.h>

struct Event {
    String timestamp;
    String hostname;
    String severity;
    String facility;
    String message;
    String messageId;
    String ssid;
};

const int MAX_EVENTS = 10;

class EventModule {
public:
    void loop();
    void createAndEnqueueEvent(const String& timestamp, const String& hostname, 
                               const String& severity, const String& facility, 
                               const String& message, const String& ssid);

private:
    Event eventBuffer[MAX_EVENTS];
    int bufferStart = 0;
    int bufferEnd = 0;
    int numEvents = 0;

    bool enqueueEvent(const Event &event);
    bool dequeueEvent(Event &event);
    Event createEvent(const String& timestamp, const String& hostname, const String& severity,
                      const String& facility, const String& message, const String& ssid);
    String generateMessageId();
    void sendEventToFirebase(const Event &event);
    void gatherAndEcryptEventInformation(const Event &event, char* encryptedHostname,
                                         char* encryptedSeverity, char* encryptedFacility,
                                         char* encryptedMessage, char* encryptedWifiSSID);
};

#endif