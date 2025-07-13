#ifndef ARDUINO_H
#define ARDUINO_H

#ifdef MOCK_M5STACK

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

// Arduino types
typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
#ifndef _TIME_T_DECLARED
typedef long time_t;
#define _TIME_T_DECLARED
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
#ifndef _STRUCT_TIMEVAL
#ifndef __timeval_defined
struct timeval {
    long tv_sec;
    long tv_usec;
};
#define __timeval_defined
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
    void println(const char* str) {}
    void println(int val) {}
    void println(unsigned long val) {}
    void println(long val) {}
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

#else
// Include real Arduino library
#include <Arduino.h>
#endif

#endif // ARDUINO_H 