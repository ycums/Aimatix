#include "mock_m5stack.h"
#include <cstdio>

// グローバル変数の実装
MockM5Stack M5;
MockSerial Serial;
MockEEPROM EEPROM;

// モック時間管理
unsigned long mockMillis = 0;
time_t mockTime = 1000000000; // 2001年9月9日 01:46:40 UTC

// Arduino関数の実装（このファイルのみで定義）
unsigned long millis() {
  return mockMillis;
}

time_t time(time_t* t) {
  if (t) *t = mockTime;
  return mockTime;
}

void delay(unsigned long ms) {
  mockMillis += ms;
}

// 時間を進める関数（テスト用）
void advanceTime(unsigned long ms) {
  mockMillis += ms;
}

void advanceTimeSeconds(int seconds) {
  mockTime += seconds;
}

// ボタン状態をリセットする関数（テスト用）
void resetAllButtons() {
  M5.BtnA.reset();
  M5.BtnB.reset();
  M5.BtnC.reset();
}

// ボタン押下をシミュレートする関数（テスト用）
void simulateButtonPress(int buttonIndex) {
  switch (buttonIndex) {
    case 0: M5.BtnA.simulatePress(); break;
    case 1: M5.BtnB.simulatePress(); break;
    case 2: M5.BtnC.simulatePress(); break;
  }
}

void simulateButtonRelease(int buttonIndex) {
  switch (buttonIndex) {
    case 0: M5.BtnA.simulateRelease(); break;
    case 1: M5.BtnB.simulateRelease(); break;
    case 2: M5.BtnC.simulateRelease(); break;
  }
}

// バッテリー状態を設定する関数（テスト用）
void setBatteryLevel(int level) {
  // Powerクラスを修正してレベルを設定可能にする必要がある
  // 現在は固定値75%を返す実装
}

void setChargingState(bool charging) {
  // Powerクラスを修正して充電状態を設定可能にする必要がある
  // 現在は固定値falseを返す実装
} 