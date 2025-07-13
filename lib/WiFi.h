#ifndef WIFI_H
#define WIFI_H

#ifdef MOCK_M5STACK

#include "Arduino.h"

// WiFi status constants
#define WL_CONNECTED 3
#define WL_DISCONNECTED 6
#define WL_NO_SSID_AVAIL 1
#define WL_SCAN_COMPLETED 2
#define WL_CONNECT_FAILED 4
#define WL_WRONG_PASSWORD 5
#define WL_IDLE_STATUS 0

// Mock WiFi class
class WiFiClass {
public:
    WiFiClass() {}
    
    // Connection methods
    bool begin(const char* ssid, const char* password) { return true; }
    bool begin(const char* ssid, const char* password, int32_t channel, const uint8_t* bssid, bool connect) { return true; }
    void disconnect(bool wifioff = false, bool eraseap = false) {}
    
    // Status methods
    bool isConnected() { return true; }
    int status() { return WL_CONNECTED; }
    
    // IP methods
    String localIP() { return "192.168.1.100"; }
    String gatewayIP() { return "192.168.1.1"; }
    String subnetMask() { return "255.255.255.0"; }
    String dnsIP() { return "8.8.8.8"; }
    
    // MAC methods
    String macAddress() { return "AA:BB:CC:DD:EE:FF"; }
    
    // RSSI methods
    int32_t RSSI() { return -50; }
};

// Global WiFi instance
extern WiFiClass WiFi;

#else
// Include real WiFi library
#include <WiFi.h>
#endif

#endif // WIFI_H 