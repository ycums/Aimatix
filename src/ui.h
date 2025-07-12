#ifndef UI_H
#define UI_H

#include <M5Stack.h>
#include <time.h>
#include <vector> // std::vector を使用するため
#include "types.h" // Mode enum と SettingsMenu 構造体を使用するため
#include "settings.h" // Settings 構造体を使用するため
#include "input.h" // InputState 構造体を使用するため

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

// グリッド座標からピクセル座標への変換ヘルパー
#define GRID_X(col) (col * GRID_WIDTH)
#define GRID_Y(row) (TITLE_HEIGHT + row * GRID_HEIGHT)

// Declare the sprite object (TFT_eSprite is available via M5Stack library)
extern TFT_eSprite sprite;

// Declare global variables used in UI functions
extern std::vector<time_t> alarmTimes; // Declare alarmTimes as extern

// Function declarations
void drawWarningColorTest();

// 関数プロトタイプ
void drawStatusBar(const char* mode);
void drawButtonHints(const char* btnA, const char* btnB, const char* btnC);
String getTimeString(time_t t);
String getDateString(time_t t);
String getRemainTimeString(time_t now, time_t target);
void drawProgressBar(int x, int y, int width, int height, float progress);
void drawInvertedText(const char* text, int x, int y, int font);
void initUI(); // Function to initialize UI including sprite
void drawInfoDisplay();

// グリッドシステム関連の関数プロトタイプ
void drawCommonUI(const char* modeName);
void drawTitleBar(const char* modeName);
void drawButtonHintsGrid(const char* btnA, const char* btnB, const char* btnC);
void drawGridLines(); // グリッド線の描画（デバッグ用）

// メニュー項目描画の共通関数
void drawMenuItems(const char** items, int itemCount, int selectedItem, int startGridRow, int endGridRow, int marginTop);

// 各モードの描画関数プロトタイプ
void drawMainDisplay();
void drawNTPSync();
void drawInputMode();
void drawScheduleSelection();

void drawAlarmActive();
void drawSettingsMenu();

#endif // UI_H
