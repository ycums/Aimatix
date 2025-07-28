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