#include <M5Stack.h>
#include "ui.h"
#include <settings.h>
#include "m5stack_adapters.h"

// drawMainDisplay用ダミー変数
#include <time.h>
time_t lastReleaseTime = 0;

void setup() {
  M5.begin();
  M5.Power.begin();
  Serial.begin(115200);
  // 設定値ロード
  loadSettings(&eepromAdapter);
  // デバッグ: 明度値を出力
  Serial.print("lcd_brightness: ");
  Serial.println(settings.lcd_brightness);
  // LCD明度を設定値で反映
  M5.Lcd.setBrightness(settings.lcd_brightness);
  // UI初期化
  initUI();
  Serial.println("Phase2-2: settingsロード・明度反映");
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