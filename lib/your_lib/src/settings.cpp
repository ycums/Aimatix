#include "settings.h"

// Global variables
Settings settings = {true, 100, 0};  // デフォルト設定

// 設定のロード
void loadSettings(IEEPROM* eeprom) {
  if (!eeprom) return;
  // EEPROMから設定値を読み出し
  settings.sound_enabled = eeprom->read(SETTINGS_ADDR) != 0;
  settings.lcd_brightness = eeprom->read(SETTINGS_ADDR + 1);
  settings.checksum = eeprom->read(SETTINGS_ADDR + 2);
  // チェックサム検証
  char calculatedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  if (settings.checksum != calculatedChecksum) {
    // チェックサム不一致時はデフォルト値
    settings.sound_enabled = true;
    settings.lcd_brightness = 100;
    settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  }
}

// 設定の保存
void saveSettings(IEEPROM* eeprom) {
  if (!eeprom) return;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  eeprom->write(SETTINGS_ADDR, settings.sound_enabled ? 1 : 0);
  eeprom->write(SETTINGS_ADDR + 1, settings.lcd_brightness);
  eeprom->write(SETTINGS_ADDR + 2, settings.checksum);
  eeprom->commit();
}

// 設定のリセット
void resetSettings(IEEPROM* eeprom) {
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  saveSettings(eeprom);
}

// 設定の整合性チェック
bool validateSettings(IEEPROM* eeprom) {
  char calculatedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  return (settings.checksum == calculatedChecksum);
}
