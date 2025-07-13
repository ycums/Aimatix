#ifndef ARDUINO_H
#define ARDUINO_H

#ifdef MOCK_M5STACK

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// Arduino types
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

// Windows環境でのtime_t型の競合を回避
#ifdef _WIN32
  #ifndef _TIME_T_DECLARED
    #define _TIME_T_DECLARED
  #endif
#else
  #ifndef _TIME_T_DECLARED
    typedef long time_t;
    #define _TIME_T_DECLARED
  #endif
#endif

// Arduino constants
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// Arduino functions
unsigned long millis();
void delay(unsigned long ms);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);

// Mock time functions
#ifdef _WIN32
  // Windows環境ではtimevalを再定義しない
  #ifndef _STRUCT_TIMEVAL
    #define _STRUCT_TIMEVAL
  #endif
#else
  #ifndef _STRUCT_TIMEVAL
  #ifndef __timeval_defined
  struct timeval {
      long tv_sec;
      long tv_usec;
  };
  #define __timeval_defined
  #endif
  #endif
#endif

int settimeofday(const struct timeval* tv, void* tz) { return 0; }

// Mock IPAddress class
class IPAddress {
public:
    IPAddress() : addr{0, 0, 0, 0} {}
    IPAddress(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) 
        : addr{first, second, third, fourth} {}
    
    uint8_t operator[](int index) const { return addr[index]; }
    uint8_t& operator[](int index) { return addr[index]; }
    
private:
    uint8_t addr[4];
};

// Mock Serial class
class SerialClass {
public:
    SerialClass() {}
    void begin(unsigned long baud) {}
    void print(const char* str) {}
    void print(int val) {}
    void print(unsigned long val) {}
    void print(long val) {}
    void print(long long val) {}
    void print(size_t val) {}
    void println(const char* str) {}
    void println(int val) {}
    void println(unsigned long val) {}
    void println(long val) {}
    void println(long long val) {}
    void println(size_t val) {}
    void println() {}
};

extern SerialClass Serial;

// Arduino string functions
class String {
public:
    String() {}
    String(const char* str) : str_(str ? str : "") {}
    String(const String& other) : str_(other.str_) {}
    
    const char* c_str() const { return str_.c_str(); }
    int length() const { return str_.length(); }
    
    String& operator=(const String& other) {
        str_ = other.str_;
        return *this;
    }
    
    String& operator+=(const String& other) {
        str_ += other.str_;
        return *this;
    }
    
    String operator+(const String& other) const {
        return String((str_ + other.str_).c_str());
    }
    
    bool equals(const String& other) const {
        return str_ == other.str_;
    }
    
    bool equals(const char* str) const {
        return str_ == (str ? str : "");
    }
    
    // Comparison operators for std::map
    bool operator<(const String& other) const {
        return str_ < other.str_;
    }
    
    bool operator==(const String& other) const {
        return str_ == other.str_;
    }
    
private:
    std::string str_;
};

// Mock WiFi class
class WiFiClass {
public:
    static void begin(const char* ssid, const char* password) {}
    static int status() { return WL_CONNECTED; }
    static IPAddress localIP() { return IPAddress(192, 168, 1, 100); }
};

extern WiFiClass WiFi;

// Mock EEPROM class
class EEPROMClass {
public:
    void begin(size_t size) {}
    uint8_t read(int address) { return 0; }
    void write(int address, uint8_t value) {}
    void commit() {}
    size_t length() { return 512; }
};

extern EEPROMClass EEPROM;

// Mock Preferences class
class Preferences {
public:
    bool begin(const char* name, bool readOnly = false) { return true; }
    void end() {}
    bool putString(const char* key, const char* value) { return true; }
    bool putInt(const char* key, int32_t value) { return true; }
    bool putBool(const char* key, bool value) { return true; }
    bool putUChar(const char* key, uint8_t value) { return true; }
    String getString(const char* key, const char* defaultValue = "") { return String(defaultValue); }
    int32_t getInt(const char* key, int32_t defaultValue = 0) { return defaultValue; }
    bool getBool(const char* key, bool defaultValue = false) { return defaultValue; }
    uint8_t getUChar(const char* key, uint8_t defaultValue = 0) { return defaultValue; }
    bool remove(const char* key) { return true; }
    bool clear() { return true; }
};

// WiFi status constants
#define WL_CONNECTED 3
#define WL_DISCONNECTED 6
#define WL_NO_SSID_AVAIL 7
#define WL_SCAN_COMPLETED 8
#define WL_CONNECT_FAILED 9
#define WL_WRONG_PASSWORD 10
#define WL_IDLE_STATUS 11

#else
// Include real Arduino library
#include <Arduino.h>
#endif

#endif // ARDUINO_H 