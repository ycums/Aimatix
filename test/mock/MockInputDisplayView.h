#pragma once
#include "IInputDisplayView.h"
#include <string>

class MockInputDisplayView : public IInputDisplayView {
public:
    // 呼び出し回数を記録
    int showTitleCallCount = 0;
    int showHintsCallCount = 0;
    int showPreviewCallCount = 0;
    int clearCallCount = 0;
    int showDigitCallCount = 0;
    int showColonCallCount = 0;
    
    // 最後に呼び出された引数を記録
    std::string lastTitle;
    int lastBatteryLevel = 0;
    bool lastIsCharging = false;
    std::string lastBtnA;
    std::string lastBtnB;
    std::string lastBtnC;
    std::string lastPreview;
    int lastDigitIndex = 0;
    int lastDigitValue = 0;
    bool lastDigitEntered = false;
    
    // インターフェース実装
    void showTitle(const char* title, int batteryLevel, bool isCharging) override {
        showTitleCallCount++;
        lastTitle = title ? title : "";
        lastBatteryLevel = batteryLevel;
        lastIsCharging = isCharging;
    }
    
    void showHints(const char* btnA, const char* btnB, const char* btnC) override {
        showHintsCallCount++;
        lastBtnA = btnA ? btnA : "";
        lastBtnB = btnB ? btnB : "";
        lastBtnC = btnC ? btnC : "";
    }
    
    void showPreview(const char* preview) override {
        showPreviewCallCount++;
        lastPreview = preview ? preview : "";
    }
    
    void clear() override {
        clearCallCount++;
    }
    
    void showDigit(int index, int value, bool entered) override {
        showDigitCallCount++;
        lastDigitIndex = index;
        lastDigitValue = value;
        lastDigitEntered = entered;
    }
    
    void showColon() override {
        showColonCallCount++;
    }
    
    // テスト用のリセットメソッド
    void reset() {
        showTitleCallCount = 0;
        showHintsCallCount = 0;
        showPreviewCallCount = 0;
        clearCallCount = 0;
        showDigitCallCount = 0;
        showColonCallCount = 0;
        lastTitle.clear();
        lastBatteryLevel = 0;
        lastIsCharging = false;
        lastBtnA.clear();
        lastBtnB.clear();
        lastBtnC.clear();
        lastPreview.clear();
        lastDigitIndex = 0;
        lastDigitValue = 0;
        lastDigitEntered = false;
    }
}; 