#include "idt.h"
#include "exceptions.h"

__attribute__((aligned(0x10)))
static IDTEntry_t _IDT[MAX_INTERRUPTS];

void lidt(ptr_t base, uint16_t size){
    struct __attribute__((packed)) {
        uint16_t size;
        uint32_t base;
    } _IDTr = {size, (uint32_t)base};
    __asm__ __volatile__ (
            "cli;"
            "lidt %0;"
            "sti"::"m"(_IDTr)
    );
}
void sidt(IDTr_t* ptr){
    __asm__ __volatile__ ("cli\n\t"
            "sidt %0\n\t"
            "sti"::"m"(ptr));
}

bool_t idt_install_handler(uint32_t index, uint16_t flags, uint16_t selector, InterruptHandler_t handler) {
    if (index > MAX_INTERRUPTS) return false;
    if (!handler) return false;
    uint64_t base          = (uint64_t)handler;
    _IDT[index].BaseHigh   = (base >> 16) & 0xFFFF;
    _IDT[index].Flags    = flags;
    _IDT[index]._reserved   = 0;
    _IDT[index].Selector   = selector;
    _IDT[index].BaseLow    = (base >> 0)  & 0xFFFF;
    return true;
}

__attribute__((interrupt))  void default_interrupt_handler(ptr_t stack_top) {
}


void initialize_idt(uint16_t codeSel) {
    memset(&_IDT[0], 0, sizeof(IDTEntry_t)*MAX_INTERRUPTS -1);
    for(int i = 0; i < 19; i++)
        idt_install_handler(i, IDT_DESC_PRESENT|IDT_DESC_BIT32,
                        codeSel, (InterruptHandler_t)get_default_exception_handler(i));
    for(int i = 19; i < MAX_INTERRUPTS; i++)
        idt_install_handler(i, IDT_DESC_PRESENT|IDT_DESC_BIT32,
                        codeSel, (InterruptHandler_t)&default_interrupt_handler);

    lidt(&_IDT[0], MAX_INTERRUPTS*sizeof(IDTEntry_t)-1);
	sti();
}

void idt_set_vector(uint32_t index, InterruptHandler_t vect) {
    idt_install_handler(index, IDT_DESC_PRESENT|IDT_DESC_BIT32, 0x8, vect);
}
InterruptHandler_t idt_get_vector(uint32_t index) {
    IDTEntry_t* desc = &_IDT[index];
    if(!desc) return 0;
    addr_t addr = desc->BaseLow | (desc->BaseHigh << 16);
    InterruptHandler_t handler = (InterruptHandler_t)addr;
    return handler;
}