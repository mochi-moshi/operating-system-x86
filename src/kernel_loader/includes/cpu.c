#include "cpu.h"
void initialize_cpu(void) {
}

void cpu_shutdown(void) {
    outw(0x604, 0x2000); // qemu specific
}

void cpu_reset(void) {
    // ps2_controller_command(PS2_Controller_reset);
}

char* cpu_get_vender(void) {
    static char vender[32] = {0};
    __asm__ __volatile__ ("cpuid" : "=b"(vender[0]), "=d"(vender[4]), "=c"(vender[8]) : "a"(0));
    return vender;
}