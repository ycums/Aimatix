#include "settings.h"

// Global variables
Settings settings = {true, 100, 0};  // デフォルト設定

void loadSettings() {
  Settings stored;
  EEPROM.get(SETTINGS_ADDR, stored);
  
  // チェックサムが一致する場合のみ設定を読み込む
  if (stored.checksum == (stored.sound_enabled + stored.lcd_brightness) % 256) {
    settings = stored;
  }
}

void saveSettings() {
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  EEPROM.put(SETTINGS_ADDR, settings);
  EEPROM.commit();
}
