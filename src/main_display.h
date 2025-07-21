#pragma once

void drawMainDisplay();

// Adapter層で描画関数を差し替え可能にするsetter
void setDrawRectImpl(void (*impl)(int, int, int, int));
void setDrawStringImpl(void (*impl)(const char*, int, int));
void setFillProgressBarImpl(void (*impl)(int, int, int, int, int));

void drawRect(int x, int y, int w, int h);
void drawString(const char* str, int x, int y);
void fillProgressBar(int x, int y, int w, int h, int percent); 