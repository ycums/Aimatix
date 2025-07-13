#include "mock_m5stack.h"
#include <cstdio>

// time_tの定義を避けるためにctimeを直接インクルードしない
// 代わりに必要な関数を直接定義

// グローバルなM5Stackインスタンス
MockM5Stack M5;

// モック関数の実装
unsigned long mockMillis = 0;
time_t mockTime = 1000000000; // 2001年9月9日 01:46:40 UTC

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

// シリアル通信のモック
class MockSerial {
public:
  void begin(unsigned long baud) {}
  void print(const char* str) { printf("%s", str); }
  void print(int value) { printf("%d", value); }
  void print(unsigned long value) { printf("%lu", value); }
  void print(long value) { printf("%ld", value); }
  void print(long long value) { printf("%lld", value); }
  void print(size_t value) { printf("%zu", value); }
  void println(const char* str) { printf("%s\n", str); }
  void println(int value) { printf("%d\n", value); }
  void println(unsigned long value) { printf("%lu\n", value); }
  void println(long value) { printf("%ld\n", value); }
  void println(long long value) { printf("%lld\n", value); }
  void println(size_t value) { printf("%zu\n", value); }
  void println() { printf("\n"); }
};

MockSerial Serial; 