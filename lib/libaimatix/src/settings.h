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
  uint8_t checksum;  // 設定が有効かどうかを確認するためのチェックサム
};

// Function prototypes - 依存性注入パターンに従った設計
void loadSettings(IEEPROM* eeprom, Settings& settings);
void saveSettings(IEEPROM* eeprom, const Settings& settings);
void resetSettings(IEEPROM* eeprom, Settings& settings);
bool validateSettings(const Settings& settings);

// 後方互換性のためのグローバル変数（段階的移行用）
extern Settings settings;

// 後方互換性のための関数（段階的移行用）
void loadSettings(IEEPROM* eeprom);
void saveSettings(IEEPROM* eeprom);
void resetSettings(IEEPROM* eeprom);
bool validateSettings(IEEPROM* eeprom);

#endif // SETTINGS_H
