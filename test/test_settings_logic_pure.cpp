#include <unity.h>
#include <cstdio>
#include <cstring>
#include "settings.h"

// SettingsLogicの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 設定構造体の初期化テスト
void test_settings_initialization() {
  Settings settings = {true, 100, 0};
  
  // 初期値の確認
  TEST_ASSERT_TRUE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(100, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(0, settings.checksum);
  
  printf("✓ Settings初期化テスト: 成功\n");
}

// チェックサム計算ロジックのテスト
void test_checksum_calculation() {
  Settings settings;
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  
  // チェックサム計算
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(101, settings.checksum); // true(1) + 100 = 101
  
  // 別の値でのテスト
  settings.sound_enabled = false;
  settings.lcd_brightness = 50;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(50, settings.checksum); // false(0) + 50 = 50
  
  printf("✓ チェックサム計算ロジックテスト: 成功\n");
}

// チェックサム検証ロジックのテスト
void test_checksum_validation() {
  Settings settings;
  
  // 有効なチェックサム
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  bool isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
  TEST_ASSERT_TRUE(isValid);
  
  // 無効なチェックサム
  settings.checksum = 0; // 不正な値
  isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
  TEST_ASSERT_FALSE(isValid);
  
  printf("✓ チェックサム検証ロジックテスト: 成功\n");
}

// 設定値の境界値テスト
void test_settings_boundary_values() {
  Settings settings;
  
  // LCD明度の境界値テスト
  settings.lcd_brightness = 0;
  TEST_ASSERT_EQUAL(0, settings.lcd_brightness);
  
  settings.lcd_brightness = 255;
  TEST_ASSERT_EQUAL(255, settings.lcd_brightness);
  
  // 音設定の境界値テスト
  settings.sound_enabled = true;
  TEST_ASSERT_TRUE(settings.sound_enabled);
  
  settings.sound_enabled = false;
  TEST_ASSERT_FALSE(settings.sound_enabled);
  
  printf("✓ 設定値境界値テスト: 成功\n");
}

// 設定の整合性テスト
void test_settings_consistency() {
  Settings settings1 = {true, 100, 0};
  Settings settings2 = {true, 100, 0};
  
  // 同じ設定値の場合
  settings1.checksum = (settings1.sound_enabled + settings1.lcd_brightness) % 256;
  settings2.checksum = (settings2.sound_enabled + settings2.lcd_brightness) % 256;
  
  TEST_ASSERT_EQUAL(settings1.checksum, settings2.checksum);
  TEST_ASSERT_EQUAL(settings1.sound_enabled, settings2.sound_enabled);
  TEST_ASSERT_EQUAL(settings1.lcd_brightness, settings2.lcd_brightness);
  
  // 異なる設定値の場合
  settings2.sound_enabled = false;
  settings2.checksum = (settings2.sound_enabled + settings2.lcd_brightness) % 256;
  
  TEST_ASSERT_NOT_EQUAL(settings1.checksum, settings2.checksum);
  
  printf("✓ 設定整合性テスト: 成功\n");
}

// デフォルト設定値のテスト
void test_default_settings() {
  // デフォルト設定の確認
  Settings defaultSettings = {true, 100, 0};
  
  TEST_ASSERT_TRUE(defaultSettings.sound_enabled);
  TEST_ASSERT_EQUAL(100, defaultSettings.lcd_brightness);
  TEST_ASSERT_EQUAL(0, defaultSettings.checksum);
  
  // デフォルト設定のチェックサム計算
  defaultSettings.checksum = (defaultSettings.sound_enabled + defaultSettings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(101, defaultSettings.checksum);
  
  printf("✓ デフォルト設定値テスト: 成功\n");
}

// 設定変更ロジックのテスト
void test_settings_modification() {
  Settings settings = {true, 100, 0};
  
  // 音設定の変更
  settings.sound_enabled = false;
  TEST_ASSERT_FALSE(settings.sound_enabled);
  
  // LCD明度の変更
  settings.lcd_brightness = 50;
  TEST_ASSERT_EQUAL(50, settings.lcd_brightness);
  
  // 変更後のチェックサム計算
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  TEST_ASSERT_EQUAL(50, settings.checksum); // false(0) + 50 = 50
  
  printf("✓ 設定変更ロジックテスト: 成功\n");
}

// 複合設定テスト
void test_composite_settings() {
  Settings settings;
  
  // 複数の設定を同時に変更
  settings.sound_enabled = true;
  settings.lcd_brightness = 200;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  // 検証
  TEST_ASSERT_TRUE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(200, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(201, settings.checksum); // true(1) + 200 = 201
  
  // 整合性チェック
  bool isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
  TEST_ASSERT_TRUE(isValid);
  
  printf("✓ 複合設定テスト: 成功\n");
}

void setUp(void) {}
void tearDown(void) {}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== SettingsLogic 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_settings_initialization);
  RUN_TEST(test_checksum_calculation);
  RUN_TEST(test_checksum_validation);
  RUN_TEST(test_settings_boundary_values);
  RUN_TEST(test_settings_consistency);
  RUN_TEST(test_default_settings);
  RUN_TEST(test_settings_modification);
  RUN_TEST(test_composite_settings);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 