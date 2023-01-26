#include "drive.h"
#include "stddef.h"
// #include "pit.h"

// static void ata_io_wait(void) {
// 	inb(DISK_CONTROL_PORT);
// 	inb(DISK_CONTROL_PORT);
// 	inb(DISK_CONTROL_PORT);
// 	inb(DISK_CONTROL_PORT);
// }

static DiskFSInfo_t FS_info;

void initialize_disk()
{
    BOOTCODE_t *bootcode = (BOOTCODE_t*)0x7C00;
    memcpy(&FS_info, &bootcode->OEMName[0], sizeof(DiskFSInfo_t));
}

DiskFSInfo_t disk_get_info() {
    return FS_info;
}

void disk_read_from(ptr_t location, addr_t linear_block_address, uint8_t number_of_sectors) {
    if(!number_of_sectors) return;
    while(disk_get_status() & 0b10000000);
    outb(DISK_DRIVE_REGISTER, 0b11100000 | ((linear_block_address >> 24) & 0xF)); // 1 LBA 1 master 
    outb(DISK_SECTOR_COUNT_PORT, number_of_sectors);
    outb(DISK_SECTOR_NUMBER_PORT, (linear_block_address >> 0) & 0xFF);
    outb(DISK_CYLINDER_LOW_PORT, (linear_block_address >> 8) & 0xFF);
    outb(DISK_CYLINDER_HIGH_PORT, (linear_block_address >> 16) & 0xFF);
    outb(DISK_COMMAND_PORT, READ_SECTORS_WITH_RETRY);

    // PIT_start_counter(100, PIT_OCW_COUNTER_0, PIT_OCW_MODE_SOFTWARETRIG);
    // int counter = 0;
    for(uint8_t i=0; i<number_of_sectors; i++) {
        // int counter = 0;
        uint8_t status;
        do {
            status = disk_get_status();
        } while(((status & 0b10000000) || !(status & 0b1000)) ); // && (counter < 100000)

        // if(counter >= 100000) do_interrupt(0x0f);
        insw(DISK_DATA_PORT, location + i*512, 256);
        // __asm__ __volatile__("rep insw" : : "D"(loc), "c"(256), "d"(DISK_DATA_PORT));
    }
}
void disk_write_to(ptr_t location, addr_t linear_block_address, uint8_t number_of_sectors) {
    if(!number_of_sectors) return;
    while(disk_get_status() & 0b10000000);
    outb(DISK_DRIVE_REGISTER, 0xE0 | (FS_info.DriveNumber & 0xF));
    outb(DISK_SECTOR_COUNT_PORT, number_of_sectors);
    outb(DISK_SECTOR_NUMBER_PORT, (linear_block_address >> 0) & 0xFF);
    outb(DISK_CYLINDER_LOW_PORT, (linear_block_address >> 8) & 0xFF);
    outb(DISK_CYLINDER_HIGH_PORT, (linear_block_address >> 16) & 0xFF);
    outb(DISK_COMMAND_PORT, WRITE_SECTORS_WITH_RETRY);
    for(uint8_t i=0; i<number_of_sectors; i++) {
        int counter = 0;
        uint8_t status;
        do {
            status = disk_get_status();
        } while(((status & 0b10000000) || !(status & 0b1000)) && (counter < 100000));

        if(counter >= 100000) do_interrupt(0x0f);

        outsl(DISK_DATA_PORT, location + i*512, 128);
    }
}