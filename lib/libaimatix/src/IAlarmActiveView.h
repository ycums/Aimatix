#pragma once

class IAlarmActiveView {
public:
    virtual ~IAlarmActiveView() {}
    // 中央コンテンツのみを反転描画（タイトル/ナビを除外、プログレスは含む）
    virtual void drawFlashOverlay(bool on) = 0;
};


