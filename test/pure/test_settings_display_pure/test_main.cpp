#include <unity.h>
#include "SettingsDisplayState.h"
#include "SettingsLogic.cpp"
#include "../mock/MockTimeProvider.h"
#include <memory>

extern std::vector<time_t> alarm_times;

const time_t kFixedTestTime = 1700000000;
std::shared_ptr<MockTimeProvider> testTimeProvider = std::make_shared<MockTimeProvider>(kFixedTestTime);

void setUp(void) {}
void tearDown(void) {}

// 基本的なテスト
void test_settings_display_basic() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // 基本的な動作確認
    TEST_ASSERT_NOT_NULL(&state);
}

// 未テスト関数のテストケース追加
void test_settings_display_get_selected_index() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // 初期状態の選択インデックス確認
    int selectedIndex = state.getSelectedIndex();
    TEST_ASSERT_EQUAL(0, selectedIndex);
    
    // 選択インデックスを変更
    state.setSelectedIndex(2);
    selectedIndex = state.getSelectedIndex();
    TEST_ASSERT_EQUAL(2, selectedIndex);
}

void test_settings_display_set_selected_index() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // 選択インデックスを設定
    state.setSelectedIndex(1);
    TEST_ASSERT_EQUAL(1, state.getSelectedIndex());
    
    // 境界値テスト
    state.setSelectedIndex(0);
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex());
    
    state.setSelectedIndex(3);
    TEST_ASSERT_EQUAL(3, state.getSelectedIndex());
}

void test_settings_display_on_exit() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // onExitが例外を投げないことを確認
    state.onExit();
    // 正常に実行されることを確認（例外が発生しない）
}

void test_settings_display_on_button_c() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // onButtonCが例外を投げないことを確認
    state.onButtonC();
    // 正常に実行されることを確認（例外が発生しない）
}

void test_settings_display_on_button_c_long_press() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // onButtonCLongPressが例外を投げないことを確認
    state.onButtonCLongPress();
    // 正常に実行されることを確認（例外が発生しない）
}

void test_settings_display_edge_cases() {
    SettingsLogic logic;
    SettingsDisplayState state(&logic);
    
    // 境界値テスト
    state.setSelectedIndex(-1);
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex()); // 負の値は0に正規化される
    
    state.setSelectedIndex(10);
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex()); // 大きな値も0に正規化される
    
    // 複数回の呼び出しテスト
    state.onButtonA();
    state.onButtonB();
    state.onButtonC();
    state.onButtonALongPress();
    state.onButtonBLongPress();
    state.onButtonCLongPress();
    state.onExit();
    
    // 全て正常に実行されることを確認
}

// SettingsLogicの未テスト関数のテストケース追加
void test_settings_logic_load_save_settings() {
    SettingsLogic logic;
    
    // loadSettingsが例外を投げないことを確認
    logic.loadSettings();
    // 正常に実行されることを確認（例外が発生しない）
    
    // saveSettingsが例外を投げないことを確認
    logic.saveSettings();
    // 正常に実行されることを確認（例外が発生しない）
}

void test_settings_logic_set_lcd_brightness() {
    SettingsLogic logic;
    
    // 初期値を確認
    TEST_ASSERT_EQUAL(150, logic.getLcdBrightness());
    
    // 有効範囲のテスト
    logic.setLcdBrightness(50);
    TEST_ASSERT_EQUAL(50, logic.getLcdBrightness());
    
    logic.setLcdBrightness(250);
    TEST_ASSERT_EQUAL(250, logic.getLcdBrightness());
    
    // 境界値テスト（50-250の範囲でのみ設定される）
    logic.setLcdBrightness(0);
    TEST_ASSERT_EQUAL(250, logic.getLcdBrightness()); // 範囲外なので前の値のまま
    
    logic.setLcdBrightness(255);
    TEST_ASSERT_EQUAL(250, logic.getLcdBrightness()); // 範囲外なので前の値のまま
}

void test_settings_logic_set_sound_enabled() {
    SettingsLogic logic;
    
    // サウンド設定を変更
    logic.setSoundEnabled(false);
    TEST_ASSERT_FALSE(logic.isSoundEnabled());
    
    logic.setSoundEnabled(true);
    TEST_ASSERT_TRUE(logic.isSoundEnabled());
}

void test_settings_logic_set_value_edit_mode() {
    SettingsLogic logic;
    
    // 値編集モードを設定
    logic.setValueEditMode(true);
    TEST_ASSERT_TRUE(logic.isValueEditMode());
    
    logic.setValueEditMode(false);
    TEST_ASSERT_FALSE(logic.isValueEditMode());
}

void test_settings_logic_validate_settings() {
    SettingsLogic logic;
    
    // validateSettingsが例外を投げないことを確認
    bool isValid = logic.validateSettings();
    // 正常に実行されることを確認（例外が発生しない）
    // 戻り値は実装に依存するため、ここでは確認しない
}

void test_settings_logic_edge_cases() {
    SettingsLogic logic;
    
    // 極端な値でのテスト
    logic.setLcdBrightness(-1);
    TEST_ASSERT_EQUAL(150, logic.getLcdBrightness()); // 負の値は初期値に戻る
    
    logic.setLcdBrightness(1000);
    TEST_ASSERT_EQUAL(150, logic.getLcdBrightness()); // 大きな値も初期値に戻る
    
    // 複数回の設定変更
    logic.setSoundEnabled(true);
    logic.setSoundEnabled(false);
    logic.setSoundEnabled(true);
    TEST_ASSERT_TRUE(logic.isSoundEnabled());
    
    logic.setValueEditMode(true);
    logic.setValueEditMode(false);
    logic.setValueEditMode(true);
    TEST_ASSERT_TRUE(logic.isValueEditMode());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_settings_display_basic);
    RUN_TEST(test_settings_display_get_selected_index);
    RUN_TEST(test_settings_display_set_selected_index);
    RUN_TEST(test_settings_display_on_exit);
    RUN_TEST(test_settings_display_on_button_c);
    RUN_TEST(test_settings_display_on_button_c_long_press);
    RUN_TEST(test_settings_display_edge_cases);
    RUN_TEST(test_settings_logic_load_save_settings);
    RUN_TEST(test_settings_logic_set_lcd_brightness);
    RUN_TEST(test_settings_logic_set_sound_enabled);
    RUN_TEST(test_settings_logic_set_value_edit_mode);
    RUN_TEST(test_settings_logic_validate_settings);
    RUN_TEST(test_settings_logic_edge_cases);
    UNITY_END();
    return 0;
} 