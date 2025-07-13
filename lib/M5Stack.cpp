#include "M5Stack.h"

#ifdef MOCK_M5STACK

// Global M5Stack instance
M5Stack M5;

// Mock Arduino functions
unsigned long millis() {
    // Return a mock time value for testing
    static unsigned long mockTime = 0;
    return mockTime += 100; // Increment by 100ms each call
}

void delay(unsigned long ms) {
    // Mock delay - do nothing in test environment
    (void)ms; // Suppress unused parameter warning
}

#endif // MOCK_M5STACK 