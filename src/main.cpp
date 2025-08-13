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
#include "AlarmActiveViewImpl.h"
#include "AlarmActiveState.h"
#include "SettingsDisplayViewImpl.h"
#include "DateTimeInputViewImpl.h"
#include "TimeLogic.h"
#include "AlarmLogic.h"
#include "DisplayAdapter.h"
#include "TimeValidationLogic.h"
#include "BootAutoSyncPolicy.h"
#include "ButtonManager.h"
#include "TimeSyncDisplayState.h"
#include "TimeSyncViewImpl.h"
#include "SoftApTimeSyncController.h"
#include "M5TimeService.h"
#ifdef ARDUINO
#ifdef M5STACK_CORE2
#include "VibrationSequencer.h"
#include "Core2VibrationAdapter.h"
#endif
#endif

// 共通include（全環境で使用）
#include <Arduino.h>
#include <vector>
#include <ctime>
// #include "DateTimeAdapter.h" // deprecated after unifying to ITimeService

// 統一されたM5Unified include
#include <M5Unified.h>
#include <M5GFX.h>

// 定数定義
// LOOP_DELAY_MS はフレームクロック導入により廃止

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

// ITimeService へ一本化したため、ITimeManager/DateTimeAdapter は削除

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
#include "FrameClockPlanner.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Core2 vibration: single file-scope instances used by setup()/loop()
#ifdef M5STACK_CORE2
static VibrationSequencer g_vibe_seq;
static Core2VibrationAdapter g_vibe_out;
#endif
// M5Stack関連のクラス（全デバイス共通）
static M5TimeService g_time_service_impl;
ITimeService* g_time_service = &g_time_service_impl;
const std::shared_ptr<ITimeService> m5_time_service{ &g_time_service_impl, [](ITimeService*){} };
InputLogic input_logic(m5_time_service);
InputDisplayState input_display_state(&input_logic, &input_display_view_impl, g_time_service);
MainDisplayState main_display_state(&state_manager, &input_display_state, &main_display_view_impl, &time_logic, &alarm_logic);
 AlarmDisplayState alarm_display_state(&state_manager, &alarm_display_view_impl, m5_time_service);
 AlarmActiveViewImpl alarm_active_view_impl(&display_adapter);
 AlarmActiveState alarm_active_state(&state_manager, &alarm_active_view_impl, &main_display_state, &g_time_service_impl);
SettingsDisplayState settings_display_state(&settings_logic, &settings_display_view_impl);
 TimeSyncViewImpl time_sync_view_impl(&display_adapter);
 SoftApTimeSyncController time_sync_controller;
 TimeSyncDisplayState time_sync_display_state(&time_sync_view_impl, &time_sync_controller);
 // 起動時自動開始の抑止管理（同一ブート内）
 BootAutoSyncPolicy g_boot_auto_policy;
DateTimeInputState datetime_input_state(g_time_service, &datetime_input_view_impl);
// フレームクロック（16fps固定, tick=1ms）
static TickType_t g_last_wake = 0;
static FrameClockPlanner g_frame_clock_planner(62500, 1000);
#else
// Native環境用のモック（テスト用）
InputLogic input_logic(nullptr);
InputDisplayState input_display_state(&input_logic, &input_display_view_impl);
MainDisplayState main_display_state(&state_manager, &input_display_state, &main_display_view_impl, &time_logic, &alarm_logic);
 AlarmDisplayState alarm_display_state(&state_manager, &alarm_display_view_impl, nullptr);
 AlarmActiveViewImpl alarm_active_view_impl(&display_adapter);
 AlarmActiveState alarm_active_state(&state_manager, &alarm_active_view_impl, &main_display_state);
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
	Serial.begin(cfg.serial_baudrate);
	Serial.println("[BOOT] M5.begin done");
	M5.Display.setTextColor(AMBER_COLOR, TFT_BLACK);
#if defined(M5STACK_CORE2) && defined(ENABLE_CORE2_BOOT_VIBE_DEMO)
	g_vibe_seq.loadPattern({
		{100, 100},   // 100ms ON（100%）
		{100, 0},     // 100ms OFF
		{100, 100},   // 100ms ON（100%）
		{500, 0},     // 500ms OFF
		{1000, 80},   // 1000ms ON（80%）
		{500, 0},     // 500ms OFF
		{1000, 80}    // 1000ms ON（80%）
	}, false);        // 繰り返しなし
	g_vibe_seq.start(millis()); // パターン再生開始
	Serial.println("[VIBE] pattern loaded and started");
#endif
	
	// ITimeService一本化後も、必要なら here で初期化ロジックを追加可能
	
	// アラームリスト初期化
	alarm_times.clear();
	time_t now = time(nullptr);
	AlarmLogic::initAlarms(alarm_times, now);
	
	// Boot Auto: 無効時刻ならTime Sync自動開始（EXITで同一ブート抑止）
	// 注意: 自動開始の判定は「補正前の生時刻」で行う
	#ifndef SKIP_BOOT_AUTO_SYNC
	g_boot_auto_policy.resetForBoot();
	const bool isInvalidAtBoot = TimeValidationLogic::isSystemTimeBeforeMinimum(g_time_service);

	// システム時刻の検証と補正（起動時処理）
	(void)TimeValidationLogic::validateAndCorrectSystemTime(g_time_service);

	if (g_boot_auto_policy.shouldStartAutoSync(isInvalidAtBoot)) {
		// 直ちにTIME_SYNC状態へ遷移（UI/QRはTimeSyncDisplayStateに委譲）
		time_sync_display_state.setManager(&state_manager);
		time_sync_display_state.setSettingsDisplayState(&settings_display_state);
		time_sync_display_state.setMainDisplayState(&main_display_state);
		time_sync_display_state.setBootAutoSyncPolicy(&g_boot_auto_policy);
		state_manager.setState(&time_sync_display_state);
	}
	#endif

	// --- 状態遷移の依存注入（@/design/ui_state_management.md準拠） ---
	input_display_state.setManager(&state_manager);
	input_display_state.setMainDisplayState(&main_display_state);
	main_display_state.setAlarmDisplayState(&alarm_display_state);
	main_display_state.setAlarmActiveState(&alarm_active_state);
	alarm_display_state.setMainDisplayState(&main_display_state);
	settings_display_state.setManager(&state_manager);
	settings_display_state.setMainDisplayState(&main_display_state);
	settings_display_state.setSettingsLogic(&settings_logic);
	// DateTimeInputへの導線はMVP1では停止し、TimeSyncへ差し替え
	suggestions_display_state.setTimeSyncDisplayState(&time_sync_display_state);
	main_display_state.setSettingsDisplayState(&settings_display_state);
	time_sync_display_state.setManager(&state_manager);
	time_sync_display_state.setSettingsDisplayState(&settings_display_state);
	time_sync_display_state.setMainDisplayState(&main_display_state);
	// 状態遷移の初期状態をMainDisplayに（既に他状態へ遷移済みなら変更しない）
	if (state_manager.getCurrentState() == nullptr) {
		state_manager.setState(&main_display_state);
	}

	// フレームクロック初期化（位相維持の基準）
	g_last_wake = xTaskGetTickCount();
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
	// 論理イベントを一度だけ取得し、以降で共有利用（消費の二重化を防止）
	const bool pdA = button_manager.isPressDown(ButtonManager::BtnA);
	const bool pdB = button_manager.isPressDown(ButtonManager::BtnB);
	const bool pdC = button_manager.isPressDown(ButtonManager::BtnC);
	const bool spA = button_manager.isShortPress(ButtonManager::BtnA);
	const bool spB = button_manager.isShortPress(ButtonManager::BtnB);
	const bool spC = button_manager.isShortPress(ButtonManager::BtnC);
	const bool lpA = button_manager.isLongPress(ButtonManager::BtnA);
	const bool lpB = button_manager.isLongPress(ButtonManager::BtnB);
	const bool lpC = button_manager.isLongPress(ButtonManager::BtnC);

	// 論理イベントでStateManagerに伝搬（ローカル値を使用）
	if (spA) { state_manager.handleButtonA(); }
	if (spB) { state_manager.handleButtonB(); }
	if (spC) { state_manager.handleButtonC(); }
	if (lpA) { state_manager.handleButtonALongPress(); }
	if (lpB) { state_manager.handleButtonBLongPress(); }
	if (lpC) { state_manager.handleButtonCLongPress(); }
	// 現在の状態の描画
	IState* current = state_manager.getCurrentState();
	if (current != nullptr) {
		current->onDraw();
	}

	// --- Core2: Haptics feedback on press/longPress ---
	// --- Core2: Haptics feedback on press/longPress ---
#ifdef M5STACK_CORE2
	// 設定: press/longPressで個別のパターン（初期: 100ms/100%）
	static const std::vector<VibrationSequencer::Segment> kPressDownPattern = { {100, 100} };
	static const std::vector<VibrationSequencer::Segment> kLongPressPattern = { {100, 100} };
	if (pdA || pdB || pdC) {
		g_vibe_seq.loadPattern(kPressDownPattern, false);
		g_vibe_seq.start(millis());
	}
	if (lpA || lpB || lpC) {
		g_vibe_seq.loadPattern(kLongPressPattern, false);
		g_vibe_seq.start(millis());
	}
	g_vibe_seq.update(millis(), &g_vibe_out);
#endif
	// 位相維持フレームクロック（16fps）
	const TickType_t step = pdMS_TO_TICKS(g_frame_clock_planner.nextDelayMs());
	vTaskDelayUntil(&g_last_wake, step);
}
#endif

#ifndef ARDUINO
// Native環境用のmain関数
int main() {
	// Native環境では何もしない（テスト用）
	return 0;
}
#endif
