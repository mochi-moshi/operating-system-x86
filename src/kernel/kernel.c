#include "vbe_driver.h"
#include "FAT16.h"
#include "ps2.h"
#include "ctype.h"
#include "drive.h"
#include "pic.h"
#include "idt.h"
#include "virtual_memory_manager.h"
#include "physical_memory_manager.h"


uint32_t index;

void write_to_screen(KeyEvent_t ke) {
	// sc_putch(0, 0, ke.ascii);
    if(ke.keypressed && isprint(ke.ascii)) {
        sc_putch((index % 160)*8 + 4, (index / 160)*16 + 64, ke.ascii);
        index++;
    } else if (ke.keypressed && ke.ascii == '\b') {
        index--;
        sc_putch((index % 160)*8 + 4, (index / 160)*16 + 64, ' ');
    } else if (ke.keypressed && ke.ascii == '\r') {
        index += index % 160;
    } else if (ke.keypressed && ke.ascii == '\n') {
        index += 160;
    }
}

const uint32_t header_foreground = 0xFFE2A302;
const uint32_t line_foreground = 0xFFE2A302;
const uint32_t text_backgound = 0xFF040505;

extern uint32_t __bss_start, __bss_end, MALLOC_START_ADDRESS;

__attribute__((section("entry")))
void _main(void) {
    // for(addr_t i = 0, k_start = 0xC0000000; i < ((addr_t)&__bss_end-0xC0000000)/VMM_PAGE_SIZE; i++) { // TODO: Fix page faults on run
    //     // addr_t block = (addr_t)pmm_allocate_blocks(1);
    //     pt_entry_t *page = vmm_get_page(k_start + i*VMM_PAGE_SIZE);
    //     if(!VMM_TEST_ATTRIBUTE(page, PTE_PRESENT)) {
    //         ptr_t paddr = vmm_allocate_page(page);
    //         if(!paddr) {
    //             // sc_print_hex(0, 48, &block, 4);
    //             // sc_print_hex(0, 64, &k_start, 4);
    //             // sc_print_hex(0, 200, pmm_memory_blocks, 100);
    //             sc_print(0, 32, "Failed to allocate memory!");
    //             for(;;) __asm__ __volatile__ ("hlt");
    //         }
    //         VMM_SET_ATTRIBUTE(page, PTE_READ_WRITE);
    //         if(!vmm_map_page(paddr, (ptr_t)(k_start + i*VMM_PAGE_SIZE))) {  
    //             sc_print(0, 32, "Error mapping in memory!");
    //             for(;;) __asm__ __volatile__ ("hlt");
    //         }
    //     }
    // }
    // initialize_disk();
    // initialize_FATFS();
    initialize_vbe(0xFFFFFFFF, 0xFF000000);
    // __initialize_pic(0x20,0x28);
    // initialize_idt(0x8);
    sc_print(0, 64, "in kernel!");
    // sc_set_foreground(header_foreground);
    // sc_set_background(text_backgound);
    // sc_clear();
    // DirectoryEntry_t *entry = find_entry("ap_2    bin");
    // uint32_t *filedata = (uint32_t *)MALLOC_START_ADDRESS;
    // if(entry) {
    //     read_all_to(filedata, entry);
    //     uint32_t width = filedata[0];
    //     uint32_t height = filedata[1];
    //     // sc_print_hex(17, 0, &width, 4);
    //     // sc_print_hex(40, 0, &height, 4);
    //     sc_putimg_masked(6, 6, width, height, filedata+2, line_foreground, text_backgound);
    // } else {
    //     sc_print(0, 1, "Could not find image file...");
    // }
    // sc_print(62, 22, "Shitty OS 0.3.0");

    // sc_box(0, 0, sc_get_width()-1, sc_get_height()-1, 2, line_foreground);
    // sc_fill(0, 60, sc_get_width()-1, 61, line_foreground);

    // index = 0;
    // register_keyevent_handler(write_to_screen);
    for(;;) {
        __asm__ __volatile__("hlt");
    }
    outw(0x604, 0x2000);
}