#pragma once
#include "../../lib/libaimatix/include/IEEPROM.h"
#include <cstring>

// 汎用的なモックEEPROMクラス
class MockEEPROM : public IEEPROM {
public:
    uint8_t data[512];
    bool write_called = false;
    bool read_called = false;
    bool commit_called = false;
    
    MockEEPROM() {
        memset(data, 0, sizeof(data));
    }
    
    void write(int address, uint8_t value) override {
        if (address >= 0 && address < 512) {
            data[address] = value;
            write_called = true;
        }
    }
    
    uint8_t read(int address) override {
        read_called = true;
        if (address >= 0 && address < 512) {
            return data[address];
        }
        return 0;
    }
    
    void commit() override {
        commit_called = true;
    }
    
    void reset() {
        memset(data, 0, sizeof(data));
        write_called = false;
        read_called = false;
        commit_called = false;
    }
}; 