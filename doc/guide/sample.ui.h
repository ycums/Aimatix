#ifndef UI_H
#define UI_H

#include <M5Stack.h>
#include <time.h>
#include <vector> // std::vector を使用するため
#include "types.h" // Mode enum と SettingsMenu 構造体を使用するため
#include <settings.h> // Settings 構造体を使用するため
#include <input.h> // InputState 構造体を使用するため
#include <cstdint>
#include <string>

// カラーパレット定義
constexpr uint16_t AMBER_COLOR = 0xFB20;  // アンバー色 (RGB565: R=31, G=22, B=0) - 確定
constexpr uint16_t FLASH_ORANGE = 0xF000;  // 警告色 (RGB565: R=31, G=0, B=0) - 確定
constexpr uint16_t TFT_BLACK = 0x0000;

// フォント定義
constexpr uint8_t FONT_AUXILIARY = 2; // Font2 (Height 16)
constexpr uint8_t FONT_IMPORTANT = 4; // Font4 (Height 26)
constexpr uint8_t FONT_MAIN = 7; // Font7 (Height 48)

// グリッドシステム定義
constexpr int SCREEN_WIDTH = 320;
constexpr int SCREEN_HEIGHT = 240;

// 共通UI要素の高さ
constexpr int TITLE_HEIGHT = 20; // タイトルバー高さ
constexpr int HINT_HEIGHT = 20;  // ボタンヒント高さ

// コンテンツ領域（タイトルバーとボタンヒントを除いた部分）
constexpr int CONTENT_WIDTH = 320;
constexpr int CONTENT_HEIGHT = 200;  // 240 - 20 - 20

// グリッド定義（全体を16x12グリッドに分割）
constexpr int GRID_WIDTH = 20;   // 320/16
constexpr int GRID_HEIGHT = 20;  // 240/12

// グリッド座標からピクセル座標への変換ヘルパー（条件付きコンパイル）
#ifndef GRID_X
#define GRID_X(col) (col * GRID_WIDTH)
#endif
#ifndef GRID_Y
#define GRID_Y(row) (TITLE_HEIGHT + row * GRID_HEIGHT)
#endif

// Declare the sprite object (TFT_eSprite is available via M5Stack library)
extern TFT_eSprite sprite;

// Declare global variables used in UI functions
extern std::vector<time_t> alarm_times; // Declare alarm_times as extern

// Function declarations
void initUI();
void draw_title_bar(const char* modeName);
void draw_button_hints_grid(const char* btnA, const char* btnB, const char* btnC);
void draw_common_ui(const char* modeName);
void draw_grid_lines();
std::string get_time_string(time_t t);
std::string get_date_string(time_t t);
std::string get_remain_time_string(time_t now, time_t target);
void draw_progress_bar(int pos_x, int pos_y, int width, int height, float progress);
void draw_inverted_text(const char* text, int pos_x, int pos_y, int font);
bool show_yes_no_dialog(const char* title, const char* detail);

// 警告メッセージ表示機能
void show_warning_message(const char* message, unsigned long duration = 3000);
bool is_warning_message_displayed(const char* message);
void clear_warning_message();

// Screen drawing functions
void draw_main_display(const Settings& settings);
void draw_time_sync();
void draw_input_mode(const DigitEditTimeInputState& state);
void draw_alarm_management(); // アラーム管理画面を追加
void draw_alarm_active();
void draw_settings_menu(const Settings& settings);
void draw_info_display();
void draw_warning_color_test();

// メニュー項目描画の共通関数
void draw_menu_items(const char** items, int itemCount, int selectedItem, int startGridRow, int endGridRow, int marginTop);

// 各モードの描画関数プロトタイプ
void draw_main_display();
void draw_time_sync();
void draw_input_mode();

void draw_alarm_active();
void draw_settings_menu();

#endif // UI_H
