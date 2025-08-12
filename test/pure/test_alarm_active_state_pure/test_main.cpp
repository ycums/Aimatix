#include <unity.h>
#include "AlarmActiveState.h"

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

struct MockView : public IAlarmActiveView {
  int calls = 0;
  bool last = false;
  void drawFlashOverlay(bool on) override { ++calls; last = on; }
};

struct MockTimeService : public ITimeService {
  uint32_t ms = 0;
  time_t n = 0;
  time_t now() const override { return n; }
  struct tm* localtime(time_t* t) const override { return ::localtime(t); }
  bool setSystemTime(time_t) override { return true; }
  uint32_t monotonicMillis() const override { return ms; }
};
}

void setUp() {}
void tearDown() {}

void test_overlay_called_only_on_toggle_edges(void) {
  StateManager mgr;
  DummyState main;
  MockView view;
  MockTimeService ts;
  AlarmActiveState s(&mgr, &view, &main, &ts);

  // enter at t=0ms (initial ON)
  ts.ms = 0;
  s.onEnter();

  // 0..1000ms (50ms step) におけるトグル境界到達回数に一致する
  for (int i = 0; i <= 20; ++i) { // 0..1000 inclusive, 50ms step
    s.onDraw();
    ts.ms += 50;
  }
  // 期待: サンプリング50ms刻みでのエッジ検出は6回となる（375ms等の境界はサンプリング点に乗らない）
  TEST_ASSERT_EQUAL_INT(6, view.calls);
}

void test_on_exit_overlay_cleared(void) {
  StateManager mgr;
  DummyState main;
  MockView view;
  MockTimeService ts;
  AlarmActiveState s(&mgr, &view, &main, &ts);

  ts.ms = 0;
  s.onEnter();
  s.onDraw(); // initial draw
  s.onExit();
  TEST_ASSERT_FALSE(view.last);
}

void test_reenter_increases_draw_calls(void) {
  StateManager mgr;
  DummyState main;
  MockView view;
  MockTimeService ts;
  AlarmActiveState s(&mgr, &view, &main, &ts);

  ts.ms = 0;
  s.onEnter();
  ts.ms = 5000;     // 1回目は終了相当まで進める
  s.onDraw();

  int before = view.calls;
  s.onEnter();      // 再入
  s.onDraw();       // 初期ONエッジで描画が増えるはず
  TEST_ASSERT_TRUE(view.calls > before);
}

void test_reenter_last_state_is_on(void) {
  StateManager mgr;
  DummyState main;
  MockView view;
  MockTimeService ts;
  AlarmActiveState s(&mgr, &view, &main, &ts);

  ts.ms = 0;
  s.onEnter();
  ts.ms = 5000;
  s.onDraw();

  s.onEnter();      // 再入
  s.onDraw();
  TEST_ASSERT_TRUE(view.last);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_overlay_called_only_on_toggle_edges);
  RUN_TEST(test_on_exit_overlay_cleared);
  RUN_TEST(test_reenter_increases_draw_calls);
  RUN_TEST(test_reenter_last_state_is_on);
  return UNITY_END();
}


