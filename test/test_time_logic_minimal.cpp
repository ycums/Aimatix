#include "time_logic.h"
#include <cstdio>

int main() {
    bool result = TimeLogic::isValidTime(0, 0);
    printf("isValidTime(0,0) = %d\n", result ? 1 : 0);
    return 0;
} 