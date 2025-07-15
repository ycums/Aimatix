#pragma once
// スピーカー制御の抽象インターフェース
class ISpeaker {
public:
    virtual ~ISpeaker() {}
    virtual void beep(int freq, int durationMs) = 0; // ビープ音
    virtual void stop() = 0; // 停止
    // 必要に応じて追加
}; 