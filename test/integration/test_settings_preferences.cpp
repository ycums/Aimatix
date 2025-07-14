#include <unity.h>
#include <Preferences.h>
#include <EEPROM.h>
#include "settings.h"

// モック設定
extern "C" {
  // Preferences関連のモック関数
  bool mock_preferences_begin = true;
  bool mock_preferences_put_bool = true;
  bool mock_preferences_put_uchar = true;
  bool mock_preferences_put_char = true;
  bool mock_preferences_get_bool = true;
  bool mock_preferences_get_uchar = true;
  bool mock_preferences_get_char = true;
  bool mock_preferences_clear = true;
  
  // EEPROM関連のモック関数
  bool mock_eeprom_begin = true;
  bool mock_eeprom_put = true;
  bool mock_eeprom_get = true;
  bool mock_eeprom_commit = true;
  
  // 保存された値のモック
  bool mock_stored_sound_enabled = true;
  uint8_t mock_stored_brightness = 100;
  char mock_stored_checksum = 0;
  
  // デフォルト値
  bool mock_default_sound_enabled = true;
  uint8_t mock_default_brightness = 100;
}

// テスト用の設定
Settings testSettings;

void setUp(void) {
  // テスト前の初期化
  mock_preferences_begin = true;
  mock_preferences_put_bool = true;
  mock_preferences_put_uchar = true;
  mock_preferences_put_char = true;
  mock_preferences_get_bool = true;
  mock_preferences_get_uchar = true;
  mock_preferences_get_char = true;
  mock_preferences_clear = true;
  mock_eeprom_begin = true;
  mock_eeprom_put = true;
  mock_eeprom_get = true;
  mock_eeprom_commit = true;
  
  // デフォルト値の設定
  mock_stored_sound_enabled = true;
  mock_stored_brightness = 100;
  mock_stored_checksum = (mock_stored_sound_enabled + mock_stored_brightness) % 256;
  
  // テスト用設定の初期化
  testSettings.sound_enabled = true;
  testSettings.lcd_brightness = 100;
  testSettings.checksum = (testSettings.sound_enabled + testSettings.lcd_brightness) % 256;
}

void tearDown(void) {
  // テスト後のクリーンアップ
}

// テストケース1: Preferences初期化テスト
void test_preferences_initialization() {
  // 正常な初期化
  mock_preferences_begin = true;
  loadSettings();
  
  // 設定が正しく読み込まれているかチェック
  TEST_ASSERT_EQUAL(mock_default_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_default_brightness, settings.lcd_brightness);
}

// テストケース2: 設定保存テスト
void test_settings_save() {
  // テスト用の設定値
  settings.sound_enabled = false;
  settings.lcd_brightness = 50;
  
  // 設定保存
  saveSettings();
  
  // チェックサムが正しく計算されているかチェック
  char expectedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(expectedChecksum, settings.checksum);
}

// テストケース3: 設定読み込みテスト（正常ケース）
void test_settings_load_success() {
  // 正常な保存された値のモック設定
  mock_stored_sound_enabled = false;
  mock_stored_brightness = 75;
  mock_stored_checksum = (mock_stored_sound_enabled + mock_stored_brightness) % 256;
  
  // 設定読み込み
  loadSettings();
  
  // 保存された値が正しく読み込まれているかチェック
  TEST_ASSERT_EQUAL(mock_stored_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_stored_brightness, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(mock_stored_checksum, settings.checksum);
}

// テストケース4: 設定読み込みテスト（チェックサム不一致）
void test_settings_load_checksum_mismatch() {
  // チェックサム不一致のモック設定
  mock_stored_sound_enabled = false;
  mock_stored_brightness = 75;
  mock_stored_checksum = 0; // 不正なチェックサム
  
  // 設定読み込み
  loadSettings();
  
  // デフォルト値が使用されているかチェック
  TEST_ASSERT_EQUAL(mock_default_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_default_brightness, settings.lcd_brightness);
  
  // 正しいチェックサムが計算されているかチェック
  char expectedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(expectedChecksum, settings.checksum);
}

// テストケース5: 設定リセットテスト
void test_settings_reset() {
  // 初期設定
  settings.sound_enabled = false;
  settings.lcd_brightness = 50;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  // 設定リセット
  resetSettings();
  
  // デフォルト値にリセットされているかチェック
  TEST_ASSERT_EQUAL(mock_default_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_default_brightness, settings.lcd_brightness);
  
  // 正しいチェックサムが計算されているかチェック
  char expectedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(expectedChecksum, settings.checksum);
}

// テストケース6: 設定整合性チェックテスト
void test_settings_validation() {
  // 正常な設定
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  bool isValid = validateSettings();
  TEST_ASSERT_TRUE(isValid);
  
  // 不正な設定（チェックサム不一致）
  settings.checksum = 0;
  isValid = validateSettings();
  TEST_ASSERT_FALSE(isValid);
}

// テストケース7: Preferences初期化失敗テスト
void test_preferences_initialization_failure() {
  // Preferences初期化失敗のモック設定
  mock_preferences_begin = false;
  
  // 設定読み込み
  loadSettings();
  
  // デフォルト値が使用されているかチェック
  TEST_ASSERT_EQUAL(mock_default_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_default_brightness, settings.lcd_brightness);
}

// テストケース8: 境界値テスト
void test_boundary_values() {
  // 明度の境界値テスト
  settings.lcd_brightness = 0;
  settings.sound_enabled = true;
  saveSettings();
  
  char expectedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(expectedChecksum, settings.checksum);
  
  // 明度の最大値テスト
  settings.lcd_brightness = 255;
  settings.sound_enabled = false;
  saveSettings();
  
  expectedChecksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(expectedChecksum, settings.checksum);
}

// テストケース9: 複数回保存・読み込みテスト
void test_multiple_save_load() {
  // 1回目の保存・読み込み
  settings.sound_enabled = false;
  settings.lcd_brightness = 75;
  saveSettings();
  
  mock_stored_sound_enabled = false;
  mock_stored_brightness = 75;
  mock_stored_checksum = (mock_stored_sound_enabled + mock_stored_brightness) % 256;
  
  loadSettings();
  TEST_ASSERT_EQUAL(false, settings.sound_enabled);
  TEST_ASSERT_EQUAL(75, settings.lcd_brightness);
  
  // 2回目の保存・読み込み
  settings.sound_enabled = true;
  settings.lcd_brightness = 150;
  saveSettings();
  
  mock_stored_sound_enabled = true;
  mock_stored_brightness = 150;
  mock_stored_checksum = (mock_stored_sound_enabled + mock_stored_brightness) % 256;
  
  loadSettings();
  TEST_ASSERT_EQUAL(true, settings.sound_enabled);
  TEST_ASSERT_EQUAL(150, settings.lcd_brightness);
}

// テストケース10: エラーハンドリングテスト
void test_error_handling() {
  // Preferences保存失敗のテスト
  mock_preferences_put_bool = false;
  mock_preferences_put_uchar = false;
  mock_preferences_put_char = false;
  
  // 設定保存（失敗してもエラーで停止しない）
  settings.sound_enabled = false;
  settings.lcd_brightness = 50;
  saveSettings();
  
  // 設定読み込み失敗のテスト
  mock_preferences_get_bool = false;
  mock_preferences_get_uchar = false;
  mock_preferences_get_char = false;
  
  // 設定読み込み（失敗してもデフォルト値を使用）
  loadSettings();
  TEST_ASSERT_EQUAL(mock_default_sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(mock_default_brightness, settings.lcd_brightness);
}

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_preferences_initialization);
  RUN_TEST(test_settings_save);
  RUN_TEST(test_settings_load_success);
  RUN_TEST(test_settings_load_checksum_mismatch);
  RUN_TEST(test_settings_reset);
  RUN_TEST(test_settings_validation);
  RUN_TEST(test_preferences_initialization_failure);
  RUN_TEST(test_boundary_values);
  RUN_TEST(test_multiple_save_load);
  RUN_TEST(test_error_handling);
  
  UNITY_END();
}

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
} 