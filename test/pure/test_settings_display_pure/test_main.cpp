#include <unity.h>
#include "SettingsLogic.h"
#include "SettingsLogic.cpp"
#include "SettingsDisplayState.h"
#include "ISettingsDisplayView.h"
#include "StateManager.h"
#include <memory>
#include <vector>
#include <string>

// モックSettingsDisplayView
class MockSettingsDisplayView : public ISettingsDisplayView {
public:
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        lastTitle = title;
        lastBatteryLevel = batteryLevel;
        lastIsCharging = isCharging;
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        lastBtnA = btnA;
        lastBtnB = btnB;
        lastBtnC = btnC;
    }
    
    void showSettingsList(const std::vector<std::string>& items, size_t selectedIndex) override {
        lastItems = items;
        lastSelectedIndex = selectedIndex;
    }
    
    void clear() override {
        clearCalled = true;
    }
    
    // テスト用のアクセサ
    std::string lastTitle;
    int lastBatteryLevel = 0;
    bool lastIsCharging = false;
    std::string lastBtnA, lastBtnB, lastBtnC;
    std::vector<std::string> lastItems;
    size_t lastSelectedIndex = 0;
    bool clearCalled = false;
};

// テスト用のSettingsLogicインスタンス
SettingsLogic testSettingsLogic;

void setUp(void) {
    testSettingsLogic.resetSettings();
}

void tearDown(void) {
    // 必要に応じてクリーンアップ
}

void test_SettingsLogic_Initialization() {
    // 初期値の確認
    TEST_ASSERT_EQUAL(150, testSettingsLogic.getLcdBrightness());
    TEST_ASSERT_TRUE(testSettingsLogic.isSoundEnabled());
    TEST_ASSERT_EQUAL(SettingsItem::SOUND, testSettingsLogic.getSelectedItem());
    TEST_ASSERT_FALSE(testSettingsLogic.isValueEditMode());
    TEST_ASSERT_EQUAL(4, testSettingsLogic.getItemCount());
}

void test_SettingsLogic_ItemManagement() {
    // 論理的識別子ベースの項目管理
    TEST_ASSERT_EQUAL(SettingsItem::SOUND, testSettingsLogic.getItemByIndex(0));
    TEST_ASSERT_EQUAL(SettingsItem::LCD_BRIGHTNESS, testSettingsLogic.getItemByIndex(1));
    TEST_ASSERT_EQUAL(SettingsItem::SET_DATE_TIME, testSettingsLogic.getItemByIndex(2));
    TEST_ASSERT_EQUAL(SettingsItem::INFO, testSettingsLogic.getItemByIndex(3));
    
    TEST_ASSERT_EQUAL(0, testSettingsLogic.getIndexByItem(SettingsItem::SOUND));
    TEST_ASSERT_EQUAL(1, testSettingsLogic.getIndexByItem(SettingsItem::LCD_BRIGHTNESS));
    TEST_ASSERT_EQUAL(2, testSettingsLogic.getIndexByItem(SettingsItem::SET_DATE_TIME));
    TEST_ASSERT_EQUAL(3, testSettingsLogic.getIndexByItem(SettingsItem::INFO));
}

void test_SettingsLogic_DisplayNames() {
    // 表示名の確認
    TEST_ASSERT_EQUAL_STRING("SOUND", testSettingsLogic.getItemDisplayName(SettingsItem::SOUND).c_str());
    TEST_ASSERT_EQUAL_STRING("LCD BRIGHTNESS", testSettingsLogic.getItemDisplayName(SettingsItem::LCD_BRIGHTNESS).c_str());
    TEST_ASSERT_EQUAL_STRING("SET DATE/TIME", testSettingsLogic.getItemDisplayName(SettingsItem::SET_DATE_TIME).c_str());
    TEST_ASSERT_EQUAL_STRING("INFO", testSettingsLogic.getItemDisplayName(SettingsItem::INFO).c_str());
}

void test_SettingsLogic_ValueStrings() {
    // 値文字列の確認
    TEST_ASSERT_EQUAL_STRING("ON", testSettingsLogic.getItemValueString(SettingsItem::SOUND).c_str());
    TEST_ASSERT_EQUAL_STRING("150", testSettingsLogic.getItemValueString(SettingsItem::LCD_BRIGHTNESS).c_str());
    TEST_ASSERT_EQUAL_STRING("", testSettingsLogic.getItemValueString(SettingsItem::SET_DATE_TIME).c_str());
    TEST_ASSERT_EQUAL_STRING("", testSettingsLogic.getItemValueString(SettingsItem::INFO).c_str());
}

void test_SettingsLogic_ItemSelection() {
    // 項目選択の確認
    testSettingsLogic.setSelectedItem(SettingsItem::LCD_BRIGHTNESS);
    TEST_ASSERT_EQUAL(SettingsItem::LCD_BRIGHTNESS, testSettingsLogic.getSelectedItem());
    
    testSettingsLogic.setSelectedItem(SettingsItem::INFO);
    TEST_ASSERT_EQUAL(SettingsItem::INFO, testSettingsLogic.getSelectedItem());
}

void test_SettingsDisplayState_Initialization() {
    MockSettingsDisplayView mockView;
    SettingsDisplayState state(&testSettingsLogic, &mockView);
    
    state.onEnter();
    
    TEST_ASSERT_EQUAL_STRING("SETTINGS", mockView.lastTitle.c_str());
    TEST_ASSERT_EQUAL_STRING("UP", mockView.lastBtnA.c_str());
    TEST_ASSERT_EQUAL_STRING("DOWN", mockView.lastBtnB.c_str());
    TEST_ASSERT_EQUAL_STRING("SELECT", mockView.lastBtnC.c_str());
    TEST_ASSERT_TRUE(mockView.clearCalled);
}

void test_SettingsDisplayState_ItemNavigation() {
    MockSettingsDisplayView mockView;
    SettingsDisplayState state(&testSettingsLogic, &mockView);
    
    // 初期状態はSOUND
    TEST_ASSERT_EQUAL(SettingsItem::SOUND, testSettingsLogic.getSelectedItem());
    
    // Bボタンで下に移動
    state.onButtonB();
    TEST_ASSERT_EQUAL(SettingsItem::LCD_BRIGHTNESS, testSettingsLogic.getSelectedItem());
    
    // Bボタンでさらに下に移動
    state.onButtonB();
    TEST_ASSERT_EQUAL(SettingsItem::SET_DATE_TIME, testSettingsLogic.getSelectedItem());
    
    // Aボタンで上に移動
    state.onButtonA();
    TEST_ASSERT_EQUAL(SettingsItem::LCD_BRIGHTNESS, testSettingsLogic.getSelectedItem());
}

void test_SettingsDisplayState_BoundaryNavigation() {
    MockSettingsDisplayView mockView;
    SettingsDisplayState state(&testSettingsLogic, &mockView);
    
    // 一番上からAボタンで移動しない（端で停止）
    testSettingsLogic.setSelectedItem(SettingsItem::SOUND);
    state.onButtonA();
    TEST_ASSERT_EQUAL(SettingsItem::SOUND, testSettingsLogic.getSelectedItem());
    
    // 一番下からBボタンで移動しない（端で停止）
    testSettingsLogic.setSelectedItem(SettingsItem::INFO);
    state.onButtonB();
    TEST_ASSERT_EQUAL(SettingsItem::INFO, testSettingsLogic.getSelectedItem());
}

void test_SettingsDisplayState_LongPressNavigation() {
    MockSettingsDisplayView mockView;
    SettingsDisplayState state(&testSettingsLogic, &mockView);
    
    // A長押しで一番上に移動
    testSettingsLogic.setSelectedItem(SettingsItem::INFO);
    state.onButtonALongPress();
    TEST_ASSERT_EQUAL(SettingsItem::SOUND, testSettingsLogic.getSelectedItem());
    
    // B長押しで一番下に移動
    testSettingsLogic.setSelectedItem(SettingsItem::SOUND);
    state.onButtonBLongPress();
    TEST_ASSERT_EQUAL(SettingsItem::INFO, testSettingsLogic.getSelectedItem());
}

void test_SettingsDisplayState_DisplayList() {
    MockSettingsDisplayView mockView;
    SettingsDisplayState state(&testSettingsLogic, &mockView);
    
    state.onDraw();
    
    // 設定項目リストの確認
    TEST_ASSERT_EQUAL(4, mockView.lastItems.size());
    TEST_ASSERT_EQUAL_STRING("SOUND: ON", mockView.lastItems[0].c_str());
    TEST_ASSERT_EQUAL_STRING("LCD BRIGHTNESS: 150", mockView.lastItems[1].c_str());
    TEST_ASSERT_EQUAL_STRING("SET DATE/TIME", mockView.lastItems[2].c_str());
    TEST_ASSERT_EQUAL_STRING("INFO", mockView.lastItems[3].c_str());
    TEST_ASSERT_EQUAL(0, mockView.lastSelectedIndex); // 初期選択はSOUND
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_SettingsLogic_Initialization);
    RUN_TEST(test_SettingsLogic_ItemManagement);
    RUN_TEST(test_SettingsLogic_DisplayNames);
    RUN_TEST(test_SettingsLogic_ValueStrings);
    RUN_TEST(test_SettingsLogic_ItemSelection);
    RUN_TEST(test_SettingsDisplayState_Initialization);
    RUN_TEST(test_SettingsDisplayState_ItemNavigation);
    RUN_TEST(test_SettingsDisplayState_BoundaryNavigation);
    RUN_TEST(test_SettingsDisplayState_LongPressNavigation);
    RUN_TEST(test_SettingsDisplayState_DisplayList);
    
    return UNITY_END();
} 