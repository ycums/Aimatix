#include "settings.h"

// Global variables
Settings settings = {true, 100, 0};  // デフォルト設定

// 依存性注入パターンに従った新しい実装

// 設定のロード
void loadSettings(IEEPROM* eeprom, Settings& settings) {
  if (!eeprom) return;
  // EEPROMから設定値を読み出し
  settings.sound_enabled = eeprom->read(SETTINGS_ADDR) != 0;
  settings.lcd_brightness = eeprom->read(SETTINGS_ADDR + 1);
  settings.checksum = eeprom->read(SETTINGS_ADDR + 2);
  // チェックサム検証
  uint8_t calculatedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  bool valid = (settings.checksum == calculatedChecksum)
               && (settings.lcd_brightness >= 1 && settings.lcd_brightness <= 255)
               && (settings.sound_enabled == 0 || settings.sound_enabled == 1);
  if (!valid) {
      // 不正値時はデフォルト値
      settings.sound_enabled = true;
      settings.lcd_brightness = 100;
      settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  }
}

// 設定の保存
void saveSettings(IEEPROM* eeprom, const Settings& settings) {
  if (!eeprom) return;
  uint8_t checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  eeprom->write(SETTINGS_ADDR, settings.sound_enabled ? 1 : 0);
  eeprom->write(SETTINGS_ADDR + 1, settings.lcd_brightness);
  eeprom->write(SETTINGS_ADDR + 2, checksum);
  eeprom->commit();
}

// 設定のリセット
void resetSettings(IEEPROM* eeprom, Settings& settings) {
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  saveSettings(eeprom, settings);
}

// 設定の整合性チェック
bool validateSettings(const Settings& settings) {
  uint8_t calculatedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  return (settings.checksum == calculatedChecksum);
}

// 後方互換性のための実装（段階的移行用）

// 設定のロード（グローバル変数版）
void loadSettings(IEEPROM* eeprom) {
  loadSettings(eeprom, settings);
}

// 設定の保存（グローバル変数版）
void saveSettings(IEEPROM* eeprom) {
  saveSettings(eeprom, settings);
}

// 設定のリセット（グローバル変数版）
void resetSettings(IEEPROM* eeprom) {
  resetSettings(eeprom, settings);
}

// 設定の整合性チェック（グローバル変数版）
bool validateSettings(IEEPROM* eeprom) {
  return validateSettings(settings);
}
