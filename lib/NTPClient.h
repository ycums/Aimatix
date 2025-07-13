#ifndef NTPCLIENT_H
#define NTPCLIENT_H

#ifdef MOCK_M5STACK

#include "Arduino.h"

// Forward declaration
class WiFiUDP;

// Mock NTPClient class
class NTPClient {
public:
    NTPClient() {}
    NTPClient(const char* poolServerName) {}
    NTPClient(const char* poolServerName, long timeOffset) {}
    NTPClient(const char* poolServerName, long timeOffset, long updateInterval) {}
    NTPClient(WiFiUDP& udp, const char* poolServerName, long timeOffset, long updateInterval) {}
    
    void begin() {}
    void setTimeOffset(int timeOffset) {}
    void setUpdateInterval(long updateInterval) {}
    void forceUpdate() {}
    void update() {}
    
    String getFormattedTime() { return "12:00:00"; }
    String getFormattedDate() { return "2024-01-01"; }
    unsigned long getEpochTime() { return 1704067200; } // 2024-01-01 00:00:00
    
    int getHours() { return 12; }
    int getMinutes() { return 0; }
    int getSeconds() { return 0; }
    int getDay() { return 1; }
    int getMonth() { return 1; }
    int getYear() { return 2024; }
};

#else
// Include real NTPClient library
#include <NTPClient.h>
#endif

#endif // NTPCLIENT_H 