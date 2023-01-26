#pragma once
#include "stddef.h"
#include "idt.h"

#define PIC1_REG_COMMAND_PORT    0x20
#define PIC1_REG_STATUS_PORT     0x20
#define PIC1_REG_DATA_PORT       0x21
#define PIC1_REG_IMR_PORT        0x21

#define PIC2_REG_COMMAND_PORT    0xA0
#define PIC2_REG_STATUS_PORT     0xA0
#define PIC2_REG_DATA_PORT       0xA1
#define PIC2_REG_IMR_PORT        0xA1

#define PIC_OCW2_MASK_EOI        0x20

enum PIC_IRQ {
    PIC_TIMER,
    PIC_CMOSTIMER = 0,
    PIC_KEYBOARD,
    PIC_CGARETRACE = 1,
    PIC_SERIAL2 = 3,
    PIC_SERIAL1,
    PIC_AUXILIARY = 4,
    PIC_PARALLEL2,
    PIC_FPU = 5,
    PIC_DISKETTE,
    PIC_HDC = 6,
    PIC_PARALLEL1
};

enum PIC_ICW1 {
    PIC_IC4_EXPECT = 1,
    PIC_SNGL = 2,
    PIC_ADI_CALLINTERVAL8 = 0,
    PIC_ADI_CALLINTERVAL4 = 4,
    PIC_LTIM_EDGETRIGGERED = 0,
    PIC_LTIM_LEVELTRIGGERED = 8,
    PIC_INIT = 16
};
enum PIC_ICW4 {
    PIC_UPM_MCSMODE = 0,
    PIC_UPM_86MODE = 1,
    PIC_AEOI_AUTOEOI = 2,
    PIC_MS_BUFFERSLAVE = 0,
    PIC_MS_BUFFERMASTER = 4,
    PIC_BUF_MODE = 8,
    PIC_SFNM_NESTEDMODE = 16
};

enum PIC_OCW3 {
    PIC_READ_IRR = 0xA,
    PIC_READ_ISR = 0xB
};

uint8_t pic_read_data(uint8_t picNum);
void pic_send_data(uint8_t picNum, uint8_t data);
void pic_send_command(uint8_t picNum, uint8_t command);
void pic_mask_irq(uint8_t picNum, uint8_t mask);
void pic_enable_irq(uint8_t line);
void pic_disable_irq(uint8_t line);
void initialize_pic(uint8_t base0, uint8_t base1);
void __initialize_pic(uint8_t base0, uint8_t base1);

void pic_eoi(uint8_t intno) __attribute__((no_caller_saved_registers));

void pic_set_irq(uint8_t index, InterruptHandler_t handler);
void irq7_handler(ptr_t stack) __attribute__((interrupt));

uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);