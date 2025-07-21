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
#define MC_DATUM 4

#endif // AIMATIX_UI_CONSTANTS_H 