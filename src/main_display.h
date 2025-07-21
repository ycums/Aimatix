#pragma once

// --- UI定数（spec.md/sample.ui.h準拠） ---
#define AMBER_COLOR 0xFB20
#define FLASH_ORANGE 0xF000
#define TFT_BLACK 0x0000
#define FONT_AUXILIARY 2
#define FONT_IMPORTANT 4
#define FONT_MAIN 7
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TITLE_HEIGHT 20
#define HINT_HEIGHT 20
#define GRID_WIDTH 20
#define GRID_HEIGHT 20
#define GRID_X(col) (col * GRID_WIDTH)
#define GRID_Y(row) (TITLE_HEIGHT + (row) * GRID_HEIGHT)
#define TL_DATUM 0
#define TC_DATUM 1
#define MC_DATUM 4

// --- 既存宣言 ---
void drawMainDisplay();

// Adapter層で描画関数を差し替え可能にするsetter
void setDrawRectImpl(void (*impl)(int, int, int, int));
void setDrawStringImpl(void (*impl)(const char*, int, int));
void setFillProgressBarImpl(void (*impl)(int, int, int, int, int));
void setFontImpl(void (*impl)(int));
void setTextDatumImpl(void (*impl)(int));

void drawRect(int x, int y, int w, int h);
void drawString(const char* str, int x, int y);
void fillProgressBar(int x, int y, int w, int h, int percent);
void setFont(int font);
void setTextDatum(int datum);

// --- 追加: タイトルバー・ボタンヒント描画関数 ---
void drawTitleBar(const char* modeName, int batteryLevel, bool isCharging);
void drawButtonHintsGrid(const char* btnA, const char* btnB, const char* btnC);
void drawGridLines(); 