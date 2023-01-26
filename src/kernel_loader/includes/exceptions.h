#pragma once
#include "stddef.h"
#include "idt.h"

typedef void(*ExceptionHandler_t)(void) __attribute__((noreturn));

ExceptionHandler_t get_default_exception_handler(size_t i);

void divide_by_zero_fault(interrupt_frame_t frame) __attribute__((noreturn));
void single_step(interrupt_frame_t frame);
void non_maskable_interrupt(interrupt_frame_t frame) __attribute__((noreturn));
void break_point(interrupt_frame_t frame) __attribute__((noreturn));
void overflow(interrupt_frame_t frame) __attribute__((noreturn));
void bounds_check(interrupt_frame_t frame) __attribute__((noreturn));
void undefined_opcode(interrupt_frame_t frame) __attribute__((noreturn));
void no_coprocessor(interrupt_frame_t frame) __attribute__((noreturn));
void double_fault(interrupt_frame_t frame) __attribute__((noreturn));
void coprocessor_segment_overrun(interrupt_frame_t frame) __attribute__((noreturn));
void invalid_task_segment(interrupt_frame_t frame) __attribute__((noreturn));
void segment_not_present(interrupt_frame_t frame) __attribute__((noreturn));
void stack_segment_overrun(interrupt_frame_t frame) __attribute__((noreturn));
void general_protection_fault(interrupt_frame_t frame) __attribute__((noreturn));
void page_fault(interrupt_frame_t frame, uint32_t error_code) __attribute__((noreturn));
void unassigned(interrupt_frame_t frame) __attribute__((noreturn));
void coprocessor_error(interrupt_frame_t frame) __attribute__((noreturn));
void alignment_check(interrupt_frame_t frame) __attribute__((noreturn));
void machine_check(interrupt_frame_t frame) __attribute__((noreturn));