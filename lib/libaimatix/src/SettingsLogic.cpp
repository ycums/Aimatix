#include "SettingsLogic.h"
#include <cassert>
#include <string>

constexpr int DEFAULT_LCD_BRIGHTNESS = 150;
constexpr int MIN_LCD_BRIGHTNESS = 50;
constexpr int MAX_LCD_BRIGHTNESS = 250;

class SettingsLogic : public ISettingsLogic {
public:
    SettingsLogic() {
        resetSettings();
    }
    
    // 設定値のgetter/setter
    auto getLcdBrightness() const -> int override { return lcdBrightness_; }
    void setLcdBrightness(int value) override { 
        if (value >= MIN_LCD_BRIGHTNESS && value <= MAX_LCD_BRIGHTNESS) {
            lcdBrightness_ = value;
        }
    }
    auto isSoundEnabled() const -> bool override { return soundEnabled_; }
    void setSoundEnabled(bool enabled) override { soundEnabled_ = enabled; }
    
    // 設定項目の管理（論理的識別子ベース）
    auto getSelectedItem() const -> SettingsItem override { return selectedItem_; }
    void setSelectedItem(SettingsItem item) override { selectedItem_ = item; }
    auto getItemCount() const -> int override { return 4; } // SOUND, LCD_BRIGHTNESS, SET_DATE_TIME, INFO
    
    auto getItemByIndex(int index) const -> SettingsItem override {
        if (index < 0 || index >= getItemCount()) {
            return SettingsItem::SOUND; // デフォルト
        }
        return static_cast<SettingsItem>(index);
    }
    
    auto getIndexByItem(SettingsItem item) const -> int override {
        return static_cast<int>(item);
    }
    
    // 設定項目の表示名取得
    auto getItemDisplayName(SettingsItem item) const -> std::string override {
        switch (item) {
            case SettingsItem::SOUND: return "SOUND";
            case SettingsItem::LCD_BRIGHTNESS: return "LCD BRIGHTNESS";
            case SettingsItem::SET_DATE_TIME: return "SET DATE/TIME";
            case SettingsItem::INFO: return "INFO";
            default: return "UNKNOWN";
        }
    }
    
    auto getItemValueString(SettingsItem item) const -> std::string override {
        switch (item) {
            case SettingsItem::SOUND: 
                return soundEnabled_ ? "ON" : "OFF";
            case SettingsItem::LCD_BRIGHTNESS: 
                return std::to_string(lcdBrightness_);
            case SettingsItem::SET_DATE_TIME:
            case SettingsItem::INFO: 
                return ""; // 画面遷移項目なので値なし
            default: 
                return "";
        }
    }
    
    // モード管理
    auto isValueEditMode() const -> bool override { return valueEditMode_; }
    void setValueEditMode(bool editMode) override { valueEditMode_ = editMode; }
    
    // 永続化（stub実装）
    void loadSettings() override {
        // stub実装: 初期値を使用
        resetSettings();
    }
    
    void saveSettings() const override {
        // stub実装: 何もしない
    }
    
    void resetSettings() override {
        lcdBrightness_ = DEFAULT_LCD_BRIGHTNESS;
        soundEnabled_ = true;
        selectedItem_ = SettingsItem::SOUND;
        valueEditMode_ = false;
    }
    
    auto validateSettings() const -> bool override {
        return lcdBrightness_ >= MIN_LCD_BRIGHTNESS && lcdBrightness_ <= MAX_LCD_BRIGHTNESS;
    }

private:
    int lcdBrightness_ = DEFAULT_LCD_BRIGHTNESS;
    bool soundEnabled_ = true;
    SettingsItem selectedItem_ = SettingsItem::SOUND;
    bool valueEditMode_ = false;
}; 