#include "mock_eeprom.h"

MockEEPROM::MockEEPROM() {}
MockEEPROM::~MockEEPROM() {}

uint8_t MockEEPROM::read(int address) {
    return memory[address];
}

void MockEEPROM::write(int address, uint8_t value) {
    memory[address] = value;
}

void MockEEPROM::commit() {}

void MockEEPROM::clear() {
    memory.clear();
} 