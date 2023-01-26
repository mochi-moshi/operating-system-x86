#pragma once
#include "stddef.h"

#define PIT_COUNTER0_PORT    0x40
#define PIT_COUNTER1_PORT    0x41
#define PIT_COUNTER2_PORT    0x42
#define PIT_COMMAND_PORT     0x43

enum PIT_OCW {
    BINCOUNT_BINARY = 0,
    BINCOUT_BCD = 1,
    MODE_TERMINALCOUNT = 0,
    MODE_ONESHOT = 2,
    MODE_RATEGEN = 4,
    MODE_SQUAREWAVEGEN = 6,
    MODE_SOFTWARETRIG = 8,
    MODE_HARDWARETRIG = 10,
    RL_LATCH = 0,
    RL_LSBONLY = 0x10,
    RL_MSBONLY = 0x20,
    RL_DATA = 0x30
};
enum PIT_COUNTER {
    COUNTER0 = 0,
    COUNTER1 = 0x40,
    COUNTER2 = 0x80
};

void pit_send_command(uint8_t command);
void pit_send_data(uint8_t counter, uint16_t data);
uint8_t pit_read_data(uint8_t counter);
void pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode);