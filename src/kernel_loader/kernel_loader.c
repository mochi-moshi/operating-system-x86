#include "stddef.h"
#include "cpu.h"
#include "pic.h"
#include "ps2.h"
#include "vbe_driver.h"
#include "drive.h"
#include "bootcode.h"
#include "FAT16.h"
#include "ctype.h"
#include "physical_memory_manager.h"
#include "virtual_memory_manager.h"

extern ptr_t __bss_start, __bss_end;
__attribute__((section("kernel_entry")))
void entry(void) {
    cli();
    initialize_disk();
    initialize_FATFS();
    initialize_vbe(0xFFFFFFFF, 0xFF000000);
    initialize_pic(0x20,0x28);
    initialize_idt(0x8);
    initialize_ps2();
    
    sc_clear();
    const uint32_t header_foreground = 0xFFE2A302;
    const uint32_t line_foreground = 0xFFE2A302;
    const uint32_t text_backgound = 0xFF040505;
    sc_set_foreground(header_foreground);
    sc_set_background(text_backgound);
    sc_clear();
    DirectoryEntry_t *entry = find_entry("ap_2    bin");
    uint32_t *filedata = (uint32_t *)__bss_end;
    if(entry) {
        read_all_to(filedata, entry);
        uint32_t width = filedata[0];
        uint32_t height = filedata[1];
        sc_putimg_masked(6, 6, width, height, filedata+2, line_foreground, text_backgound);
    } else {
        sc_print(0, 1, "Could not find image file...");
    }
    sc_print(62, 22, "Hobby OS 0.2.0");

    sc_box(0, 0, sc_get_width()-1, sc_get_height()-1, 2, line_foreground);
    sc_fill(0, 60, sc_get_width()-1, 61, line_foreground);
    for(;;) {
        __asm__ __volatile__("hlt");
    }
    __builtin_unreachable();
}