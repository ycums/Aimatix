#pragma once

/**
 * BootAutoSyncPolicy
 *
 * 起動時自動Time Syncの実行可否を管理する純粋ポリシークラス。
 * - 起動直後の時刻が無効であれば1回だけ自動開始を許可
 * - 同一ブート内でユーザーがC=EXITした場合は以後の自動開始を抑止
 * - 次回ブートでリセット
 */
class BootAutoSyncPolicy {
public:
    void resetForBoot() {
        hasStarted_ = false;
        suppressed_ = false;
    }

    // 無効時刻かつ未開始・未抑止なら true を返し、開始フラグを立てる
    bool shouldStartAutoSync(bool isTimeInvalidAtBoot) {
        if (suppressed_) return false;
        if (hasStarted_) return false;
        if (!isTimeInvalidAtBoot) return false;
        hasStarted_ = true;
        return true;
    }

    // 同一ブート内での自動開始を抑止
    void suppressForThisBoot() { suppressed_ = true; }

    bool isSuppressed() const { return suppressed_; }
    bool hasStarted() const { return hasStarted_; }

private:
    bool hasStarted_ { false };
    bool suppressed_ { false };
};

