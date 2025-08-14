#include <unity.h>
#include "AlarmActiveState.h"
#include "BacklightSequencer.h"

namespace {
struct DummyState : public IState {
  int drawCount = 0;
  void onEnter() override {}
  void onExit() override {}
  void onDraw() override { ++drawCount; }
  void onButtonA() override {}
  void onButtonB() override {}
  void onButtonC() override {}
  void onButtonALongPress() override {}
  void onButtonBLongPress() override {}
  void onButtonCLongPress() override {}
};

struct MockBacklight : public IBacklight {
  uint8_t last = 0;
  int calls = 0;
  void setBrightness(uint8_t b) override { last = b; ++calls; }
};
}

void setUp() {}
void tearDown() {}

// 1) onEnter でデフォルトパターンが開始され、1秒(16f)の代表点が期待値になる
void test_alarm_enter_and_frame_progression(void) {
  StateManager mgr;
  DummyState main;
  BacklightSequencer seq;
  MockBacklight out;
  AlarmActiveState s(&mgr, &main, &seq, &out);

  // ベースラインを作る: 事前に任意明度にしておく
  seq.clear();
  seq.enqueueStep(123, 1);
  seq.start();
  seq.tick(&out);

  // 鳴動開始
  s.onEnter();
  // 1秒=16f の代表フレームを検証
  // f0..f15: 255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0
  const uint8_t expected[16] = {255,255,0,0,255,255,0,0,255,255,0,0,0,0,0,0};
  for (int i = 0; i < 16; ++i) {
    seq.tick(&out);
    TEST_ASSERT_EQUAL_UINT8(expected[i], out.last);
  }
}

// 2) 4秒後に完了し、開始前明度(123)へ復帰
void test_alarm_completion_restores_baseline(void) {
  StateManager mgr;
  DummyState main;
  BacklightSequencer seq;
  MockBacklight out;
  AlarmActiveState s(&mgr, &main, &seq, &out);

  // ベースライン=123
  seq.clear(); seq.enqueueStep(123, 1); seq.start(); seq.tick(&out);

  s.onEnter();
  // 4秒=64f 進める
  for (int i = 0; i < 64; ++i) { seq.tick(&out); }
  // onDrawで終了検出→復帰を行う
  s.onDraw();
  TEST_ASSERT_FALSE(seq.isActive());
  TEST_ASSERT_EQUAL_UINT8(123, out.last);
}

// 3) 即時停止（ボタン）で開始前明度へ即復帰
void test_alarm_immediate_stop_restores_baseline(void) {
  StateManager mgr;
  DummyState main;
  BacklightSequencer seq;
  MockBacklight out;
  AlarmActiveState s(&mgr, &main, &seq, &out);

  // ベースライン=77
  seq.clear(); seq.enqueueStep(77, 1); seq.start(); seq.tick(&out);

  s.onEnter();
  seq.tick(&out); // 何フレームか進める
  s.onButtonA();  // 即時停止
  // 即復帰していること
  TEST_ASSERT_EQUAL_UINT8(77, out.last);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_alarm_enter_and_frame_progression);
  RUN_TEST(test_alarm_completion_restores_baseline);
  RUN_TEST(test_alarm_immediate_stop_restores_baseline);
  return UNITY_END();
}

