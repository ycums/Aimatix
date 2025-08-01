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
#define GRID_X(col) (((col) * GRID_WIDTH))
#define GRID_Y(row) ((TITLE_HEIGHT + (row) * GRID_HEIGHT))

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
#define ALARM_BACKGROUND_OFFSET (ALARM_LINE_HEIGHT/2)
#define ALARM_UPDATE_PAUSE_DURATION 100
#define ALARM_TIME_STR_SIZE 16

// バッテリー関連
#define BATTERY_WARNING_THRESHOLD 20

// DateTimeInput用定数
// 桁位置インデックス
constexpr int DIGIT_YEAR_THOUSAND = 0;
constexpr int DIGIT_YEAR_HUNDRED = 1;
constexpr int DIGIT_YEAR_TEN = 2;
constexpr int DIGIT_YEAR_ONE = 3;
constexpr int DIGIT_MONTH_TEN = 4;
constexpr int DIGIT_MONTH_ONE = 5;
constexpr int DIGIT_DAY_TEN = 6;
constexpr int DIGIT_DAY_ONE = 7;
constexpr int DIGIT_HOUR_TEN = 8;
constexpr int DIGIT_HOUR_ONE = 9;
constexpr int DIGIT_MINUTE_TEN = 10;
constexpr int DIGIT_MINUTE_ONE = 11;
constexpr int TOTAL_DIGITS = 12;

// 日時の境界値
constexpr int MAX_DIGIT_9 = 9;
constexpr int MAX_MONTH_TEN_DIGIT = 1;
constexpr int MAX_DAY_TEN_DIGIT_FEB = 2;
constexpr int MAX_DAY_TEN_DIGIT_OTHER = 3;
constexpr int MAX_HOUR_TEN_DIGIT = 2;
constexpr int MAX_MINUTE_TEN_DIGIT = 5;

// 日付の値
constexpr int DAYS_IN_FEBRUARY = 28;
constexpr int DAYS_IN_FEBRUARY_LEAP = 29;
constexpr int DAYS_IN_MONTH_30 = 30;
constexpr int DAYS_IN_MONTH_31 = 31;

// 年の計算
constexpr int YEAR_OFFSET_1900 = 1900;
constexpr int YEAR_MULTIPLIER_1000 = 1000;
constexpr int YEAR_MULTIPLIER_100 = 100;
constexpr int YEAR_MULTIPLIER_10 = 10;

// うるう年計算
constexpr int LEAP_YEAR_DIV_4 = 4;
constexpr int LEAP_YEAR_DIV_100 = 100;
constexpr int LEAP_YEAR_DIV_400 = 400;

// フォント・文字サイズ
constexpr int CHAR_WIDTH_FONT7 = 16;
constexpr int CHAR_WIDTH_FONT4 = 8;
constexpr int STRING_BUFFER_SIZE = 16;

// 画面位置
constexpr int SCREEN_CENTER_X = 160;

#endif // AIMATIX_UI_CONSTANTS_H 