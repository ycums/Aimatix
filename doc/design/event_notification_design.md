# イベント通知方式設計書（Phase 2.4.4対応）

## 目的
- UI層とロジック層の責務分離を徹底し、ロジック層の処理結果（入力確定・設定保存等）をUI層へ明確に通知する方式を設計する。
- テスト容易性・拡張性・保守性の向上。
- **Aimatixではコマンド/イベント駆動設計を標準方針とし、全ての副作用（UI更新・保存・通信等）をこの方式で一元管理する。**

---

## 1. 基本方針
- ロジック層は「値の検証・変換・保存」などの純粋ロジックのみ担当し、UI遷移や画面制御は一切行わない。
- UI層が「入力確定/保存」イベントを受けて、画面遷移やUI反映を一元的に制御する。
- ロジック層のAPIは「必要な情報・成否」だけを戻り値やイベントでUI層に通知する。
- 副作用（保存・通信等）はインターフェース経由またはコマンド/イベント駆動で一元管理する。

---

## 2. イベント通知方式の設計例

### 2.1 戻り値による通知
- ロジック層のAPIは、処理結果（成功/失敗・必要な情報）を戻り値や構造体で返す。

```cpp
// 入力確定処理
struct InputResult {
    bool success;
    int hour;
    int minute;
    std::string errorMessage;
};
InputResult confirmInput(const InputData& input);

// 設定保存処理
bool saveSettings(const Settings& settings, IEEPROM* eeprom);
```

### 2.2 コールバック/イベントリスナー方式
- 必要に応じて、UI層がリスナー/コールバックを登録し、ロジック層が処理完了時に通知する方式も選択可能。

```cpp
// コールバック型の例
using SaveCallback = std::function<void(bool success, const std::string& message)>;
void saveSettingsAsync(const Settings& settings, IEEPROM* eeprom, SaveCallback callback);
```

### 2.3 コマンド/イベント駆動方式
- 状態遷移の結果として「UI更新」「設定保存」などのコマンド/イベントリストを返す設計を**標準方式**とする。

```cpp
// コマンド型の例
enum class CommandType {
    UpdateUI,
    SaveSettings,
    ShowWarning,
    // ... 必要に応じて拡張
};
struct Command {
    CommandType type;
    // 必要に応じてパラメータを追加
};
std::vector<Command> handleEvent(const Event& event);
```

---

## 3. UI層での受け取り・画面遷移制御の例

```cpp
// 設定保存ボタン押下時
if (saveSettings(settings, &eepromAdapter)) {
    currentMode = MAIN_DISPLAY; // 保存成功時に画面遷移
    drawMainDisplay();
} else {
    showWarning("保存に失敗しました");
}

// コマンド駆動方式の場合
for (const auto& cmd : handleEvent(event)) {
    switch (cmd.type) {
        case CommandType::UpdateUI:
            drawMainDisplay();
            break;
        case CommandType::SaveSettings:
            saveSettings(settings, &eepromAdapter);
            break;
        case CommandType::ShowWarning:
            showWarning(cmd.message);
            break;
    }
}
```

---

## 4. メリット
- UI層とロジック層の責務分離が明確になり、テスト容易性・拡張性が向上
- UI以外の副作用（保存・通信等）も同じ仕組みで柔軟に追加可能
- 設計・実装・レビュー・保守が容易になる

---

## 5. 今後の拡張方針
- プロジェクトの規模や要件に関わらず、コマンド/イベント駆動方式を標準とし、Effect/Command Dispatcherによる副作用一元管理を必須とする。

--- 