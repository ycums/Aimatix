#pragma once
#include "ISettingsLogic.h"
#include "ui_constants.h"
#include <string>

class SettingsLogic : public ISettingsLogic {
public:
    SettingsLogic();

    auto getLcdBrightness() const -> int override;
    void setLcdBrightness(int value) override;
    auto isSoundEnabled() const -> bool override;
    void setSoundEnabled(bool enabled) override;

    auto getSelectedItem() const -> SettingsItem override;
    void setSelectedItem(SettingsItem item) override;
    auto getItemCount() const -> int override;
    auto getItemByIndex(int index) const -> SettingsItem override;
    auto getIndexByItem(SettingsItem item) const -> int override;

    auto getItemDisplayName(SettingsItem item) const -> std::string override;
    auto getItemValueString(SettingsItem item) const -> std::string override;

    auto isValueEditMode() const -> bool override;
    void setValueEditMode(bool editMode) override;

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

inline SettingsLogic::SettingsLogic() { resetSettings(); }
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
inline auto SettingsLogic::getItemCount() const -> int { return 4; }
inline auto SettingsLogic::getItemByIndex(int index) const -> SettingsItem {
    if (index < 0 || index >= getItemCount()) { return SettingsItem::SOUND; }
    return static_cast<SettingsItem>(index);
}
inline auto SettingsLogic::getIndexByItem(SettingsItem item) const -> int { return static_cast<int>(item); }
inline auto SettingsLogic::getItemDisplayName(SettingsItem item) const -> std::string {
    switch (item) {
        case SettingsItem::SOUND: return "SOUND";
        case SettingsItem::LCD_BRIGHTNESS: return "LCD BRIGHTNESS";
        case SettingsItem::SET_DATE_TIME: return "TIME SYNC";
        case SettingsItem::INFO: return "INFO";
        default: return "UNKNOWN";
    }
}
inline auto SettingsLogic::getItemValueString(SettingsItem item) const -> std::string {
    switch (item) {
        case SettingsItem::SOUND: return soundEnabled_ ? "ON" : "OFF";
        case SettingsItem::LCD_BRIGHTNESS: return std::to_string(lcdBrightness_);
        default: return "";
    }
}
inline auto SettingsLogic::isValueEditMode() const -> bool { return valueEditMode_; }
inline void SettingsLogic::setValueEditMode(bool editMode) { valueEditMode_ = editMode; }
inline void SettingsLogic::loadSettings() { resetSettings(); }
inline void SettingsLogic::saveSettings() const {}
inline void SettingsLogic::resetSettings() {
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


