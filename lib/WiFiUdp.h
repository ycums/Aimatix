#ifndef WIFIUDP_H
#define WIFIUDP_H

#ifdef MOCK_M5STACK

#include "Arduino.h"

// Mock WiFiUDP class
class WiFiUDP {
public:
    WiFiUDP() {}
    
    // UDP methods
    uint8_t begin(uint16_t port) { return 1; }
    void stop() {}
    int beginPacket(const char* host, uint16_t port) { return 1; }
    int beginPacket(IPAddress ip, uint16_t port) { return 1; }
    int endPacket() { return 1; }
    size_t write(uint8_t byte) { return 1; }
    size_t write(const uint8_t* buffer, size_t size) { return size; }
    int parsePacket() { return 0; }
    int available() { return 0; }
    int read() { return -1; }
    int read(unsigned char* buffer, size_t len) { return 0; }
    int read(char* buffer, size_t len) { return 0; }
    int peek() { return -1; }
    void flush() {}
    IPAddress remoteIP() { return IPAddress(192, 168, 1, 1); }
    uint16_t remotePort() { return 123; }
};

#else
// Include real WiFiUDP library
#include <WiFiUdp.h>
#endif

#endif // WIFIUDP_H 