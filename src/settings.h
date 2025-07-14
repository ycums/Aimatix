#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include <M5Stack.h>
#include <cstdint>

// Constants
#define EEPROM_SIZE 512
#define SETTINGS_ADDR 0

// Settings structure
struct Settings {
  bool sound_enabled;
  uint8_t lcd_brightness;
  char checksum;  // 設定が有効かどうかを確認するためのチェックサム
};

// Global variables (declared in settings.cpp)
extern Settings settings;

// Function prototypes
void loadSettings();
void saveSettings();
void resetSettings();
bool validateSettings();

#endif // SETTINGS_H
