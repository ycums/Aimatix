#include "DisplayCommon.h"
#include "StateManager.h"
#include "MainDisplayState.h"
#include "InputDisplayState.h"
#include "AlarmDisplayState.h"
#include "SettingsDisplayState.h"
#include "DateTimeInputState.h"
#include "InputLogic.h"
#include "SettingsLogic.h"
#include "InputDisplayViewImpl.h"
#include "MainDisplayViewImpl.h"
#include "AlarmDisplayViewImpl.h"
#include "SettingsDisplayViewImpl.h"
#include "DateTimeInputViewImpl.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include "DisplayAdapter.h"
#include "TimeValidationLogic.h"
#include "ButtonManager.h"
#include "TimeSyncDisplayState.h"
#include "TimeSyncViewImpl.h"
#include "SoftApTimeSyncController.h"

// 共通include（全環境で使用）
#include <Arduino.h>
#include <vector>
#include <ctime>
#include "DateTimeAdapter.h"

// 統一されたM5Unified include
#include <M5Unified.h>
#include <M5GFX.h>

// 定数定義
constexpr int LOOP_DELAY_MS = 50;

// 統一された描画関数（全デバイス共通）
auto m5_rect_impl(int pos_x, int pos_y, int width, int height) -> void {
    M5.Display.drawRect(pos_x, pos_y, width, height, AMBER_COLOR);
}

auto m5_string_impl(const char* str, int pos_x, int pos_y) -> void {
    M5.Display.drawString(str, pos_x, pos_y);
}

auto m5_progress_bar_impl(int pos_x, int pos_y, int width, int height, int percent) -> void {
    constexpr int BORDER_WIDTH = 1;
    constexpr int PERCENT_DENOMINATOR = 100;
    
    M5Canvas canvas(&M5.Display);
    canvas.createSprite(width, height);
    canvas.fillSprite(TFT_BLACK);
    canvas.drawRect(0, 0, width, height, AMBER_COLOR);
    
    const int fillW = (width - 2 * BORDER_WIDTH) * percent / PERCENT_DENOMINATOR;
    if (fillW > 0) {
        canvas.fillRect(BORDER_WIDTH, BORDER_WIDTH, fillW, height - 2 * BORDER_WIDTH, AMBER_COLOR);
    }
    
    canvas.pushSprite(pos_x, pos_y);
    canvas.deleteSprite();
}

auto m5_set_font_impl(int font_size) -> void {
    M5.Display.setTextFont(font_size);
    M5.Display.setTextColor(AMBER_COLOR, TFT_BLACK);
}

auto m5_set_text_datum_impl(int text_datum) -> void {
    M5.Display.setTextDatum(text_datum);
}

auto m5_fill_rect_impl(int pos_x, int pos_y, int width, int height, int color) -> void {
    M5.Display.fillRect(pos_x, pos_y, width, height, color);
}

#ifdef ARDUINO
// M5Stack用TimeManager実装（全デバイス共通）
class M5StackTimeManager : public ITimeManager {
public:
    auto getCurrentMillis() const -> unsigned long override { return millis(); }
    auto getCurrentTime() const -> time_t override { return time(nullptr); }
};
#endif

// --- アラームリスト ---
std::vector<time_t> alarm_times;

// --- 状態管理クラスのグローバル生成 ---
StateManager state_manager;
DisplayAdapter display_adapter;
InputDisplayViewImpl input_display_view_impl(&display_adapter);
MainDisplayViewImpl main_display_view_impl(&display_adapter);
AlarmDisplayViewImpl alarm_display_view_impl(&display_adapter);
SettingsDisplayViewImpl settings_display_view_impl(&display_adapter);
DateTimeInputViewImpl datetime_input_view_impl(&display_adapter);
TimeLogic time_logic;
AlarmLogic alarm_logic;
SettingsLogic settings_logic;
ButtonManager button_manager;

#ifdef ARDUINO
// M5Stack関連のクラス（全デバイス共通）
const std::shared_ptr<DateTimeAdapter> m5_time_provider = std::make_shared<DateTimeAdapter>();
const std::shared_ptr<M5StackTimeManager> m5_time_manager = std::make_shared<M5StackTimeManager>();
// Export globals for components in src/ that need time adapters (e.g., SoftApTimeSyncController)
ITimeProvider* g_time_provider = m5_time_provider.get();
ITimeManager* g_time_manager = m5_time_manager.get();
InputLogic input_logic(m5_time_provider);
InputDisplayState input_display_state(&input_logic, &input_display_view_impl, m5_time_provider.get());
MainDisplayState main_display_state(&state_manager, &input_display_state, &main_display_view_impl, &time_logic, &alarm_logic);
AlarmDisplayState alarm_display_state(&state_manager, &alarm_display_view_impl, m5_time_provider, m5_time_manager);
SettingsDisplayState settings_display_state(&settings_logic, &settings_display_view_impl);
 TimeSyncViewImpl time_sync_view_impl(&display_adapter);
 SoftApTimeSyncController time_sync_controller;
 TimeSyncDisplayState time_sync_display_state(&time_sync_view_impl, &time_sync_controller);
DateTimeInputState datetime_input_state(m5_time_provider.get(), &datetime_input_view_impl);
#else
// Native環境用のモック（テスト用）
InputLogic input_logic(nullptr);
InputDisplayState input_display_state(&input_logic, &input_display_view_impl);
MainDisplayState main_display_state(&state_manager, &input_display_state, &main_display_view_impl, &time_logic, &alarm_logic);
AlarmDisplayState alarm_display_state(&state_manager, &alarm_display_view_impl, nullptr, nullptr);
SettingsDisplayState settings_display_state(&settings_logic, &settings_display_view_impl);
DateTimeInputState datetime_input_state(nullptr, &datetime_input_view_impl);
#endif

// 統一されたsetup関数
#ifdef ARDUINO
void setup() {
    auto cfg = M5.config();
    // シリアル出力のボーレート設定（platformio.ini の SERIAL_BAUD に準拠）
#ifdef SERIAL_BAUD
    cfg.serial_baudrate = SERIAL_BAUD;
#else
    cfg.serial_baudrate = 115200;
#endif
    cfg.clear_display = true;
    cfg.output_power = true;
    M5.begin(cfg);
    M5.Display.setTextColor(AMBER_COLOR, TFT_BLACK);
    
    // DateTimeAdapterの初期化
    m5_time_provider->initializeTime();
    
    // アラームリスト初期化
    alarm_times.clear();
    time_t now = time(nullptr);
    AlarmLogic::initAlarms(alarm_times, now);
    
    // システム時刻の検証と補正（起動時処理）
    if (TimeValidationLogic::validateAndCorrectSystemTime(m5_time_provider.get())) {
        // 時刻補正が実行された場合のログ出力（デバッグ用）
        // プロダクション環境では必要に応じて削除可能
    }

    // --- 状態遷移の依存注入（@/design/ui_state_management.md準拠） ---
    input_display_state.setManager(&state_manager);
    input_display_state.setMainDisplayState(&main_display_state);
    main_display_state.setAlarmDisplayState(&alarm_display_state);
    alarm_display_state.setMainDisplayState(&main_display_state);
    settings_display_state.setManager(&state_manager);
    settings_display_state.setMainDisplayState(&main_display_state);
    settings_display_state.setSettingsLogic(&settings_logic);
    // DateTimeInputへの導線はMVP1では停止し、TimeSyncへ差し替え
    settings_display_state.setTimeSyncDisplayState(&time_sync_display_state);
    main_display_state.setSettingsDisplayState(&settings_display_state);
    time_sync_display_state.setManager(&state_manager);
    time_sync_display_state.setSettingsDisplayState(&settings_display_state);
    time_sync_display_state.setMainDisplayState(&main_display_state);
    // 状態遷移の初期状態をMainDisplayに
    state_manager.setState(&main_display_state);
}
#endif

// 統一されたloop関数
#ifdef ARDUINO
void loop() {
    M5.update();
    // 物理ボタン状態をButtonManagerに渡す
    button_manager.update(ButtonManager::BtnA, M5.BtnA.isPressed(), millis());
    button_manager.update(ButtonManager::BtnB, M5.BtnB.isPressed(), millis());
    button_manager.update(ButtonManager::BtnC, M5.BtnC.isPressed(), millis());
    // 論理イベントでStateManagerに伝搬
    if (button_manager.isShortPress(ButtonManager::BtnA)) { state_manager.handleButtonA(); }
    if (button_manager.isShortPress(ButtonManager::BtnB)) { state_manager.handleButtonB(); }
    if (button_manager.isShortPress(ButtonManager::BtnC)) { state_manager.handleButtonC(); }
    if (button_manager.isLongPress(ButtonManager::BtnA)) { state_manager.handleButtonALongPress(); }
    if (button_manager.isLongPress(ButtonManager::BtnB)) { state_manager.handleButtonBLongPress(); }
    if (button_manager.isLongPress(ButtonManager::BtnC)) { state_manager.handleButtonCLongPress(); }
    // 現在の状態の描画
    IState* current = state_manager.getCurrentState();
    if (current != nullptr) {
        current->onDraw();
    }
    delay(LOOP_DELAY_MS);
}
#endif

#ifndef ARDUINO
// Native環境用のmain関数
int main() {
    // Native環境では何もしない（テスト用）
    return 0;
}
#endif 