#include "main_display.h"
#ifdef ARDUINO
#include <M5Stack.h>
#endif

void setup() {
#ifdef ARDUINO
    M5.begin();
#endif
    drawMainDisplay();
}

void loop() {
    // 何もしない（雛形）
} 