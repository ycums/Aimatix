#include "DisplayCommon.h"
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "InputLogic.h"
#include "InputDisplayViewImpl.h"
#include "MainDisplayViewImpl.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include "DisplayAdapter.h"
#ifdef ARDUINO
#include <M5Stack.h>
#include <vector>
#include <ctime>
#include <M5Display.h>
#endif
#include "ButtonManager.h"

// 定数定義
constexpr int LOOP_DELAY_MS = 50;

extern void setFillRectImpl(void (*impl)(int, int, int, int, int));
extern void setFillProgressBarSpriteImpl(void (*impl)(int, int, int, int, int));

#ifdef ARDUINO
// --- M5Stack用描画関数 ---
void m5_rect_impl(int pos_x, int pos_y, int width, int height) {
    M5.Lcd.drawRect(pos_x, pos_y, width, height, AMBER_COLOR);
}
void m5_string_impl(const char* str, int pos_x, int pos_y) {
    M5.Lcd.drawString(str, pos_x, pos_y);
}
void m5_progress_bar_impl(int pos_x, int pos_y, int width, int height, int percent) {
    const int BORDER_WIDTH = 1;
    const int PERCENT_DENOMINATOR = 100;
    
    M5.Lcd.drawRect(pos_x, pos_y, width, height, AMBER_COLOR);
    M5.Lcd.fillRect(pos_x + BORDER_WIDTH, pos_y + BORDER_WIDTH, width - 2 * BORDER_WIDTH, height - 2 * BORDER_WIDTH, TFT_BLACK);
    const int fillW = (width - 2 * BORDER_WIDTH) * percent / PERCENT_DENOMINATOR;
    if (fillW > 0) {
        M5.Lcd.fillRect(pos_x + BORDER_WIDTH, pos_y + BORDER_WIDTH, fillW, height - 2 * BORDER_WIDTH, AMBER_COLOR);
    }
}
void m5_set_font_impl(int font_size) {
    M5.Lcd.setTextFont(font_size);
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);
}
void m5_set_text_datum_impl(int text_datum) {
    M5.Lcd.setTextDatum(text_datum);
}
void m5_fill_rect_impl(int pos_x, int pos_y, int width, int height, int color) {
    M5.Lcd.fillRect(pos_x, pos_y, width, height, color);
}
#endif

// --- アラームリスト ---
std::vector<time_t> alarm_times;

// --- 状態管理クラスのグローバル生成 ---
StateManager state_manager;
InputLogic input_logic;
DisplayAdapter display_adapter;
InputDisplayViewImpl input_display_view_impl(&display_adapter);
MainDisplayViewImpl main_display_view_impl(&display_adapter);
TimeLogic time_logic;
AlarmLogic alarm_logic;
InputDisplayState input_display_state(&input_logic, &input_display_view_impl);
MainDisplayState main_display_state(&state_manager, &input_display_state, &main_display_view_impl, &time_logic, &alarm_logic);
ButtonManager button_manager; // 追加

void setup() {
#ifdef ARDUINO
    M5.begin();
    M5.Lcd.setTextColor(AMBER_COLOR, TFT_BLACK);

    // アラームリスト初期化
    alarm_times.clear();
    time_t now = time(nullptr);
    AlarmLogic::initAlarms(alarm_times, now);
#endif
    // --- 状態遷移の依存注入（@/design/ui_state_management.md準拠） ---
    input_display_state.setManager(&state_manager);
    input_display_state.setMainDisplayState(&main_display_state);
    // 状態遷移の初期状態をMainDisplayに
    state_manager.setState(&main_display_state);
}

void loop() {
#ifdef ARDUINO
    M5.update();
    // 物理ボタン状態をButtonManagerに渡す
    button_manager.update(ButtonManager::BtnA, M5.BtnA.isPressed(), millis());
    button_manager.update(ButtonManager::BtnB, M5.BtnB.isPressed(), millis());
    button_manager.update(ButtonManager::BtnC, M5.BtnC.isPressed(), millis());
    // 論理イベントでStateManagerに伝搬
    if (button_manager.isShortPress(ButtonManager::BtnA)) state_manager.handleButtonA();
    if (button_manager.isShortPress(ButtonManager::BtnB)) state_manager.handleButtonB();
    if (button_manager.isShortPress(ButtonManager::BtnC)) state_manager.handleButtonC();
    if (button_manager.isLongPress(ButtonManager::BtnA)) state_manager.handleButtonALongPress();
    if (button_manager.isLongPress(ButtonManager::BtnB)) state_manager.handleButtonBLongPress();
    if (button_manager.isLongPress(ButtonManager::BtnC)) state_manager.handleButtonCLongPress();
    // 現在の状態の描画
    IState* current = state_manager.getCurrentState();
    if (current != nullptr) {
        current->onDraw();
    }
    delay(LOOP_DELAY_MS);
#endif
} 