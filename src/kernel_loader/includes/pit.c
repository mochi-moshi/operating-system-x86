#include "pit.h"
#include "idt.h"
#include "pic.h"
// #include "task.h"


void pit_send_command(uint8_t command) {
    outb(PIT_COMMAND_PORT, command);
}

void pit_send_data(uint8_t counter, uint16_t data) {
    uint8_t port = 0x40 + (counter >> 6);
    outb(port, (uint8_t)data);
}

uint8_t pit_read_data(uint8_t counter) {
    uint8_t port = 0x40 + (counter >> 6);
    return inb(port);
}

void pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode) {
    if(freq == 0) return;
    uint16_t divisor;
    if(freq > 18) {
        divisor = (uint16_t)(1193181/(uint16_t)freq);
    } else {
        divisor = 0xFFFF;
    }

    pit_send_command(mode | RL_DATA | counter);

    cli();

    pit_send_data(divisor&0xFF,      counter);
    pit_send_data((divisor>>8)&0xFF, counter);
    pic_enable_irq(0); // 0x20
    sti();
}