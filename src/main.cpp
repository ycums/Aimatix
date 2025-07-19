#include <M5Stack.h>
#include "ui.h"

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200);
  // LCD明度255固定
  M5.Lcd.setBrightness(255);
  // UI初期化
  initUI();
  Serial.println("Phase2-1: Baseline UI only, brightness 255");
}

void loop() {
  // 画面更新のみ（100ms間隔）
  static unsigned long lastScreenUpdate = 0;
  if (millis() - lastScreenUpdate >= 100) {
    lastScreenUpdate = millis();
    drawMainDisplay();
    sprite.pushSprite(0, 0);
  }
  delay(10);
}