#include <unity.h>
#include "../../../lib/libaimatix/src/TimeValidationLogic.h"
#include "../../../lib/libaimatix/src/ITimeProvider.h"
#include "../../../lib/libaimatix/src/BootAutoSyncPolicy.h"
#include <ctime>

// モックTimeProviderクラス
class MockTimeProvider : public ITimeProvider {
public:
    time_t mockTime = 1000; // デフォルトは1970年代（最小時刻より前）
    bool setSystemTimeResult = true;
    time_t lastSetTime = 0;
    
    time_t now() const override { return mockTime; }
    struct tm* localtime(time_t* t) const override { return ::localtime(t); }
    
    bool setSystemTime(time_t time) override {
        if (setSystemTimeResult) {
            lastSetTime = time;
            mockTime = time; // 設定された時刻に更新
        }
        return setSystemTimeResult;
    }
    
    // テスト用のメソッド
    void setTime(time_t time) { mockTime = time; }
    void setSetSystemTimeResult(bool result) { setSystemTimeResult = result; }
    time_t getLastSetTime() const { return lastSetTime; }
};

void setUp(void) {
    // 各テスト前の初期化
}

void tearDown(void) {
    // 各テスト後のクリーンアップ
}

// テスト: 最小時刻のtime_t値取得
void test_getMinimumSystemTime() {
    time_t minTime = TimeValidationLogic::getMinimumSystemTime();
    
    // 2025/01/01 00:00:00のtime_t値を手動計算で検証
    struct tm expectedTm = {};
    expectedTm.tm_year = 2025 - 1900;
    expectedTm.tm_mon = 1 - 1;
    expectedTm.tm_mday = 1;
    expectedTm.tm_hour = 0;
    expectedTm.tm_min = 0;
    expectedTm.tm_sec = 0;
    expectedTm.tm_isdst = -1;
    
    time_t expectedTime = mktime(&expectedTm);
    
    TEST_ASSERT_EQUAL(expectedTime, minTime);
}

// テスト: 最小時刻以前の時刻チェック（true）
void test_isSystemTimeBeforeMinimum_true() {
    MockTimeProvider mockProvider;
    // 1970年代の時刻を設定（最小時刻より前）
    mockProvider.setTime(1000);
    
    bool result = TimeValidationLogic::isSystemTimeBeforeMinimum(&mockProvider);
    
    TEST_ASSERT_TRUE(result);
}

// テスト: 最小時刻以降の時刻チェック（false）
void test_isSystemTimeBeforeMinimum_false() {
    MockTimeProvider mockProvider;
    // 2026年の時刻を設定（最小時刻より後）
    struct tm futureTm = {};
    futureTm.tm_year = 2026 - 1900;
    futureTm.tm_mon = 1 - 1;
    futureTm.tm_mday = 1;
    futureTm.tm_hour = 0;
    futureTm.tm_min = 0;
    futureTm.tm_sec = 0;
    futureTm.tm_isdst = -1;
    
    mockProvider.setTime(mktime(&futureTm));
    
    bool result = TimeValidationLogic::isSystemTimeBeforeMinimum(&mockProvider);
    
    TEST_ASSERT_FALSE(result);
}

// テスト: timeProviderがnullの場合（true）
void test_isSystemTimeBeforeMinimum_null_provider() {
    bool result = TimeValidationLogic::isSystemTimeBeforeMinimum(nullptr);
    
    TEST_ASSERT_TRUE(result); // nullの場合は補正が必要とみなす
}

// テスト: 時刻補正の成功
void test_correctSystemTimeToMinimum_success() {
    MockTimeProvider mockProvider;
    mockProvider.setTime(1000); // 古い時刻
    mockProvider.setSetSystemTimeResult(true);
    
    bool result = TimeValidationLogic::correctSystemTimeToMinimum(&mockProvider);
    
    TEST_ASSERT_TRUE(result);
    
    // 設定された時刻が最小時刻であることを確認
    time_t expectedMinTime = TimeValidationLogic::getMinimumSystemTime();
    TEST_ASSERT_EQUAL(expectedMinTime, mockProvider.getLastSetTime());
    TEST_ASSERT_EQUAL(expectedMinTime, mockProvider.now());
}

// テスト: 時刻補正の失敗
void test_correctSystemTimeToMinimum_failure() {
    MockTimeProvider mockProvider;
    mockProvider.setSetSystemTimeResult(false); // 設定失敗をシミュレート
    
    bool result = TimeValidationLogic::correctSystemTimeToMinimum(&mockProvider);
    
    TEST_ASSERT_FALSE(result);
}

// テスト: timeProviderがnullの場合の補正失敗
void test_correctSystemTimeToMinimum_null_provider() {
    bool result = TimeValidationLogic::correctSystemTimeToMinimum(nullptr);
    
    TEST_ASSERT_FALSE(result);
}

// テスト: 検証と補正の統合テスト（補正が必要）
void test_validateAndCorrectSystemTime_correction_needed() {
    MockTimeProvider mockProvider;
    mockProvider.setTime(1000); // 古い時刻
    mockProvider.setSetSystemTimeResult(true);
    
    bool result = TimeValidationLogic::validateAndCorrectSystemTime(&mockProvider);
    
    TEST_ASSERT_TRUE(result); // 補正が実行された
    
    // 時刻が最小時刻に設定されていることを確認
    time_t expectedMinTime = TimeValidationLogic::getMinimumSystemTime();
    TEST_ASSERT_EQUAL(expectedMinTime, mockProvider.now());
}

// テスト: 検証と補正の統合テスト（補正が不要）
void test_validateAndCorrectSystemTime_no_correction_needed() {
    MockTimeProvider mockProvider;
    // 2026年の時刻を設定（最小時刻より後）
    struct tm futureTm = {};
    futureTm.tm_year = 2026 - 1900;
    futureTm.tm_mon = 1 - 1;
    futureTm.tm_mday = 1;
    futureTm.tm_hour = 0;
    futureTm.tm_min = 0;
    futureTm.tm_sec = 0;
    futureTm.tm_isdst = -1;
    
    mockProvider.setTime(mktime(&futureTm));
    
    bool result = TimeValidationLogic::validateAndCorrectSystemTime(&mockProvider);
    
    TEST_ASSERT_FALSE(result); // 補正は実行されなかった
}

// テスト: nullプロバイダでの統合テスト
void test_validateAndCorrectSystemTime_null_provider() {
    bool result = TimeValidationLogic::validateAndCorrectSystemTime(nullptr);
    
    TEST_ASSERT_FALSE(result);
}

// Unity テストランナー
int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_getMinimumSystemTime);
    RUN_TEST(test_isSystemTimeBeforeMinimum_true);
    RUN_TEST(test_isSystemTimeBeforeMinimum_false);
    RUN_TEST(test_isSystemTimeBeforeMinimum_null_provider);
    RUN_TEST(test_correctSystemTimeToMinimum_success);
    RUN_TEST(test_correctSystemTimeToMinimum_failure);
    RUN_TEST(test_correctSystemTimeToMinimum_null_provider);
    RUN_TEST(test_validateAndCorrectSystemTime_correction_needed);
    RUN_TEST(test_validateAndCorrectSystemTime_no_correction_needed);
    RUN_TEST(test_validateAndCorrectSystemTime_null_provider);

    // AIM-57: BootAutoSyncPolicy の基本仕様検証（1テスト1アサート方針）
    {
        BootAutoSyncPolicy p; p.resetForBoot();
        TEST_ASSERT_TRUE(p.shouldStartAutoSync(true));
    }
    {
        BootAutoSyncPolicy p; p.resetForBoot();
        (void)p.shouldStartAutoSync(true);
        TEST_ASSERT_FALSE(p.shouldStartAutoSync(true));
    }
    {
        BootAutoSyncPolicy p; p.resetForBoot();
        p.suppressForThisBoot();
        TEST_ASSERT_FALSE(p.shouldStartAutoSync(true));
    }
    
    return UNITY_END();
}
