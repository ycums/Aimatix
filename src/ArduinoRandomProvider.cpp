#include "ArduinoRandomProvider.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

uint64_t ArduinoRandomProvider::getRandom64() {
#ifdef ARDUINO
    // 非暗号用の簡易64bit生成（ミリ秒/マイクロ秒から拡散）
    uint64_t x = static_cast<uint64_t>(micros());
    x ^= (static_cast<uint64_t>(millis()) << 21);
    x ^= (x >> 35);
    x ^= (x << 4);
    return x;
#else
    return 0x12345678ULL;
#endif
}


