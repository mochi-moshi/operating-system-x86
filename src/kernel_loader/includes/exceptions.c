#include "exceptions.h"
#include "idt.h"
#include "vbe_driver.h"
// #include "virtual_memory_manager.h"

static ExceptionHandler_t error_handlers[19] = {
	(ExceptionHandler_t)divide_by_zero_fault,       // 0x00
	(ExceptionHandler_t)single_step,              // 0x01
	(ExceptionHandler_t)non_maskable_interrupt,     // 0x02
	(ExceptionHandler_t)break_point,              // 0x03
	(ExceptionHandler_t)overflow,                // 0x04
	(ExceptionHandler_t)bounds_check,             // 0x05
	(ExceptionHandler_t)undefined_opcode,          // 0x06
	(ExceptionHandler_t)no_coprocessor,            // 0x07
	(ExceptionHandler_t)double_fault,             // 0x08
	(ExceptionHandler_t)coprocessor_segment_overrun, // 0x09
	(ExceptionHandler_t)invalid_task_segment,       // 0x0a
	(ExceptionHandler_t)segment_not_present,        // 0x0b
	(ExceptionHandler_t)stack_segment_overrun,      // 0x0c
	(ExceptionHandler_t)general_protection_fault,    // 0x0d
	(ExceptionHandler_t)page_fault,               // 0x0e
	(ExceptionHandler_t)unassigned,               // 0x0f
	(ExceptionHandler_t)coprocessor_error,         // 0x10
	(ExceptionHandler_t)alignment_check,           // 0x11
	(ExceptionHandler_t)machine_check             // 0x12
};

ExceptionHandler_t get_default_exception_handler(size_t i) {
    return error_handlers[i];
}
__attribute__((noreturn))
void divide_by_zero_fault(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: divide by zero");
    // set_cursor(0,1);
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    // set_background(0xFF);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: divide_by_zero_fault");
    for(;;);
    frame.eip++;
}
void single_step(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: single step\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    // set_background(0xFF);
    // for(;;);
 }
 __attribute__((noreturn))
void non_maskable_interrupt(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: nmi\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    // set_background(0xFF);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: non_maskable_interrupt");
    for(;;);
 }__attribute__((noreturn))
void break_point(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: breakpoint\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: break_point");
    for(;;);
 }__attribute__((noreturn))
void overflow(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: overflow\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: overflow");
    for(;;);
 }__attribute__((noreturn))
void bounds_check(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: bounds check\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: bounds_check");
    for(;;);
 }__attribute__((noreturn))
void undefined_opcode(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // printf("Error: undef opcode\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: undefined_opcode");
    for(;;);
 }__attribute__((noreturn))
void no_coprocessor(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: no coprocessor\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: no_coprocessor");
    for(;;);
 }__attribute__((noreturn))
void double_fault(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: double fault\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: double_fault");
    for(;;);
 }__attribute__((noreturn))
void coprocessor_segment_overrun(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: coprocessor segment overrun\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: coprocessor_segment_overrun");
    for(;;);
 }__attribute__((noreturn))
void invalid_task_segment(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: invalid task segment\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: invalid_task_segment");
    for(;;);
 }__attribute__((noreturn))
void segment_not_present(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: segment not present\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: segment_not_present");
    for(;;);
 }__attribute__((noreturn))
void stack_segment_overrun(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: stack segment overrun\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: stack_segment_overrun");
    for(;;);
 }__attribute__((noreturn))
void general_protection_fault(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: general protection fault\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: general_protection_fault");
    for(;;);
 }__attribute__((noreturn))
void page_fault(interrupt_frame_t frame, uint32_t error_code) {
    // set_cursor(0,0);
    // set_background(0xFF0000);

    // uint32_t bad_address = 0;
    // __asm__ __volatile__ ("movl %%cr2, %0" : "=r"(bad_address));
    // printf("Error: page fault %d %d   %d\n",error_code,bad_address, *get_page(bad_address));
    // *get_page(bad_address) = PTE_READ_WRITE;

    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    // set_background(0xFF);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: page_fault");
    for(;;);
    frame.eip++;
 }__attribute__((noreturn))
void unassigned(interrupt_frame_t frame) {
    //  set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: unassigned");
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: disk read\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: unassigned");
    for(;;);
 }__attribute__((noreturn))
void coprocessor_error(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: coprocessor error\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: coprocessor_error");
    for(;;);
 }__attribute__((noreturn))
void alignment_check(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: alignment check\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: alignment_check");
    for(;;);
 }__attribute__((noreturn))
void machine_check(interrupt_frame_t frame) {
    // set_cursor(0,0);
    // set_background(0xFF0000);
    // print("Error: machine check\n");
    // printf("EIP %d CS %d ELFAGS %d ESP %d SS %d\n",frame.eip,frame.cs,frame.eflags,frame.sp,frame.ss);
    sc_set_foreground(0xffffffff);
    sc_set_background(0xffff0000);
    sc_print(0, 0, "Exception: machine_check");
    for(;;);
 }