#include "pic.h"
#include "idt.h"
#include "stddef.h"

uint8_t pic_read_data(uint8_t picNum) {
    if(picNum > 1) return 0;
    return inb((picNum==1) ? PIC2_REG_DATA_PORT : PIC1_REG_DATA_PORT);
}

void pic_send_data(uint8_t picNum, uint8_t data) {
    if(picNum > 1) return;
    outb((picNum==1) ? PIC2_REG_DATA_PORT : PIC1_REG_DATA_PORT,data);
}

void pic_send_command(uint8_t picNum, uint8_t command) {
    if(picNum > 1) return;
    outb((picNum==1) ? PIC2_REG_COMMAND_PORT : PIC1_REG_COMMAND_PORT,command);
}
void pic_mask_irq(uint8_t picNum, uint8_t mask) {
    if(picNum > 1) return;
    outb((picNum==1) ? PIC2_REG_DATA_PORT : PIC1_REG_DATA_PORT, mask);
}

void pic_disable_irq(uint8_t line) {
    uint8_t port = (line < 8)?PIC1_REG_DATA_PORT:PIC2_REG_DATA_PORT;
    line = (line<8)?line:line-8;
    uint8_t value = inb(port) | (1 << line);
    outb(port, value);
}
void pic_enable_irq(uint8_t line) {
    uint8_t port = (line < 8)?PIC1_REG_DATA_PORT:PIC2_REG_DATA_PORT;
    line = (line<8)?line:line-8;
    uint8_t value = inb(port) & ~(1 << line);
    outb(port, value);
}

__attribute__((interrupt)) void irq7_handler(ptr_t stack) {
    // pic_eoi(7);
}

uint8_t BASE0, BASE1;

void __initialize_pic(uint8_t base0, uint8_t base1)  {
    BASE0 = base0;
    BASE1 = base1;
}
void initialize_pic(uint8_t base0, uint8_t base1) {
    BASE0 = base0;
    BASE1 = base1;
    idt_install_handler(base0+7, IDT_DESC_PRESENT|IDT_DESC_BIT32, 0x8, (InterruptHandler_t)&irq7_handler);

    pic_send_command(0, PIC_IC4_EXPECT | PIC_INIT);
    pic_send_command(1, PIC_IC4_EXPECT | PIC_INIT);

    pic_send_data(0, base0);
    pic_send_data(1, base1);

    pic_send_data(0, 4);
    pic_send_data(1, 2);

    pic_send_data(0, PIC_UPM_86MODE);
    pic_send_data(1, PIC_UPM_86MODE);

    pic_send_data(0, 0xFF); // mask all interupts
    pic_send_data(1, 0xFF);
}

void pic_set_irq(uint8_t index, InterruptHandler_t handler) {
    idt_install_handler(BASE0+index, IDT_DESC_PRESENT|IDT_DESC_BIT32, 0x8, handler);
}

__attribute__((no_caller_saved_registers))
void pic_eoi(uint8_t intno) {
    if(intno > 16) return;
    if(intno >= 8) pic_send_command(1, PIC_OCW2_MASK_EOI);
    pic_send_command(0,PIC_OCW2_MASK_EOI);
}
uint16_t pic_get_irr(void) {
    pic_send_command(0, PIC_READ_IRR);
    pic_send_command(1, PIC_READ_IRR);
    return (pic_read_data(1) << 8) | pic_read_data(0);
}
uint16_t pic_get_isr(void) {
    pic_send_command(0,PIC_READ_ISR);
    pic_send_command(1,PIC_READ_ISR);
    return (pic_read_data(1) << 8) | pic_read_data(0);
}