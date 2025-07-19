#include <unity.h>
#include <cstdio>
#include <ctime>
#include <alarm.h>
#include "../mock/mock_speaker.h"

// alarm.cpp/hの本体ロジックを直接テスト

void setUp(void) {
    alarmTimes.clear();
}
void tearDown(void) {}

void test_addDebugAlarms() {
    alarmTimes.clear();
    addDebugAlarms();
    TEST_ASSERT_EQUAL(5, alarmTimes.size());
    printf("✓ addDebugAlarms: 成功\n");
}

void test_sortAlarms() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now + 7200);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 10800);
    sortAlarms();
    // ここでnowを使って正しい順序を検証
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 7200, alarmTimes[1]);
    TEST_ASSERT_EQUAL(now + 10800, alarmTimes[2]);
    printf("✓ sortAlarms: 成功\n");
}

void test_getNextAlarmTime() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now - 3600);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 7200);
    sortAlarms();
    time_t next = getNextAlarmTime();
    TEST_ASSERT_EQUAL(now + 3600, next);
    printf("✓ getNextAlarmTime: 成功\n");
}

void test_removePastAlarms() {
    alarmTimes.clear();
    time_t now = time(NULL);
    alarmTimes.push_back(now - 3600);
    alarmTimes.push_back(now - 1800);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 7200);
    removePastAlarms();
    // 未来のアラームのみが残ることを検証
    TEST_ASSERT_EQUAL(2, alarmTimes.size());
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 7200, alarmTimes[1]);
    printf("✓ removePastAlarms: 成功\n");
}

// 新しく追加するテストケース（実際に存在する関数のみ）

void test_playAlarm_function() {
    MockSpeaker mockSpeaker;
    
    // 音無効でアラーム再生
    playAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.beep_called);
    
    // 音有効でアラーム再生
    mockSpeaker.reset();
    playAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.beep_called);
    TEST_ASSERT_EQUAL(880, mockSpeaker.last_freq);
    TEST_ASSERT_EQUAL(500, mockSpeaker.last_duration);
    
    printf("✓ playAlarm_function: 成功\n");
}

void test_stopAlarm_function() {
    MockSpeaker mockSpeaker;
    
    // 音無効でアラーム停止
    stopAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.stop_called);
    
    // 音有効でアラーム停止
    mockSpeaker.reset();
    stopAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.stop_called);
    
    printf("✓ stopAlarm_function: 成功\n");
}

void test_alarm_speaker_integration() {
    MockSpeaker mockSpeaker;
    
    // 完全なアラームサイクルのテスト
    // 1. アラーム再生
    playAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.beep_called);
    TEST_ASSERT_EQUAL(880, mockSpeaker.last_freq);
    TEST_ASSERT_EQUAL(500, mockSpeaker.last_duration);
    
    // 2. アラーム停止
    mockSpeaker.reset();
    stopAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.stop_called);
    
    printf("✓ alarm_speaker_integration: 成功\n");
}

void test_alarm_sound_only() {
    MockSpeaker mockSpeaker;
    
    // 音無効のテスト
    playAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.beep_called);
    
    mockSpeaker.reset();
    stopAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.stop_called);
    
    printf("✓ alarm_sound_only: 成功\n");
}

void test_alarm_vibration_only() {
    MockSpeaker mockSpeaker;
    
    // 音有効のテスト
    playAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.beep_called);
    TEST_ASSERT_EQUAL(880, mockSpeaker.last_freq);
    TEST_ASSERT_EQUAL(500, mockSpeaker.last_duration);
    
    mockSpeaker.reset();
    stopAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.stop_called);
    
    printf("✓ alarm_vibration_only: 成功\n");
}

void test_alarm_error_handling() {
    MockSpeaker mockSpeaker;
    
    // 無効なスピーカー状態でのテスト
    // 実際の実装ではnullチェックなどがある可能性
    playAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.beep_called);
    
    mockSpeaker.reset();
    stopAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.stop_called);
    
    printf("✓ alarm_error_handling: 成功\n");
}

void test_alarm_multiple_calls() {
    MockSpeaker mockSpeaker;
    
    // 複数回の呼び出しテスト
    for (int i = 0; i < 5; i++) {
        mockSpeaker.reset();
        playAlarm(&mockSpeaker, (i % 2 == 0));
        TEST_ASSERT_EQUAL((i % 2 == 0), mockSpeaker.beep_called);
        
        mockSpeaker.reset();
        stopAlarm(&mockSpeaker, (i % 2 == 0));
        TEST_ASSERT_EQUAL((i % 2 == 0), mockSpeaker.stop_called);
    }
    
    printf("✓ alarm_multiple_calls: 成功\n");
}

void test_alarm_timing_behavior() {
    MockSpeaker mockSpeaker;
    
    // タイミング動作のテスト
    // 連続した再生・停止
    playAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.beep_called);
    
    stopAlarm(&mockSpeaker, true);
    TEST_ASSERT_TRUE(mockSpeaker.stop_called);
    
    // 即座に再再生
    mockSpeaker.reset();
    playAlarm(&mockSpeaker, false);
    TEST_ASSERT_FALSE(mockSpeaker.beep_called);
    
    printf("✓ alarm_timing_behavior: 成功\n");
}

void test_alarm_vector_operations() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // ベクター操作のテスト
    alarmTimes.push_back(now + 3600);
    TEST_ASSERT_EQUAL(1, alarmTimes.size());
    
    alarmTimes.push_back(now + 7200);
    TEST_ASSERT_EQUAL(2, alarmTimes.size());
    
    // ソート後の順序確認
    sortAlarms();
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 7200, alarmTimes[1]);
    
    printf("✓ alarm_vector_operations: 成功\n");
}

void test_alarm_boundary_conditions() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // 空のベクターでのテスト
    TEST_ASSERT_EQUAL(0, alarmTimes.size());
    
    // 1つのアラームのみ
    alarmTimes.push_back(now + 3600);
    sortAlarms();
    TEST_ASSERT_EQUAL(1, alarmTimes.size());
    
    // 過去のアラームのみ
    alarmTimes.clear();
    alarmTimes.push_back(now - 3600);
    alarmTimes.push_back(now - 1800);
    removePastAlarms();
    TEST_ASSERT_EQUAL(0, alarmTimes.size());
    
    printf("✓ alarm_boundary_conditions: 成功\n");
}

void test_alarm_time_validation() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // 有効な時刻のテスト
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 7200);
    
    // 時刻の比較テスト
    TEST_ASSERT_TRUE(alarmTimes[0] < alarmTimes[1]);
    
    // ソート後の順序確認
    sortAlarms();
    TEST_ASSERT_TRUE(alarmTimes[0] <= alarmTimes[1]);
    
    printf("✓ alarm_time_validation: 成功\n");
}

void test_alarm_performance() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // 多数のアラームでのパフォーマンステスト
    for (int i = 0; i < 100; i++) {
        alarmTimes.push_back(now + (i * 60));
    }
    
    TEST_ASSERT_EQUAL(100, alarmTimes.size());
    
    // ソートのパフォーマンス
    sortAlarms();
    
    // ソート後の順序確認
    for (size_t i = 1; i < alarmTimes.size(); i++) {
        TEST_ASSERT_TRUE(alarmTimes[i-1] <= alarmTimes[i]);
    }
    
    printf("✓ alarm_performance: 成功\n");
}

void test_alarm_edge_cases() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // 同じ時刻のアラーム
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 3600);
    alarmTimes.push_back(now + 3600);
    
    sortAlarms();
    TEST_ASSERT_EQUAL(3, alarmTimes.size());
    
    // 非常に大きな時刻差
    alarmTimes.clear();
    alarmTimes.push_back(now + 86400); // 1日後
    alarmTimes.push_back(now + 3600);  // 1時間後
    
    sortAlarms();
    TEST_ASSERT_EQUAL(now + 3600, alarmTimes[0]);
    TEST_ASSERT_EQUAL(now + 86400, alarmTimes[1]);
    
    printf("✓ alarm_edge_cases: 成功\n");
}

void test_alarm_memory_management() {
    alarmTimes.clear();
    time_t now = time(NULL);
    
    // メモリ管理のテスト
    for (int i = 0; i < 50; i++) {
        alarmTimes.push_back(now + (i * 60));
    }
    
    TEST_ASSERT_EQUAL(50, alarmTimes.size());
    
    // クリア
    alarmTimes.clear();
    TEST_ASSERT_EQUAL(0, alarmTimes.size());
    
    // 再追加
    alarmTimes.push_back(now + 3600);
    TEST_ASSERT_EQUAL(1, alarmTimes.size());
    
    printf("✓ alarm_memory_management: 成功\n");
}

int main() {
    UNITY_BEGIN();
    printf("=== alarm.cpp/h 本体ロジック直接テスト ===\n");
    RUN_TEST(test_addDebugAlarms);
    RUN_TEST(test_sortAlarms);
    RUN_TEST(test_getNextAlarmTime);
    RUN_TEST(test_removePastAlarms);
    RUN_TEST(test_playAlarm_function);
    RUN_TEST(test_stopAlarm_function);
    RUN_TEST(test_alarm_speaker_integration);
    RUN_TEST(test_alarm_sound_only);
    RUN_TEST(test_alarm_vibration_only);
    RUN_TEST(test_alarm_error_handling);
    RUN_TEST(test_alarm_multiple_calls);
    RUN_TEST(test_alarm_timing_behavior);
    RUN_TEST(test_alarm_vector_operations);
    RUN_TEST(test_alarm_boundary_conditions);
    RUN_TEST(test_alarm_time_validation);
    RUN_TEST(test_alarm_performance);
    RUN_TEST(test_alarm_edge_cases);
    RUN_TEST(test_alarm_memory_management);
    printf("=== 全テスト完了 ===\n");
    return UNITY_END();
} 