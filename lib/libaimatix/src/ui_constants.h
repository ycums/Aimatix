#ifndef AIMATIX_UI_CONSTANTS_H
#define AIMATIX_UI_CONSTANTS_H

// 色
#define AMBER_COLOR 0xFB20
#define TFT_BLACK 0x0000

// 画面サイズ
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// UIレイアウト
#define TITLE_HEIGHT 20
#define HINT_HEIGHT 20
#define GRID_WIDTH 20
#define GRID_HEIGHT 20
#define GRID_X(col) ((col) * GRID_WIDTH)
#define GRID_Y(row) (TITLE_HEIGHT + (row) * GRID_HEIGHT)

// フォント
#define FONT_AUXILIARY 2
#define FONT_MAIN 4
#define FONT_IMPORTANT 7

// テキスト描画基準
#define TL_DATUM 0
#define TC_DATUM 1
#define ML_DATUM 3
#define MC_DATUM 4

// AlarmDisplayState用定数
#define ALARM_DISPLAY_START_Y 40
#define ALARM_LINE_HEIGHT GRID_HEIGHT
#define ALARM_MAX_DISPLAY 10
#define ALARM_TEXT_OFFSET 10
#define ALARM_BACKGROUND_OFFSET ALARM_LINE_HEIGHT/2
#define ALARM_UPDATE_PAUSE_DURATION 100
#define ALARM_TIME_STR_SIZE 16

// バッテリー関連
#define BATTERY_WARNING_THRESHOLD 20

#endif // AIMATIX_UI_CONSTANTS_H 