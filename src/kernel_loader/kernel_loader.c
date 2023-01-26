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

    BOOTCODE_t *bootcode = (BOOTCODE_t *)0x7C00;
    SMAP_t *smap = (SMAP_t *)bootcode->SMAPEntries;
    size_t memory_size = smap->Entries[smap->NumberOfEntries - 1].BaseAddress + smap->Entries[smap->NumberOfEntries - 1].Size - 1;
    addr_t ppm_location = (addr_t)&__bss_end + ((addr_t)&__bss_end % 4);
    initialize_physical_memory_manager(ppm_location, memory_size);
    for(int i=0; i < smap->NumberOfEntries; i++) {
        if(smap->Entries[i].Type == 1)
            pmm_intialize_memory_region(smap->Entries[i].BaseAddress, smap->Entries[i].Size);
    }
    pmm_deintialize_memory_region(0x10000, ppm_location-0x10000);
    pmm_deintialize_memory_region(ppm_location, memory_size / PMM_BLOCK_SIZE);

    initialize_virtual_memory_manager();

    MODEINFOBLOCK_t modeinfo = sc_get_modeinfoblock();

    size_t size_in_bytes = modeinfo.XResolution * modeinfo.YResolution * 4;
    size_t size_in_pages = size_in_bytes / VMM_PAGE_SIZE;
    if(size_in_bytes % VMM_PAGE_SIZE > 0) size_in_pages++;

    size_in_pages *= 2; // safety

    for(addr_t i = 0, fb_start = modeinfo.PhysicalBasePointer; i < size_in_pages; i++, fb_start += VMM_PAGE_SIZE)
        vmm_map_page((ptr_t)fb_start,(ptr_t)fb_start);

    pmm_deintialize_memory_region(modeinfo.PhysicalBasePointer, size_in_pages * PMM_BLOCK_SIZE);
    // sc_print_hex(0, 0, &pmm_max_memory_blocks, 4);
    DirectoryEntry_t *entry = find_entry("kernel  bin");
    if(entry) {
        sc_print(0,0, "kernel  bin found...");
        size_in_pages = entry->size / VMM_PAGE_SIZE;
        if(entry->size % VMM_PAGE_SIZE > 0) size_in_pages++;
        sc_print(0,16, "mapping virtual memory pages...");
        for(addr_t i = 0, k_start = 0xC0000000; i < size_in_pages; i++) {
            // addr_t block = (addr_t)pmm_allocate_blocks(1);
            pt_entry_t *page = vmm_get_page(k_start + i*VMM_PAGE_SIZE);
            ptr_t paddr = vmm_allocate_page(page);
            if(!paddr) {
                // sc_print_hex(0, 48, &block, 4);
                // sc_print_hex(0, 64, &k_start, 4);
                // sc_print_hex(0, 200, pmm_memory_blocks, 100);
                sc_print(0, 32, "Failed to allocate memory!");
                for(;;) __asm__ __volatile__ ("hlt");
            }
            VMM_SET_ATTRIBUTE(page, PTE_READ_WRITE);
            if(!vmm_map_page(paddr, (ptr_t)(k_start + i*VMM_PAGE_SIZE))) {  
                sc_print(0, 32, "Error mapping in memory!");
                for(;;) __asm__ __volatile__ ("hlt");
            }
        }
        sc_print(0,32, "loading kernel code...");
        read_all_to((ptr_t)0xC0000000, entry); // FIX: page fault when trying to write
        sc_print(0,48, "jumping to kernel code...");
        sc_print_hex(0,64, (ptr_t)0xC0000000, 128);
        uint32_t *kernel_start = (uint32_t*)0xC0000000;
        uint32_t *bss_end = kernel_start+1;
        for(addr_t k_start = 0xC0000000+size_in_pages*VMM_PAGE_SIZE; k_start < *bss_end; k_start += VMM_PAGE_SIZE) {
            // addr_t block = (addr_t)pmm_allocate_blocks(1);
            pt_entry_t *page = vmm_get_page(k_start);
            ptr_t paddr = vmm_allocate_page(page);
            if(!paddr) {
                // sc_print_hex(0, 48, &block, 4);
                // sc_print_hex(0, 64, &k_start, 4);
                // sc_print_hex(0, 200, pmm_memory_blocks, 100);
                sc_print(0, 32, "Failed to allocate memory!");
                for(;;) __asm__ __volatile__ ("hlt");
            }
            VMM_SET_ATTRIBUTE(page, PTE_READ_WRITE);
            if(!vmm_map_page(paddr, (ptr_t)(k_start))) {  
                sc_print(0, 32, "Error mapping in memory!");
                for(;;) __asm__ __volatile__ ("hlt");
            }
        }
        // for(;;) __asm__ __volatile__ ("hlt");
        // void (*kernel)(void) = (void (*)());
        // kernel();
        // ptr_t kernel = (ptr_t)0xC0000000;
        // for(;;) __asm__ __volatile__ ("hlt");

        __asm__ __volatile__ (
           "jmp *%0"
           : : "r"(*kernel_start));
        __builtin_unreachable();
    } else {
        sc_print(0,0, "Could not find kernel file!!!!!!");
        for(;;) __asm__ __volatile__ ("hlt");
    }
}