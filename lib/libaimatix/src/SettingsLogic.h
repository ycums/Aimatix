#pragma once
#include <string>
#include <cstddef>

// 設定項目の論理的な識別子
enum class SettingsItem {
    SOUND = 0,
    LCD_BRIGHTNESS = 1,
    SET_DATE_TIME = 2,
    INFO = 3
};

class ISettingsLogic {
public:
    virtual ~ISettingsLogic() {}
    
    // 設定値のgetter/setter
    virtual int getLcdBrightness() const = 0;
    virtual void setLcdBrightness(int value) = 0;
    virtual bool isSoundEnabled() const = 0;
    virtual void setSoundEnabled(bool enabled) = 0;
    
    // 設定項目の管理（論理的識別子ベース）
    virtual SettingsItem getSelectedItem() const = 0;
    virtual void setSelectedItem(SettingsItem item) = 0;
    virtual int getItemCount() const = 0;
    virtual SettingsItem getItemByIndex(int index) const = 0;
    virtual int getIndexByItem(SettingsItem item) const = 0;
    
    // 設定項目の表示名取得
    virtual std::string getItemDisplayName(SettingsItem item) const = 0;
    virtual std::string getItemValueString(SettingsItem item) const = 0;
    
    // モード管理
    virtual bool isValueEditMode() const = 0;
    virtual void setValueEditMode(bool editMode) = 0;
    
    // 永続化（stub実装）
    virtual void loadSettings() = 0;
    virtual void saveSettings() const = 0;
    virtual void resetSettings() = 0;
    virtual bool validateSettings() const = 0;
};

// 実装クラス
class SettingsLogic : public ISettingsLogic {
public:
    SettingsLogic();
    
    // 設定値のgetter/setter
    auto getLcdBrightness() const -> int override;
    void setLcdBrightness(int value) override;
    auto isSoundEnabled() const -> bool override;
    void setSoundEnabled(bool enabled) override;
    
    // 設定項目の管理（論理的識別子ベース）
    auto getSelectedItem() const -> SettingsItem override;
    void setSelectedItem(SettingsItem item) override;
    auto getItemCount() const -> int override;
    auto getItemByIndex(int index) const -> SettingsItem override;
    auto getIndexByItem(SettingsItem item) const -> int override;
    
    // 設定項目の表示名取得
    auto getItemDisplayName(SettingsItem item) const -> std::string override;
    auto getItemValueString(SettingsItem item) const -> std::string override;
    
    // モード管理
    auto isValueEditMode() const -> bool override;
    void setValueEditMode(bool editMode) override;
    
    // 永続化（stub実装）
    void loadSettings() override;
    void saveSettings() const override;
    void resetSettings() override;
    auto validateSettings() const -> bool override;

private:
    int lcdBrightness_;
    bool soundEnabled_;
    SettingsItem selectedItem_;
    bool valueEditMode_;
};

// 実装
inline SettingsLogic::SettingsLogic() {
    resetSettings();
}

inline auto SettingsLogic::getLcdBrightness() const -> int { return lcdBrightness_; }

inline void SettingsLogic::setLcdBrightness(int value) { 
    constexpr int MIN_LCD_BRIGHTNESS = 50;
    constexpr int MAX_LCD_BRIGHTNESS = 250;
    if (value >= MIN_LCD_BRIGHTNESS && value <= MAX_LCD_BRIGHTNESS) {
        lcdBrightness_ = value;
    }
}

inline auto SettingsLogic::isSoundEnabled() const -> bool { return soundEnabled_; }

inline void SettingsLogic::setSoundEnabled(bool enabled) { soundEnabled_ = enabled; }

inline auto SettingsLogic::getSelectedItem() const -> SettingsItem { return selectedItem_; }

inline void SettingsLogic::setSelectedItem(SettingsItem item) { selectedItem_ = item; }

inline auto SettingsLogic::getItemCount() const -> int { return 4; } // SOUND, LCD_BRIGHTNESS, SET_DATE_TIME, INFO

inline auto SettingsLogic::getItemByIndex(int index) const -> SettingsItem {
    if (index < 0 || index >= getItemCount()) {
        return SettingsItem::SOUND; // デフォルト
    }
    return static_cast<SettingsItem>(index);
}

inline auto SettingsLogic::getIndexByItem(SettingsItem item) const -> int {
    return static_cast<int>(item);
}

inline auto SettingsLogic::getItemDisplayName(SettingsItem item) const -> std::string {
    switch (item) {
        case SettingsItem::SOUND: return "SOUND";
        case SettingsItem::LCD_BRIGHTNESS: return "LCD BRIGHTNESS";
        case SettingsItem::SET_DATE_TIME: return "SET DATE/TIME";
        case SettingsItem::INFO: return "INFO";
        default: return "UNKNOWN";
    }
}

inline auto SettingsLogic::getItemValueString(SettingsItem item) const -> std::string {
    switch (item) {
        case SettingsItem::SOUND: 
            return soundEnabled_ ? "ON" : "OFF";
        case SettingsItem::LCD_BRIGHTNESS: 
            return std::to_string(lcdBrightness_);
        case SettingsItem::SET_DATE_TIME:
        case SettingsItem::INFO: 
        default: 
            return ""; // 画面遷移項目またはデフォルトなので値なし
    }
}

inline auto SettingsLogic::isValueEditMode() const -> bool { return valueEditMode_; }

inline void SettingsLogic::setValueEditMode(bool editMode) { valueEditMode_ = editMode; }

inline void SettingsLogic::loadSettings() {
    // stub実装: 初期値を使用
    resetSettings();
}

inline void SettingsLogic::saveSettings() const {
    // stub実装: 何もしない
}

inline void SettingsLogic::resetSettings() {
    constexpr int DEFAULT_LCD_BRIGHTNESS = 150;
    lcdBrightness_ = DEFAULT_LCD_BRIGHTNESS;
    soundEnabled_ = true;
    selectedItem_ = SettingsItem::SOUND;
    valueEditMode_ = false;
}

inline auto SettingsLogic::validateSettings() const -> bool {
    constexpr int MIN_LCD_BRIGHTNESS = 50;
    constexpr int MAX_LCD_BRIGHTNESS = 250;
    return lcdBrightness_ >= MIN_LCD_BRIGHTNESS && lcdBrightness_ <= MAX_LCD_BRIGHTNESS;
} 