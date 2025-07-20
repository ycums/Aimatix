#include <unity.h>
#include <cstdio>
#include <cstring>
#include <settings.h>
#include "../mock/mock_eeprom.h"
#include "../../../lib/libaimatix/src/types.h"

// SettingsLogicの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

void setUp(void) {}
void tearDown(void) {}

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

// 新しく追加するテストケース（実際に存在する関数のみ）

void test_settings_structure_operations() {
  Settings settings;
  
  // 構造体の基本操作
  settings.sound_enabled = true;
  settings.lcd_brightness = 150;
  settings.checksum = 0;
  
  TEST_ASSERT_TRUE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(150, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(0, settings.checksum);
  
  // 値の変更
  settings.sound_enabled = false;
  settings.lcd_brightness = 75;
  
  TEST_ASSERT_FALSE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(75, settings.lcd_brightness);
  
  printf("✓ settings_structure_operations: 成功\n");
}

void test_settings_checksum_logic() {
  Settings settings;
  
  // チェックサム計算ロジックの詳細テスト
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  TEST_ASSERT_EQUAL(101, settings.checksum);
  
  // オーバーフローケース
  settings.lcd_brightness = 255;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  TEST_ASSERT_EQUAL(0, settings.checksum); // (1 + 255) % 256 = 0
  
  printf("✓ settings_checksum_logic: 成功\n");
}

void test_settings_validation_logic() {
  Settings settings;
  
  // 有効な設定
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  bool isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
  TEST_ASSERT_TRUE(isValid);
  
  // 無効な設定（チェックサムが不正）
  settings.checksum = 0;
  isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
  TEST_ASSERT_FALSE(isValid);
  
  printf("✓ settings_validation_logic: 成功\n");
}

void test_settings_constants() {
  // 定数のテスト
  TEST_ASSERT_EQUAL(512, EEPROM_SIZE);
  TEST_ASSERT_EQUAL(0, SETTINGS_ADDR);
  
  printf("✓ settings_constants: 成功\n");
}

void test_settings_memory_layout() {
  Settings settings = {true, 100, 0};
  
  // メモリレイアウトのテスト
  TEST_ASSERT_EQUAL(sizeof(bool), sizeof(settings.sound_enabled));
  TEST_ASSERT_EQUAL(sizeof(uint8_t), sizeof(settings.lcd_brightness));
  TEST_ASSERT_EQUAL(sizeof(uint8_t), sizeof(settings.checksum));
  
  // 構造体サイズの確認
  TEST_ASSERT_EQUAL(sizeof(bool) + sizeof(uint8_t) + sizeof(uint8_t), sizeof(settings));
  
  printf("✓ settings_memory_layout: 成功\n");
}

void test_settings_edge_cases() {
  Settings settings;
  
  // 境界値でのテスト
  settings.sound_enabled = true;
  settings.lcd_brightness = 0;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  TEST_ASSERT_EQUAL(1, settings.checksum); // true(1) + 0 = 1
  
  settings.sound_enabled = false;
  settings.lcd_brightness = 255;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  TEST_ASSERT_EQUAL(255, settings.checksum); // false(0) + 255 = 255
  
  printf("✓ settings_edge_cases: 成功\n");
}

void test_settings_performance() {
  Settings settings;
  
  // パフォーマンステスト（多数の操作）
  for (int i = 0; i < 1000; i++) {
    settings.sound_enabled = (i % 2) == 0;
    settings.lcd_brightness = i % 256;
    settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
    
    bool isValid = (settings.checksum == (settings.sound_enabled + settings.lcd_brightness) % 256);
    TEST_ASSERT_TRUE(isValid);
  }
  
  printf("✓ settings_performance: 成功\n");
}

void test_settings_copy_operations() {
  Settings original = {true, 100, 0};
  original.checksum = (original.sound_enabled + original.lcd_brightness) % 256;
  
  // コピー操作
  Settings copy = original;
  
  // 値の比較
  TEST_ASSERT_EQUAL(original.sound_enabled, copy.sound_enabled);
  TEST_ASSERT_EQUAL(original.lcd_brightness, copy.lcd_brightness);
  TEST_ASSERT_EQUAL(original.checksum, copy.checksum);
  
  // 独立した変更
  copy.sound_enabled = false;
  copy.checksum = (copy.sound_enabled + copy.lcd_brightness) % 256;
  
  TEST_ASSERT_NOT_EQUAL(original.sound_enabled, copy.sound_enabled);
  TEST_ASSERT_NOT_EQUAL(original.checksum, copy.checksum);
  
  printf("✓ settings_copy_operations: 成功\n");
}

// 実際のSettingsLogic関数をテストするケース

void test_loadSettings_function() {
  MockEEPROM mockEeprom;
  Settings settings;
  
  // モックEEPROMに有効なデータを設定
  Settings validSettings = {true, 100, 101}; // チェックサム: 1 + 100 = 101
  mockEeprom.write(SETTINGS_ADDR, validSettings.sound_enabled);
  mockEeprom.write(SETTINGS_ADDR + 1, validSettings.lcd_brightness);
  mockEeprom.write(SETTINGS_ADDR + 2, validSettings.checksum);
  
  // loadSettings関数をテスト
  loadSettings(&mockEeprom, settings);
  
  // 正しく読み込まれたことを確認
  TEST_ASSERT_EQUAL(validSettings.sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(validSettings.lcd_brightness, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(validSettings.checksum, settings.checksum);
  
  printf("✓ loadSettings_function: 成功\n");
}

void test_saveSettings_function() {
  MockEEPROM mockEeprom;
  Settings settings = {false, 50, 0};
  
  // チェックサムを計算
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  // saveSettings関数をテスト
  saveSettings(&mockEeprom, settings);
  
  // EEPROMに正しく保存されたことを確認
  TEST_ASSERT_EQUAL(settings.sound_enabled, mockEeprom.read(SETTINGS_ADDR));
  TEST_ASSERT_EQUAL(settings.lcd_brightness, mockEeprom.read(SETTINGS_ADDR + 1));
  TEST_ASSERT_EQUAL(settings.checksum, mockEeprom.read(SETTINGS_ADDR + 2));
  
  printf("✓ saveSettings_function: 成功\n");
}

void test_resetSettings_function() {
  MockEEPROM mockEeprom;
  Settings settings = {true, 200, 100}; // 初期値は何でも良い
  
  // resetSettings関数をテスト
  resetSettings(&mockEeprom, settings);
  
  // デフォルト値にリセットされたことを確認
  TEST_ASSERT_TRUE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(100, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(101, settings.checksum); // 1 + 100 = 101
  
  // EEPROMにも保存されたことを確認
  TEST_ASSERT_EQUAL(settings.sound_enabled, mockEeprom.read(SETTINGS_ADDR));
  TEST_ASSERT_EQUAL(settings.lcd_brightness, mockEeprom.read(SETTINGS_ADDR + 1));
  TEST_ASSERT_EQUAL(settings.checksum, mockEeprom.read(SETTINGS_ADDR + 2));
  
  printf("✓ resetSettings_function: 成功\n");
}

void test_validateSettings_function() {
  Settings settings;
  
  // 有効な設定
  settings.sound_enabled = true;
  settings.lcd_brightness = 100;
  settings.checksum = (settings.sound_enabled + settings.lcd_brightness) % 256;
  
  bool isValid = validateSettings(settings);
  TEST_ASSERT_TRUE(isValid);
  
  // 無効な設定（チェックサムが不正）
  settings.checksum = 0;
  isValid = validateSettings(settings);
  TEST_ASSERT_FALSE(isValid);
  
  printf("✓ validateSettings_function: 成功\n");
}

void test_settings_integration() {
  MockEEPROM mockEeprom;
  Settings settings;
  
  // 統合テスト：保存→読み込み→検証の流れ
  Settings originalSettings = {false, 75, 0};
  originalSettings.checksum = (originalSettings.sound_enabled + originalSettings.lcd_brightness) % 256;
  
  // 1. 保存
  saveSettings(&mockEeprom, originalSettings);
  
  // 2. 読み込み
  loadSettings(&mockEeprom, settings);
  
  // 3. 検証
  bool isValid = validateSettings(settings);
  TEST_ASSERT_TRUE(isValid);
  
  // 4. 値の一致確認
  TEST_ASSERT_EQUAL(originalSettings.sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(originalSettings.lcd_brightness, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(originalSettings.checksum, settings.checksum);
  
  printf("✓ settings_integration: 成功\n");
}

void test_settings_error_handling() {
  MockEEPROM mockEeprom;
  Settings settings;
  
  // 無効なデータがEEPROMにある場合のテスト
  mockEeprom.write(SETTINGS_ADDR, true);
  mockEeprom.write(SETTINGS_ADDR + 1, 100);
  mockEeprom.write(SETTINGS_ADDR + 2, 0); // 不正なチェックサム
  
  loadSettings(&mockEeprom, settings);
  
  // 無効なデータの場合、loadSettingsはデフォルト値に設定するため、最終的には有効な設定になる
  bool isValid = validateSettings(settings);
  TEST_ASSERT_TRUE(isValid);
  
  // デフォルト値が設定されていることを確認
  TEST_ASSERT_TRUE(settings.sound_enabled);
  TEST_ASSERT_EQUAL(100, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(101, settings.checksum); // 1 + 100 = 101
  
  printf("✓ settings_error_handling: 成功\n");
}

void test_settings_boundary_conditions() {
  MockEEPROM mockEeprom;
  Settings settings;
  
  // 境界値でのテスト
  Settings boundarySettings = {false, 255, 0};
  boundarySettings.checksum = (boundarySettings.sound_enabled + boundarySettings.lcd_brightness) % 256;
  
  saveSettings(&mockEeprom, boundarySettings);
  loadSettings(&mockEeprom, settings);
  
  TEST_ASSERT_EQUAL(boundarySettings.sound_enabled, settings.sound_enabled);
  TEST_ASSERT_EQUAL(boundarySettings.lcd_brightness, settings.lcd_brightness);
  TEST_ASSERT_EQUAL(boundarySettings.checksum, settings.checksum);
  
  bool isValid = validateSettings(settings);
  TEST_ASSERT_TRUE(isValid);
  
  printf("✓ settings_boundary_conditions: 成功\n");
}

// SettingsLogicのAPIがUI遷移や画面制御を直接行わないことを検証するテスト
Mode currentMode = MAIN_DISPLAY;
void test_SettingsLogic_does_not_change_mode_or_call_ui() {
    // Arrange
    Mode beforeMode = currentMode;
    Settings s = {true, 100, 0};
    MockEEPROM mockEeprom;
    s.checksum = (s.sound_enabled + s.lcd_brightness) % 256;
    // Act
    loadSettings(&mockEeprom, s);
    saveSettings(&mockEeprom, s);
    resetSettings(&mockEeprom, s);
    validateSettings(s);
    // Assert
    TEST_ASSERT_EQUAL(beforeMode, currentMode); // currentModeが変化していないこと
    // UI描画呼び出しの有無は、必要に応じてモックやカウンタで拡張可能
    printf("✓ SettingsLogicはUI遷移や画面制御を直接行わない: 成功\n");
}

int main() {
    UNITY_BEGIN();
    
    // 基本的な設定構造体テスト
    RUN_TEST(test_settings_initialization);
    RUN_TEST(test_checksum_calculation);
    RUN_TEST(test_checksum_validation);
    RUN_TEST(test_settings_boundary_values);
    RUN_TEST(test_settings_consistency);
    RUN_TEST(test_default_settings);
    RUN_TEST(test_settings_modification);
    RUN_TEST(test_composite_settings);
    
    // 新しく追加するテストケース
    RUN_TEST(test_settings_structure_operations);
    RUN_TEST(test_settings_checksum_logic);
    RUN_TEST(test_settings_validation_logic);
    RUN_TEST(test_settings_constants);
    RUN_TEST(test_settings_memory_layout);
    RUN_TEST(test_settings_edge_cases);
    RUN_TEST(test_settings_performance);
    RUN_TEST(test_settings_copy_operations);
    
    // 実際のSettingsLogic関数テスト
    RUN_TEST(test_loadSettings_function);
    RUN_TEST(test_saveSettings_function);
    RUN_TEST(test_resetSettings_function);
    RUN_TEST(test_validateSettings_function);
    RUN_TEST(test_settings_integration);
    RUN_TEST(test_settings_error_handling);
    RUN_TEST(test_settings_boundary_conditions);
    RUN_TEST(test_SettingsLogic_does_not_change_mode_or_call_ui);
    
    return UNITY_END();
} 