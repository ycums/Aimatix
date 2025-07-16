#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstdint>
#include "../include/IEEPROM.h"

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
void loadSettings(IEEPROM* eeprom);
void saveSettings(IEEPROM* eeprom);
void resetSettings(IEEPROM* eeprom);
bool validateSettings(IEEPROM* eeprom);

#endif // SETTINGS_H
