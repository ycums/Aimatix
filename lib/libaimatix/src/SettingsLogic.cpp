#include "SettingsLogic.h"
#include <cassert>

// 定数定義
constexpr int DEFAULT_LCD_BRIGHTNESS = 150;
constexpr int MIN_LCD_BRIGHTNESS = 50;
constexpr int MAX_LCD_BRIGHTNESS = 250;
constexpr int SETTINGS_ITEM_COUNT = 4;

class SettingsLogic : public ISettingsLogic {
public:
    SettingsLogic() 
        : lcdBrightness_(DEFAULT_LCD_BRIGHTNESS), soundEnabled_(true), selectedItem_(SettingsItem::SOUND), 
          valueEditMode_(false) {
        resetSettings();
    }
    
    // 設定値のgetter/setter
    int getLcdBrightness() const override { return lcdBrightness_; }
    void setLcdBrightness(int value) override { 
        if (value >= MIN_LCD_BRIGHTNESS && value <= MAX_LCD_BRIGHTNESS) {
            lcdBrightness_ = value;
        }
    }
    bool isSoundEnabled() const override { return soundEnabled_; }
    void setSoundEnabled(bool enabled) override { soundEnabled_ = enabled; }
    
    // 設定項目の管理（論理的識別子ベース）
    SettingsItem getSelectedItem() const override { return selectedItem_; }
    void setSelectedItem(SettingsItem item) override { selectedItem_ = item; }
    int getItemCount() const override { return SETTINGS_ITEM_COUNT; } // SOUND, LCD_BRIGHTNESS, SET_DATE_TIME, INFO
    
    SettingsItem getItemByIndex(int index) const override {
        if (index < 0 || index >= getItemCount()) {
            return SettingsItem::SOUND; // デフォルト
        }
        return static_cast<SettingsItem>(index);
    }
    
    int getIndexByItem(SettingsItem item) const override {
        return static_cast<int>(item);
    }
    
    // 設定項目の表示名取得
    std::string getItemDisplayName(SettingsItem item) const override {
        switch (item) {
            case SettingsItem::SOUND: return "SOUND";
            case SettingsItem::LCD_BRIGHTNESS: return "LCD BRIGHTNESS";
            case SettingsItem::SET_DATE_TIME: return "SET DATE/TIME";
            case SettingsItem::INFO: return "INFO";
            default: return "UNKNOWN";
        }
    }
    
    std::string getItemValueString(SettingsItem item) const override {
        switch (item) {
            case SettingsItem::SOUND: 
                return soundEnabled_ ? "ON" : "OFF";
            case SettingsItem::LCD_BRIGHTNESS: 
                return std::to_string(lcdBrightness_);
            case SettingsItem::SET_DATE_TIME: 
                return ""; // 画面遷移項目なので値なし
            case SettingsItem::INFO: 
                return ""; // 画面遷移項目なので値なし
            default: 
                return "";
        }
    }
    
    // モード管理
    bool isValueEditMode() const override { return valueEditMode_; }
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
    
    bool validateSettings() const override {
        return lcdBrightness_ >= MIN_LCD_BRIGHTNESS && lcdBrightness_ <= MAX_LCD_BRIGHTNESS;
    }

private:
    int lcdBrightness_;
    bool soundEnabled_;
    SettingsItem selectedItem_;
    bool valueEditMode_;
}; 