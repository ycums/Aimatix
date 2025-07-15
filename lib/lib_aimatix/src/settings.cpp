#include "settings.h"
#include <Preferences.h>
#include <M5Stack.h>

// Global variables
Settings settings = {true, 100, 0};  // デフォルト設定

// Preferencesインスタンス
static Preferences preferences;

// 定数定義
static const char* PREF_NAMESPACE = "settings";
static const char* PREF_SOUND_KEY = "sound";
static const char* PREF_BRIGHTNESS_KEY = "brightness";
static const char* PREF_CHECKSUM_KEY = "checksum";

void loadSettings() {
  // Preferencesの初期化
  if (!preferences.begin(PREF_NAMESPACE, false)) {
    Serial.println("Failed to initialize settings preferences");
    return;
  }
  
  // 保存された設定を読み込み
  bool soundEnabled = preferences.getBool(PREF_SOUND_KEY, true);
  uint8_t brightness = preferences.getUChar(PREF_BRIGHTNESS_KEY, 100);
  char checksum = preferences.getChar(PREF_CHECKSUM_KEY, 0);
  
  // チェックサムの検証
  char calculatedChecksum = (soundEnabled + brightness) % 256;
  if (checksum == calculatedChecksum) {
    settings.sound_enabled = soundEnabled;
    settings.lcd_brightness = brightness;
    settings.checksum = checksum;
    Serial.println("Settings loaded successfully from Preferences");
  } else {
    Serial.println("Settings checksum mismatch, using defaults");
    // デフォルト設定を使用
    settings.sound_enabled = true;
    settings.lcd_brightness = 100;
    settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  }
  
  Serial.printf("Loaded settings - Sound: %s, Brightness: %d\n", 
                settings.sound_enabled ? "ON" : "OFF", settings.lcd_brightness);
}

void saveSettings() {
  // チェックサムの計算
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  // Preferencesに保存
  preferences.putBool(PREF_SOUND_KEY, settings.sound_enabled);
  preferences.putUChar(PREF_BRIGHTNESS_KEY, settings.lcd_brightness);
  preferences.putChar(PREF_CHECKSUM_KEY, settings.checksum);
  
  Serial.printf("Settings saved - Sound: %s, Brightness: %d\n", 
                settings.sound_enabled ? "ON" : "OFF", settings.lcd_brightness);
}

// 設定のリセット機能
void resetSettings() {
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  // Preferencesをクリアしてデフォルト値を保存
  preferences.clear();
  saveSettings();
  
  Serial.println("Settings reset to defaults");
}

// 設定の整合性チェック
bool validateSettings() {
  char calculatedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  bool isValid = (settings.checksum == calculatedChecksum);
  
  if (!isValid) {
    Serial.println("Settings validation failed");
  }
  
  return isValid;
}
