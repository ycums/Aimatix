# 純粋ロジックインターフェース設計書

本ドキュメントは、Aimatixの純粋ロジック層（TimeLogic, AlarmLogic, InputLogic, SettingsLogic等）のインターフェース設計を明文化するものです。

---

## 1. TimeLogic インターフェース

### 責務
- 現在時刻の管理・取得・加算・比較・フォーマット変換
- 時刻計算の純粋ロジック（ハード依存なし）

### 主API例
```cpp
class ITimeLogic {
public:
    virtual ~ITimeLogic() {}
    virtual void setCurrentTime(int hour, int minute) = 0;
    virtual void getCurrentTime(int& hour, int& minute) const = 0;
    virtual void addMinutes(int minutes) = 0;
    virtual bool isBefore(int hour, int minute) const = 0;
    virtual std::string formatTime() const = 0;
    // 必要に応じて拡張
};
```

### 設計意図
- RTC等のハード依存を排除し、テスト容易性・再利用性を確保

---

## 2. AlarmLogic インターフェース

### 責務
- アラームの追加・削除・一覧取得・鳴動判定
- アラーム管理の純粋ロジック

### 主API例
```cpp
class IAlarmLogic {
public:
    virtual ~IAlarmLogic() {}
    virtual bool addAlarm(int hour, int minute) = 0;
    virtual bool removeAlarm(int index) = 0;
    virtual void clearAlarms() = 0;
    virtual int getAlarmCount() const = 0;
    virtual void getAlarmTime(int index, int& hour, int& minute) const = 0;
    virtual int findNextAlarm(int currentHour, int currentMinute) const = 0;
    virtual bool isAlarmActive(int hour, int minute) const = 0;
    // 必要に応じて拡張
};
```

### 設計意図
- アラーム管理・鳴動判定を純粋ロジックで一元化し、UI/ハード依存を排除

---

## 3. InputLogic インターフェース

### 責務
- 入力値のバリデーション・編集・確定処理
- 入力状態の管理

### 主API例
```cpp
class IInputLogic {
public:
    virtual ~IInputLogic() {}
    virtual void setInput(int hour, int minute) = 0;
    virtual void getInput(int& hour, int& minute) const = 0;
    virtual bool validateInput() const = 0;
    virtual void clearInput() = 0;
    virtual bool moveCursor() = 0;  // 桁送り機能
    virtual int getCursor() const = 0;  // カーソル位置取得
    // 必要に応じて拡張
};
```

### 設計意図
- 入力値の検証・編集・カーソル移動をUIから分離し、テスト容易性・再利用性を確保
- 桁送り機能は純粋ロジックとして実装し、成功/失敗を戻り値で返す

---

## 4. SettingsLogic インターフェース

### 責務
- 各種設定（LCD明度、音設定、Wi-Fi設定等）のロード・保存・リセット・バリデーション
- 設定データの管理

### 主API例
```cpp
class ISettingsLogic {
public:
    virtual ~ISettingsLogic() {}
    virtual void loadSettings(IEEPROM* eeprom) = 0;
    virtual void saveSettings(IEEPROM* eeprom) const = 0;
    virtual void resetSettings() = 0;
    virtual bool validateSettings() const = 0;
    // 設定値のgetter/setter例
    virtual int getLcdBrightness() const = 0;
    virtual void setLcdBrightness(int value) = 0;
    virtual bool isSoundEnabled() const = 0;
    virtual void setSoundEnabled(bool enabled) = 0;
    // 必要に応じて拡張
};
```

### 設計意図
- 設定管理・永続化処理を純粋ロジックで一元化し、UI/ハード依存を排除

---

## 補足
- すべてのインターフェースは「単一責任」「DIによるテスト容易性」「拡張性」を重視
- 実装・テスト段階でAPI追加・修正が必要な場合は本ドキュメントを随時更新 