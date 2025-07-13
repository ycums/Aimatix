#include "Arduino.h"

#ifdef MOCK_M5STACK

// Mock Arduino functions
void digitalWrite(uint8_t pin, uint8_t val) {
    // Mock digitalWrite - do nothing in test environment
    (void)pin;
    (void)val;
}

int digitalRead(uint8_t pin) {
    // Mock digitalRead - return LOW by default
    (void)pin;
    return LOW;
}

void pinMode(uint8_t pin, uint8_t mode) {
    // Mock pinMode - do nothing in test environment
    (void)pin;
    (void)mode;
}

// Global Serial instance
SerialClass Serial;

#endif // MOCK_M5STACK 