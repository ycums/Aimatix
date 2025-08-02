#include <unity.h>
#include "AlarmDisplayState.h"
#include "IAlarmDisplayView.h"
#include "AlarmLogic.h"
#include "ITimeProvider.h"
#include "ITimeManager.h"
#include <vector>
#include <ctime>
#include <memory>

// モックTimeProviderクラス
class MockTimeProvider : public ITimeProvider {
public:
    time_t mockTime = 1000;
    unsigned long mockMillis = 0;
    bool lastSetSystemTimeResult = true;
    
    time_t now() const override { return mockTime; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    bool setSystemTime(time_t time) override { 
        mockTime = time; 
        return lastSetSystemTimeResult; 
    }
    
    void setTime(time_t time) { mockTime = time; }
    void setMillis(unsigned long millis) { mockMillis = millis; }
    void setSetSystemTimeResult(bool result) { lastSetSystemTimeResult = result; }
};

// モックTimeManagerクラス
class MockTimeManager : public ITimeManager {
public:
    unsigned long mockMillis = 0;
    time_t mockTime = 1000;
    
    unsigned long getCurrentMillis() const override { return mockMillis; }
    time_t getCurrentTime() const override { return mockTime; }
    
    void setMillis(unsigned long millis) { mockMillis = millis; }
    void setTime(time_t time) { mockTime = time; }
};

// モックAlarmDisplayViewクラス
class MockAlarmDisplayView : public IAlarmDisplayView {
public:
    std::vector<std::string> drawnTexts;
    std::vector<int> textColors;
    std::vector<int> bgColors;
    bool cleared = false;
    std::vector<time_t> lastShownAlarms;
    size_t lastSelectedIndex = 0;
    bool noAlarmsShown = false;
    
    void clear() override { cleared = true; }
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        drawnTexts.push_back(std::string("TITLE: ") + title);
    }
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        drawnTexts.push_back(std::string("HINTS: ") + btnA + " " + btnB + " " + btnC);
    }
    void showAlarmList(const std::vector<time_t>& alarms, size_t selectedIndex) override {
        lastShownAlarms = alarms;
        lastSelectedIndex = selectedIndex;
        noAlarmsShown = false;
    }
    void showNoAlarms() override {
        lastShownAlarms.clear();
        noAlarmsShown = true;
    }
    
    void reset() {
        drawnTexts.clear();
        textColors.clear();
        bgColors.clear();
        cleared = false;
        lastShownAlarms.clear();
        lastSelectedIndex = 0;
        noAlarmsShown = false;
    }
};

// モックStateManagerクラス
class MockStateManager : public StateManager, public IState {
public:
    IState* lastSetState = nullptr;
    
    // StateManager::setStateをオーバーライド
    void setState(IState* state) override {
        lastSetState = state;
    }
    
    // IStateインターフェースの実装
    void onEnter() override {}
    void onExit() override {}
    void onDraw() override {}
    void onButtonA() override {}
    void onButtonB() override {}
    void onButtonC() override {}
    void onButtonALongPress() override {}
    void onButtonBLongPress() override {}
    void onButtonCLongPress() override {}
};

// テスト用のアラームリスト
std::vector<time_t> test_alarm_times;
extern std::vector<time_t> alarm_times; // 外部変数の宣言

void setUp(void) {
    test_alarm_times.clear();
    alarm_times.clear(); // ←必ずリセット
    time_t now = time(nullptr);
    AlarmLogic::initAlarms(test_alarm_times, now);
    // alarm_timesへの初期化は各テストで必要に応じて行う
}

void tearDown(void) {
    test_alarm_times.clear();
}

// テストケース: 初期化
void test_AlarmDisplayState_Initialization() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex());
}

// テストケース: 選択管理
void test_AlarmDisplayState_SelectionManagement() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    state.setSelectedIndex(2);
    TEST_ASSERT_EQUAL(2, state.getSelectedIndex());
}

// テストケース: 上移動
void test_AlarmDisplayState_MoveUp() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    
    state.setSelectedIndex(1);
    state.onButtonA(); // 上移動
    
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex());
}

// テストケース: 下移動
void test_AlarmDisplayState_MoveDown() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    
    state.setSelectedIndex(0);
    state.onButtonB(); // 下移動
    
    TEST_ASSERT_EQUAL(1, state.getSelectedIndex());
}

// テストケース: 一番上に移動
void test_AlarmDisplayState_MoveToTop() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    
    state.setSelectedIndex(1);
    state.onButtonALongPress(); // 一番上に移動
    
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex());
}

// テストケース: 一番下に移動
void test_AlarmDisplayState_MoveToBottom() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    
    state.setSelectedIndex(0);
    state.onButtonBLongPress(); // 一番下に移動
    
    // アラームリストの最後のインデックスを取得
    size_t expectedIndex = alarm_times.size() - 1;
    TEST_ASSERT_EQUAL(expectedIndex, state.getSelectedIndex());
}

// テストケース: アラーム削除（正常）
void test_AlarmDisplayState_DeleteAlarm_Normal() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    time_t alarmTime = time(nullptr) + 3600;
    alarm_times.push_back(alarmTime);
    
    state.setSelectedIndex(0);
    size_t initialSize = alarm_times.size();
    
    state.onButtonC(); // 削除
    
    TEST_ASSERT_EQUAL(initialSize - 1, alarm_times.size());
}

// テストケース: アラーム削除（既に削除済み）
void test_AlarmDisplayState_DeleteAlarm_AlreadyRemoved() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    alarm_times.clear(); // 明示的にリセット
    // アラームを追加して削除
    time_t alarmTime = time(nullptr) + 3600;
    alarm_times.push_back(alarmTime);
    size_t initialSize = alarm_times.size();
    
    state.setSelectedIndex(0);
    state.onButtonC(); // 削除
    
    // 再度削除を試行（既に削除済み）
    state.onButtonC();
    
    // 削除後のサイズを確認（既に削除済みなので0）
    TEST_ASSERT_EQUAL(0, alarm_times.size());
}

// テストケース: ハイブリッドアプローチ - リアルタイム更新
void test_AlarmDisplayState_HybridApproach_RealTimeUpdate() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // ユーザー操作から十分な時間が経過
    timeManager->setMillis(5000); // 5秒経過
    
    // アラームを追加して描画を有効にする
    alarm_times.push_back(time(nullptr) + 3600);
    
    state.onDraw(); // リアルタイム更新が実行される
    
    // Viewが呼び出されたことを確認
    TEST_ASSERT_TRUE(mockView.cleared || !mockView.drawnTexts.empty() || !mockView.lastShownAlarms.empty());
}

// テストケース: ハイブリッドアプローチ - 更新抑制
void test_AlarmDisplayState_HybridApproach_UpdateSuppression() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // ユーザー操作直後
    timeManager->setMillis(100); // 0.1秒経過
    
    mockView.reset();
    state.onDraw(); // 更新抑制される
    
    // Viewが呼び出されないことを確認
    TEST_ASSERT_FALSE(mockView.cleared);
}

// テストケース: 空リスト
void test_AlarmDisplayState_EmptyList() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームリストを空にする
    alarm_times.clear();
    
    state.onEnter();
    
    // 空リスト表示が呼び出されることを確認
    TEST_ASSERT_TRUE(mockView.noAlarmsShown);
}

// テストケース: 境界条件
void test_AlarmDisplayState_BoundaryConditions() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    
    // 上端での移動制限
    state.setSelectedIndex(0);
    state.onButtonA(); // 上移動を試行
    TEST_ASSERT_EQUAL(0, state.getSelectedIndex()); // 移動しない
    
    // 下端での移動制限
    size_t lastIndex = alarm_times.size() - 1;
    state.setSelectedIndex(lastIndex);
    state.onButtonB(); // 下移動を試行
    TEST_ASSERT_EQUAL(lastIndex, state.getSelectedIndex()); // 移動しない
}

// テストケース: 値ベース削除
void test_AlarmDisplayState_ValueBasedDeletion() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // 同じ時刻のアラームを複数追加
    time_t alarmTime = time(nullptr) + 3600;
    alarm_times.push_back(alarmTime);
    alarm_times.push_back(alarmTime); // 重複
    
    state.setSelectedIndex(0);
    size_t initialSize = alarm_times.size();
    
    state.onButtonC(); // 削除
    
    // 値ベースで削除されることを確認
    TEST_ASSERT_EQUAL(initialSize - 1, alarm_times.size());
}

// テストケース: 時刻ベース削除 - 表示クリア
void test_AlarmDisplayState_TimeBasedDeletion_DisplayClear() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    alarm_times.clear(); // 明示的にリセット
    // 過去のアラームを追加
    time_t pastAlarm = time(nullptr) - 3600; // 1時間前
    alarm_times.push_back(pastAlarm);
    
    // MockTimeProviderの時刻を未来に設定
    timeProvider->setTime(time(nullptr) + 3600); // 1時間後
    
    // 初期化時に過去のアラームが削除されることを確認
    state.onEnter();
    
    // 過去のアラームが削除されることを確認
    TEST_ASSERT_EQUAL(0, alarm_times.size());
}

// テストケース: アラームなし表示 - ちらつきなし
void test_AlarmDisplayState_NoAlarmsDisplay_NoFlicker() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームリストを空にする
    alarm_times.clear();
    
    state.onEnter();
    mockView.reset();
    
    // 2回目の描画（変更なし）
    state.onDraw();
    
    // ちらつき防止のため、2回目の描画では何も呼び出されないことを確認
    TEST_ASSERT_FALSE(mockView.cleared);
    TEST_ASSERT_TRUE(mockView.drawnTexts.empty());
}

// テストケース: 部分領域クリア
void test_AlarmDisplayState_PartialAreaClear() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    alarm_times.clear(); // 明示的にリセット
    // 複数のアラームを追加
    alarm_times.push_back(time(nullptr) + 3600);
    alarm_times.push_back(time(nullptr) + 7200);
    alarm_times.push_back(time(nullptr) + 10800);
    
    state.onEnter();
    TEST_ASSERT_EQUAL(3, mockView.lastShownAlarms.size());
    
    // アラームを削除
    alarm_times.pop_back();
    
    // 十分な時間経過を設定してshouldUpdateRealTime()がtrueを返すようにする
    timeManager->setMillis(5000);
    
    mockView.lastShownAlarms.clear(); // ここでリセット
    
    state.onDraw();
    
    TEST_ASSERT_EQUAL(2, mockView.lastShownAlarms.size());
}

// テストケース: Cボタン長押し
void test_AlarmDisplayState_OnButtonCLongPress() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    MockStateManager mainState;
    mainState.lastSetState = nullptr; // 明示的に初期化
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    state.setMainDisplayState(&mainState);
    state.onButtonCLongPress();
    
    // メイン画面に遷移することを確認（nullptrでないことを確認）
    TEST_ASSERT_NOT_NULL(mockManager.lastSetState);
}

// テストケース: 複数回遷移での表示バグ（3-0-15）
void test_AlarmDisplayState_MultipleEntryDisplayBug() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // アラームを追加
    alarm_times.clear();
    alarm_times.push_back(time(nullptr) + 3600); // 1時間後
    alarm_times.push_back(time(nullptr) + 7200); // 2時間後
    
    // 初回表示 - 正しく表示される
    state.onEnter();
    TEST_ASSERT_EQUAL(2, mockView.lastShownAlarms.size());
    TEST_ASSERT_FALSE(mockView.noAlarmsShown);
    
    // 画面遷移をシミュレート
    state.onExit();
    mockView.reset();
    
    // 2度目の表示 - バグにより表示されない
    state.onEnter();
    
    // 期待: アラームが表示される（現在の実装では失敗）
    // バグ: lastDisplayedAlarmsが残存しているため更新されない
    TEST_ASSERT_EQUAL(2, mockView.lastShownAlarms.size());
    TEST_ASSERT_FALSE(mockView.noAlarmsShown);
}

// テストケース: 終了処理
void test_AlarmDisplayState_OnExit() {
    MockAlarmDisplayView mockView;
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    AlarmDisplayState state(&mockManager, &mockView, timeProvider, timeManager);
    
    // 終了処理を実行（現在は何もしない）
    state.onExit();
    
    // エラーが発生しないことを確認
    TEST_PASS();
}

// 未カバー分岐テスト: view == nullptrの場合のonDraw
void test_AlarmDisplayState_OnDraw_WithNullView() {
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    
    // viewをnullptrに設定
    AlarmDisplayState state(&mockManager, nullptr, timeProvider, timeManager);
    
    // onDrawを呼び出し（エラーが発生しないことを確認）
    state.onDraw();
    
    TEST_PASS();
}

// 未カバー分岐テスト: view == nullptrの場合のforceDraw
void test_AlarmDisplayState_ForceDraw_WithNullView() {
    MockStateManager mockManager;
    auto timeProvider = std::make_shared<MockTimeProvider>();
    auto timeManager = std::make_shared<MockTimeManager>();
    
    // viewをnullptrに設定
    AlarmDisplayState state(&mockManager, nullptr, timeProvider, timeManager);
    
    // forceDrawを呼び出し（エラーが発生しないことを確認）
    // 注意: forceDrawはprivateなので、onEnterを通じてテスト
    state.onEnter();
    
    TEST_PASS();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_AlarmDisplayState_Initialization);
    RUN_TEST(test_AlarmDisplayState_SelectionManagement);
    RUN_TEST(test_AlarmDisplayState_MoveUp);
    RUN_TEST(test_AlarmDisplayState_MoveDown);
    RUN_TEST(test_AlarmDisplayState_MoveToTop);
    RUN_TEST(test_AlarmDisplayState_MoveToBottom);
    RUN_TEST(test_AlarmDisplayState_DeleteAlarm_Normal);
    RUN_TEST(test_AlarmDisplayState_DeleteAlarm_AlreadyRemoved);
    RUN_TEST(test_AlarmDisplayState_HybridApproach_RealTimeUpdate);
    RUN_TEST(test_AlarmDisplayState_HybridApproach_UpdateSuppression);
    RUN_TEST(test_AlarmDisplayState_EmptyList);
    RUN_TEST(test_AlarmDisplayState_BoundaryConditions);
    RUN_TEST(test_AlarmDisplayState_ValueBasedDeletion);
    RUN_TEST(test_AlarmDisplayState_TimeBasedDeletion_DisplayClear);
    RUN_TEST(test_AlarmDisplayState_NoAlarmsDisplay_NoFlicker);
    RUN_TEST(test_AlarmDisplayState_PartialAreaClear);
    RUN_TEST(test_AlarmDisplayState_OnButtonCLongPress);
    RUN_TEST(test_AlarmDisplayState_MultipleEntryDisplayBug);
    RUN_TEST(test_AlarmDisplayState_OnExit);
    RUN_TEST(test_AlarmDisplayState_OnDraw_WithNullView);
    RUN_TEST(test_AlarmDisplayState_ForceDraw_WithNullView);
    return UNITY_END();
} 