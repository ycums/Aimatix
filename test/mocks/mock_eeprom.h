#pragma once
#include "../../lib/aimatix_lib/include/IEEPROM.h"
#include <map>

class MockEEPROM : public IEEPROM {
public:
    MockEEPROM();
    ~MockEEPROM() override;
    uint8_t read(int address) override;
    void write(int address, uint8_t value) override;
    void commit() override;
    // テスト用: メモリ内容をクリア
    void clear();
private:
    std::map<int, uint8_t> memory;
}; 