#pragma once

// ボタンタイプの定義（ButtonEvent構造体と統一）
enum ButtonType {
  BUTTON_TYPE_A,
  BUTTON_TYPE_B, 
  BUTTON_TYPE_C
};

// ボタン管理の抽象インターフェース
class IButtonManager {
public:
    virtual ~IButtonManager() {}
    virtual bool isPressed(ButtonType buttonId) = 0; // 押下状態取得
    virtual bool isLongPressed(ButtonType buttonId) = 0; // 長押し判定
    virtual void update() = 0; // 状態更新
    // 必要に応じて追加（同時押し、シーケンス等）
}; 