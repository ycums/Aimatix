#ifndef UI_H
#define UI_H

#include <M5Stack.h>
#include <time.h>
#include <vector> // std::vector を使用するため
#include "types.h" // Mode enum と SettingsMenu 構造体を使用するため
#include "settings.h" // Settings 構造体を使用するため
#include "input.h" // InputState 構造体を使用するため

// カラーパレット定義
#define AMBER_COLOR 0xFDC0
#define FLASH_ORANGE 0xFB20
#define DARK_GREY 0x2104
#define TFT_BLACK 0x0000

// フォント定義
#define FONT_AUXILIARY 2 // Font2 (Height 16)
#define FONT_IMPORTANT 4 // Font4 (Height 26)
#define FONT_MAIN      7 // Font7 (Height 48)

// グローバル変数 (declared in main.cpp or other .cpp files)
// extern Mode currentMode; // Moved to types.h
// extern Settings settings; // Moved to settings.h
// extern std::vector<time_t> alarmTimes; // Moved to alarm.h
// extern InputState inputState; // Moved to input.h
// extern SettingsMenu settingsMenu; // Moved to types.h

// 関数プロトタイプ
void drawStatusBar(const char* mode);
void drawButtonHints(const char* btnA, const char* btnB, const char* btnC);
String getTimeString(time_t t);
String getDateString(time_t t);
String getRemainTimeString(time_t now, time_t target);
void drawProgressBar(int x, int y, int width, int height, float progress);
void drawInvertedText(int x, int y, const char* text, int font);

// 各モードの描画関数プロトタイプ
void drawMainDisplay();
void drawNTPSync();
void drawInputMode();
void drawScheduleSelect(int selectedIndex);
void drawAlarmActive();
void drawSettingsMenu();

#endif // UI_H
