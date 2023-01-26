#pragma once
#include "stddef.h"

typedef struct  __attribute__((packed))
{
    uint16_t BaseLow;
    uint16_t Selector;
    uint8_t _reserved;
    // union {
    //     struct {
    //         char type    : 4;   // 0x5 = task gate, 0x6 = 16b int, 0x7 = 16b trap, 0xE = 32b int, 0xF = 32b trap
    //         char res0    : 1;   // set to 0
    //         char dpl     : 2;   // privilege level
    //         char present : 1;   // set to 1, valid gate
    //     } __attribute__((packed)) flags;
        uint8_t Flags;
    uint16_t BaseHigh;
} IDTEntry_t;

typedef struct __attribute__((packed))
{
    uint16_t Size;
    uint32_t Base;
} IDTr_t;

typedef struct __attribute__((packed))
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t sp;
    uint32_t ss;
} interrupt_frame_t;

#define IDT_DESC_BIT16      0b00000110
#define IDT_DESC_BIT32      0b00001110
#define IDT_DESC_RING1      0b01000000
#define IDT_DESC_RING2      0b00100000
#define IDT_DESC_RING3      0b01100000
#define IDT_DESC_PRESENT    0b10000000
#define MAX_INTERRUPTS      256
typedef void (*InterruptHandler_t)(ptr_t stack_top) __attribute__((interrupt));

void lidt(ptr_t base, uint16_t size);

void sidt(IDTr_t* ptr);

bool_t idt_install_handler(uint32_t index, uint16_t flags, uint16_t selector, InterruptHandler_t handler);

void default_interrupt_handler(ptr_t stack_top) __attribute__((interrupt));

void initialize_idt(const uint16_t codeSel);

void idt_set_vector(uint32_t index, InterruptHandler_t vect);
InterruptHandler_t idt_get_vector(uint32_t index);