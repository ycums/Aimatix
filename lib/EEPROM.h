#ifndef EEPROM_H
#define EEPROM_H

#ifdef MOCK_M5STACK

#include "Arduino.h"

// Mock EEPROM class
class EEPROMClass {
public:
    EEPROMClass() {}
    
    // Read/Write methods
    uint8_t read(int address) { return 0; }
    void write(int address, uint8_t value) {}
    
    // Update method
    void update(int address, uint8_t value) {}
    
    // Get/Put methods for different data types
    template<typename T>
    T& get(int address, T& data) { return data; }
    
    template<typename T>
    const T& put(int address, const T& data) { return data; }
    
    // Commit method (for ESP32)
    bool commit() { return true; }
    
    // Begin method (for ESP32)
    bool begin(size_t size) { return true; }
    
    // Length method
    size_t length() { return 512; }
};

// Global EEPROM instance
extern EEPROMClass EEPROM;

#else
// Include real EEPROM library
#include <EEPROM.h>
#endif

#endif // EEPROM_H 