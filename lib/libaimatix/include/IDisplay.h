#pragma once
// LCD表示の抽象インターフェース
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void clear() = 0; // 画面クリア
    virtual void drawText(int x, int y, const char* text, int fontSize) = 0; // テキスト描画
    virtual void setTextColor(uint32_t color) = 0; // 文字色設定
    virtual void fillRect(int x, int y, int w, int h, uint32_t color) = 0; // 矩形塗りつぶし
    // 必要に応じて追加
}; 