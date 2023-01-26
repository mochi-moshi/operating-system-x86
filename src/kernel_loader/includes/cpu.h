#pragma once
#include "stddef.h"

void cpu_initialize(void);
void cpu_shutdown(void);
void cpu_reset(void);
#define cpuid(code, a, d) \
    __asm__ __volatile__ ("cpuid" : "=a"((uint32_t)a), "=d"((uint32_t)d) : "0"((int)code) : "ebx", "ecx")
char* cpu_get_vender(void);