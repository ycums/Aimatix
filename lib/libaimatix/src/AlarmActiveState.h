#pragma once

#include "StateManager.h"
#include "IAlarmActiveView.h"
#include "AlarmFlashScheduler.h"
#include <cstdint>
#include "ITimeService.h"

class AlarmActiveState : public IState {
public:
    AlarmActiveState(StateManager* manager, IAlarmActiveView* view, IState* mainState, ITimeService* timeService = nullptr)
        : manager_(manager), view_(view), mainState_(mainState), timeService_(timeService), startedMs_(0), elapsedMs_(0) {}

    void onEnter() override {
        elapsedMs_ = 0;
        scheduler_.begin(0);
        // 次回onDrawで必ず一度だけ描画されるようエッジを強制
        lastOn_ = false;
        baseDrawn_ = false;
    }
    void onExit() override {
        // オーバーレイが残らないように必ずOFF描画
        if (view_) { view_->drawFlashOverlay(false); }
        baseDrawn_ = false;
    }
    void onDraw() override {
        // 経過時間を更新（単調msを使用）。フレームごとに差分加算にしても良いが、
        // シンプルさ優先で直値を読んで差分に変換する。
        updateElapsed_();
        const bool on = scheduler_.update(elapsedMs_);
        // ON/OFFの変化（エッジ）のみで描画を行い、ちらつきを防止
        if (on != lastOn_) {
            if (on) {
                // ON遷移: 直前にベースを最新化し、オーバーレイをON
                if (mainState_) { mainState_->onDraw(); }
                if (view_) { view_->drawFlashOverlay(true); }
                baseDrawn_ = true;
            } else {
                // OFF遷移: 先にオーバーレイをOFFし、その後にベースを最新化
                if (view_) { view_->drawFlashOverlay(false); }
                if (mainState_) { mainState_->onDraw(); }
                baseDrawn_ = true;
            }
            lastOn_ = on;
        }
        if (scheduler_.isFinished() && manager_ && mainState_) {
            manager_->setState(mainState_);
        }
    }
    void onButtonA() override { immediateExit_(); }
    void onButtonB() override { immediateExit_(); }
    void onButtonC() override { immediateExit_(); }
    void onButtonALongPress() override { immediateExit_(); }
    void onButtonBLongPress() override { immediateExit_(); }
    void onButtonCLongPress() override { immediateExit_(); }

    // 時間サービスのDI（テスト差し替え用）
    void setTimeService(ITimeService* service) { timeService_ = service; }

private:
    void updateElapsed_() {
        if (!timeService_) { return; }
        const uint32_t nowMs = timeService_->monotonicMillis();
        if (!startedMs_) {
            startedMs_ = nowMs;
        }
        elapsedMs_ = nowMs - startedMs_;
    }
    void immediateExit_() {
        if (manager_ && mainState_) { manager_->setState(mainState_); }
    }

    StateManager* manager_;
    IAlarmActiveView* view_;
    IState* mainState_;
    ITimeService* timeService_ = nullptr;
    AlarmFlashScheduler scheduler_;
    uint32_t startedMs_;
    uint32_t elapsedMs_;
    bool baseDrawn_ = false;
    bool lastOn_ = false;
};


