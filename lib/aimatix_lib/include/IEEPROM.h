#pragma once
#include <cstdint>
// EEPROMアクセスの抽象インターフェース
class IEEPROM {
public:
    virtual ~IEEPROM() {}
    virtual uint8_t read(int address) = 0; // 読み出し
    virtual void write(int address, uint8_t value) = 0; // 書き込み
    virtual void commit() = 0; // フラッシュ保存
    // 必要に応じて追加
}; 