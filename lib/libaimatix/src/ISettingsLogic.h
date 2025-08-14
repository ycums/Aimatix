#pragma once

#include <string>
#include <vector>

// Logical identifiers for settings items
enum class SettingsItem {
    SOUND = 0,
    LCD_BRIGHTNESS = 1,
    SET_DATE_TIME = 2,
    INFO = 3
};

// Pure interface for settings logic accessible from lib layer
class ISettingsLogic {
public:
    virtual ~ISettingsLogic() {}

    // Value getters/setters
    virtual int getLcdBrightness() const = 0;
    virtual void setLcdBrightness(int value) = 0;
    virtual bool isSoundEnabled() const = 0;
    virtual void setSoundEnabled(bool enabled) = 0;

    // Item management
    virtual SettingsItem getSelectedItem() const = 0;
    virtual void setSelectedItem(SettingsItem item) = 0;
    virtual int getItemCount() const = 0;
    virtual SettingsItem getItemByIndex(int index) const = 0;
    virtual int getIndexByItem(SettingsItem item) const = 0;

    // Display helpers
    virtual std::string getItemDisplayName(SettingsItem item) const = 0;
    virtual std::string getItemValueString(SettingsItem item) const = 0;

    // Mode management
    virtual bool isValueEditMode() const = 0;
    virtual void setValueEditMode(bool editMode) = 0;

    // Persistence (stub for now)
    virtual void loadSettings() = 0;
    virtual void saveSettings() const = 0;
    virtual void resetSettings() = 0;
    virtual bool validateSettings() const = 0;
};


