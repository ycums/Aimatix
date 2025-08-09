#pragma once
#include "StateManager.h"
#include "InputLogic.h"
#include "IInputDisplayView.h"
#include "PartialInputLogic.h"
#include "ITimeProvider.h"
#include "TimePreviewLogic.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "AlarmLogic.h"
#include <cstring>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
class InputDisplayState : public IState {
public:
    // InputDisplayStateのコンストラクタ
    InputDisplayState(InputLogic* logic = nullptr, IInputDisplayView* view = nullptr, ITimeProvider* timeProvider = nullptr)
        : inputLogic(logic), view(view), timeProvider_(timeProvider), manager(nullptr), mainDisplayState(nullptr), isRelativeMode(false), 
          errorMessage(""), showError(false), errorStartTime(0) {
        for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
    }
    void onEnter() override {
        if (inputLogic) inputLogic->reset();
        if (view) {
            view->clear();
            // 相対値入力モードの場合はタイトルを変更
            const char* title = isRelativeMode ? "REL+" : "INPUT";
            view->showTitle(title, 42, false);
            view->showHints("INC", "NEXT", "SET");
            for (int i = 0; i < 4; ++i) { lastDigits[i] = -1; lastEntered[i] = false; }
        }
        // 絶対入力モードでは初期状態を __:_0（分一桁=0, entered=true）にする
        if (inputLogic && !isRelativeMode) {
            inputLogic->incrementInput(0);
        }
        // エラー状態をリセット
        resetErrorState();
    }
    void onExit() override {}
    // 相対値計算結果のプレビュー文字列を生成（改善版）
    void generateRelativePreview(char* preview, size_t previewSize) {
        if (!isTimeProviderValid() || !inputLogic) {
            return;
        }
        
        time_t relativeTime = inputLogic->getAbsoluteValue();
        if (relativeTime != -1) {
            auto result = TimePreviewLogic::generateRelativePreview(relativeTime, timeProvider_);
            if (result.isValid) {
                strncpy(preview, result.preview.c_str(), previewSize - 1);
                preview[previewSize - 1] = '\0';
            }
        }
    }

    // 絶対値計算結果のプレビュー文字列を生成（改善版）
    void generateAbsolutePreview(char* preview, size_t previewSize) {
        if (!isTimeProviderValid() || !inputLogic) {
            return;
        }
        
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        if (!digits || !entered) {
            return;
        }
        
        auto result = TimePreviewLogic::generatePreview(digits, entered, timeProvider_, false);
        if (result.isValid) {
            strncpy(preview, result.preview.c_str(), previewSize - 1);
            preview[previewSize - 1] = '\0';
        }
    }

    void onDraw() override {
        updateDigitDisplay();
        updatePreviewDisplay();
        updateColonDisplay();
    }
    
    // Public methods
    void setView(IInputDisplayView* v) { view = v; }
    // StateManager, MainDisplayStateのsetterを追加
    void setManager(StateManager* m) { manager = m; }
    void setMainDisplayState(IState* mainState) { mainDisplayState = mainState; }
    
    // 相対値入力モードの設定
    void setRelativeMode(bool relative) { isRelativeMode = relative; }
    bool getRelativeMode() const { return isRelativeMode; }
    
    // テスト用: inputLogicを直接セット
    void setInputLogicForTest(InputLogic* logic) { inputLogic = logic; }
    
    // ITimeProviderのsetter
    void setTimeProvider(ITimeProvider* timeProvider) { timeProvider_ = timeProvider; }

private:
    // エラーメッセージ定数
    static constexpr const char* ERROR_EMPTY_INPUT = "Input is empty.";
    static constexpr const char* ERROR_ADD_ALARM_FAILED = "Failed to add alarm.";
    static constexpr const char* ERROR_INVALID_TIME = "Invalid time format.";
    static constexpr time_t ERROR_DISPLAY_DURATION = 3;
    
    InputLogic* inputLogic;
    IInputDisplayView* view;
    ITimeProvider* timeProvider_;
    int lastDigits[4] = {-1,-1,-1,-1};
    bool lastEntered[4] = {false,false,false,false};
    StateManager* manager;
    IState* mainDisplayState;
    bool isRelativeMode;
    
    // エラーメッセージ表示管理
    std::string errorMessage;
    bool showError;
    time_t errorStartTime;
    
    // プレビュー内容の変化チェック用
    std::string lastPreview;

    // 現在時刻を安全に取得
    time_t getCurrentTime() const {
        return timeProvider_ ? timeProvider_->now() : 0;
    }
    
    // 時刻取得が有効かチェック
    bool isTimeProviderValid() const {
        return timeProvider_ != nullptr;
    }

    // 数字表示の更新
    void updateDigitDisplay() {
        if (!inputLogic || !view) {
            return;
        }
        
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        
        if (hasInputChanged(digits, entered)) {
            for (int i = 0; i < 4; ++i) {
                view->showDigit(i, digits[i], entered[i]);
                lastDigits[i] = digits[i];
                lastEntered[i] = entered[i];
            }
        }
    }
    
    // プレビュー表示の更新
    void updatePreviewDisplay() {
        if (!view) {
            return;
        }
        
        char preview[32] = "";
        generatePreviewText(preview, sizeof(preview));
        
        // プレビュー内容の変化チェック
        std::string currentPreview(preview);
        if (currentPreview != lastPreview) {
            view->showPreview(preview);
            lastPreview = currentPreview;
        }
    }
    
    // コロン表示の更新
    void updateColonDisplay() {
        if (view) {
            view->showColon();
        }
    }
    
    // 入力値の変化をチェック
    bool hasInputChanged(const int* digits, const bool* entered) {
        if (!digits || !entered) {
            return false;
        }
        
        for (int i = 0; i < 4; ++i) {
            if (digits[i] != lastDigits[i] || entered[i] != lastEntered[i]) {
                return true;
            }
        }
        return false;
    }
    
    // プレビューテキスト生成
    void generatePreviewText(char* preview, size_t previewSize) {
        if (!preview || previewSize == 0) {
            return;
        }
        
        // エラーメッセージ表示の管理
        if (showError) {
            handleErrorDisplay(preview, previewSize);
            return;
        }
        
        int value = inputLogic ? inputLogic->getValue() : InputLogic::EMPTY_VALUE;
        
        if (value != InputLogic::EMPTY_VALUE) {
            // 完全入力時のプレビュー表示
            generateCompleteInputPreview(preview, previewSize);
        } else {
            // 部分入力時のプレビュー表示
            generatePartialInputPreview(preview, previewSize);
        }
    }
    
    // エラー表示の処理（改善版）
    void handleErrorDisplay(char* preview, size_t previewSize) {
        if (isErrorExpired()) {
            resetErrorState();
        } else {
            strncpy(preview, errorMessage.c_str(), previewSize - 1);
            preview[previewSize - 1] = '\0';
        }
    }
    
    // 完全入力時のプレビュー生成
    void generateCompleteInputPreview(char* preview, size_t previewSize) {
        if (isRelativeMode) {
            generateRelativePreview(preview, previewSize);
        } else {
            generateAbsolutePreview(preview, previewSize);
        }
    }
    
    // 部分入力時のプレビュー生成
    void generatePartialInputPreview(char* preview, size_t previewSize) {
        const int* digits = inputLogic ? inputLogic->getDigits() : nullptr;
        const bool* entered = inputLogic ? inputLogic->getEntered() : nullptr;
        
        if (!digits || !entered) {
            return;
        }
        
        auto parsedTime = PartialInputLogic::parsePartialInput(digits, entered);
        if (!hasAnyInput(entered) || !parsedTime.isValid) {
            return;
        }
        
        if (isRelativeMode) {
            generateRelativePreview(preview, previewSize);
        } else {
            generateAbsolutePreview(preview, previewSize);
        }
    }
    
    // 入力があるかチェック
    bool hasAnyInput(const bool* entered) {
        if (!entered) {
            return false;
        }
        
        for (int i = 0; i < 4; ++i) {
            if (entered[i]) {
                return true;
            }
        }
        return false;
    }
    
    // 相対値モードの確定処理
    void handleRelativeModeSubmit(bool& success, bool& error) {
        time_t relativeTime = inputLogic->getAbsoluteValue();
        if (relativeTime != -1) {
            success = addAlarmAtTime(relativeTime);
            if (!success) {
                handleErrorWithValidation(ERROR_ADD_ALARM_FAILED, error);
            }
        } else {
            handleErrorWithValidation(ERROR_EMPTY_INPUT, error);
        }
    }
    
    // 絶対値モードの確定処理
    void handleAbsoluteModeSubmit(bool& success, bool& error) {
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        if (digits && entered) {
            time_t now = time(nullptr);
            extern std::vector<time_t> alarm_times;
            AlarmLogic::AddAlarmResult result;
            std::string msg;
            bool ok = AlarmLogic::addAlarmFromPartialInput(alarm_times, now, digits, entered, result, msg);
            if (ok) {
                success = true;
            } else {
                handleErrorWithValidation(msg, error);
            }
        }
    }
    
    // アラーム追加処理
    bool addAlarmAtTime(time_t time) {
        extern std::vector<time_t> alarm_times;
        AlarmLogic::AddAlarmResult result;
        std::string msg;
        return AlarmLogic::addAlarmAtTime(alarm_times, time, result, msg);
    }
    
    // メイン画面への遷移
    void transitionToMainDisplay() {
        if (manager && mainDisplayState) {
            manager->setState(mainDisplayState);
        }
    }
    
    // エラー状態管理関数
    void resetErrorState() {
        showError = false;
        errorMessage = "";
        errorStartTime = 0;
    }
    
    bool isErrorActive() const {
        return showError;
    }
    
    bool isErrorExpired() const {
        return getCurrentTime() - errorStartTime >= ERROR_DISPLAY_DURATION;
    }
    
    // 統一されたエラー処理
    void handleErrorWithValidation(const std::string& message, bool& error) {
        handleError(message);
        error = true;
    }
    
    // 入力検証関数
    bool validateInputLogic() {
        return inputLogic != nullptr;
    }
    
    bool validateTimeProvider() {
        return isTimeProviderValid();
    }
    
    // 共通エラー処理（改善版）
    void handleError(const std::string& message) {
        showError = true;
        errorMessage = message;
        errorStartTime = getCurrentTime();
    }
    void onButtonA() override {
        if (inputLogic) {
            inputLogic->incrementInput(1);
        }
        onDraw();
    }
    void onButtonB() override {
        if (inputLogic) {
            // 桁送りを試行
            bool success = inputLogic->shiftDigits();
            // 成功時のみUI反映（失敗時は何もしない）
            if (success) {
                onDraw();
            }
        }
    }
    void onButtonC() override {
        if (!inputLogic) return;
        
        bool success = false;
        bool error = false;
        
        if (isRelativeMode) {
            handleRelativeModeSubmit(success, error);
        } else {
            handleAbsoluteModeSubmit(success, error);
        }
        
        if (success && !error) {
            transitionToMainDisplay();
        }
    }
    void onButtonALongPress() override {
        if (inputLogic) {
            inputLogic->incrementInput(5);
        }
        onDraw();
    }
    void onButtonBLongPress() override {
        if (inputLogic) {
            // リセット
            inputLogic->reset();
            // 絶対入力モードでは __:_0 に統一、REL+は従来どおり完全クリア
            if (!isRelativeMode) {
                inputLogic->incrementInput(0);
            }
            onDraw();
        }
    }
    void onButtonCLongPress() override {
        if (manager && mainDisplayState) {
            manager->setState(mainDisplayState);
        }
    }
}; 