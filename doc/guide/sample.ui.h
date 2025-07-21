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
#define AMBER_COLOR 0xFB20  // アンバー色 (RGB565: R=31, G=22, B=0) - 確定
#define FLASH_ORANGE 0xF000  // 警告色 (RGB565: R=31, G=0, B=0) - 確定
#define TFT_BLACK 0x0000

// フォント定義
#define FONT_AUXILIARY 2 // Font2 (Height 16)
#define FONT_IMPORTANT 4 // Font4 (Height 26)
#define FONT_MAIN      7 // Font7 (Height 48)

// グリッドシステム定義
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 共通UI要素の高さ
#define TITLE_HEIGHT 20 // タイトルバー高さ
#define HINT_HEIGHT 20  // ボタンヒント高さ

// コンテンツ領域（タイトルバーとボタンヒントを除いた部分）
#define CONTENT_WIDTH 320
#define CONTENT_HEIGHT 200  // 240 - 20 - 20

// グリッド定義（全体を16x12グリッドに分割）
#define GRID_WIDTH 20   // 320/16
#define GRID_HEIGHT 20  // 240/12

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
extern std::vector<time_t> alarmTimes; // Declare alarmTimes as extern

// Function declarations
void initUI();
void drawTitleBar(const char* modeName);
void drawButtonHintsGrid(const char* btnA, const char* btnB, const char* btnC);
void drawCommonUI(const char* modeName);
void drawGridLines();
std::string getTimeString(time_t t);
std::string getDateString(time_t t);
std::string getRemainTimeString(time_t now, time_t target);
void drawProgressBar(int x, int y, int width, int height, float progress);
void drawInvertedText(const char* text, int x, int y, int font);
bool showYesNoDialog(const char* title, const char* detail);

// 警告メッセージ表示機能
void showWarningMessage(const char* message, unsigned long duration = 3000);
bool isWarningMessageDisplayed(const char* message);
void clearWarningMessage();

// Screen drawing functions
void drawMainDisplay(const Settings& settings);
void drawNTPSync();
void drawInputMode(const DigitEditTimeInputState& state);
void drawAlarmManagement(); // アラーム管理画面を追加
void drawAlarmActive();
void drawSettingsMenu(const Settings& settings);
void drawInfoDisplay();
void drawWarningColorTest();

// メニュー項目描画の共通関数
void drawMenuItems(const char** items, int itemCount, int selectedItem, int startGridRow, int endGridRow, int marginTop);

// 各モードの描画関数プロトタイプ
void drawMainDisplay();
void drawNTPSync();
void drawInputMode();

void drawAlarmActive();
void drawSettingsMenu();

#endif // UI_H
