#pragma once
#include "stddef.h"
#include "bootcode.h"

#define DISK_DATA_PORT          0x1F0
#define DISK_ERROR_PORT         0x1F1
#define DISK_FEATURES_PORT      0x1F1
#define DISK_SECTOR_COUNT_PORT  0x1F2
#define DISK_SECTOR_NUMBER_PORT 0x1F3
#define DISK_CYLINDER_LOW_PORT  0x1F4
#define DISK_CYLINDER_HIGH_PORT 0x1F5
#define DISK_DRIVE_REGISTER     0x1F6
#define DISK_STATUS_PORT        0x1F7
#define DISK_COMMAND_PORT       0x1F7
#define DISK_CONTROL_PORT       0x3F6

enum Disk_command {
    READ_SECTORS_WITH_RETRY = 0x20,
    READ_SECTORS = 0x21,
    READ_LONG_WITH_RETRY = 0x22,
    READ_LONG = 0x23,
    WRITE_SECTORS_WITH_RETRY = 0x30,
    WRITE_SECTORS = 0x31,
    WRITE_LONG_WITH_RETRY = 0x32,
    WRITE_LONG = 0x33,
};

typedef struct
{
    char error : 1;
    char index : 1;     // 0
    char corrected : 1; // 0
    char drive_request : 1;
    char service : 1;
    char drive_fault : 1;
    char ready : 1;
    char busy : 1;
} Disk_status_t;
typedef struct
{
    char addr_mark_not_found : 1;
    char track_zero_not_found : 1;
    char aborted_command : 1;
    char media_change_request : 1;
    char id_not_found : 1;
    char media_changed : 1;
    char uncorrectable_data : 1;
    char bad_block : 1;
} Disk_error_t;
typedef struct
{
    char head_block_num : 4;
    char drive_number : 1;
    char __resv0 : 1;
    char lba_addressing : 1;
    char __resv1 : 1;
} Disk_drive_info_t;

typedef struct __attribute__((packed))
{
    char OEMName[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t NumberOfRootEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t NumberOfHiddenSectors;
    uint16_t TotalSectors2;
    uint8_t DriveNumber;
    uint8_t _reserved;
    uint8_t ExtensionSigniture;
    uint32_t SerialNumber;
    char VolumeLabel[11];
    char FilesystemType[8];
} DiskFSInfo_t;

DiskFSInfo_t disk_get_info();
#define disk_get_status() (inb(DISK_STATUS_PORT))
#define disk_get_error() (inb(DISK_ERROR_PORT))
#define disk_get_drive_info() (inb(DISK_DRIVE_REGISTER))

void initialize_disk();

void disk_read_from(ptr_t location, addr_t linear_block_address, uint8_t number_of_sectors);
void disk_write_to (ptr_t location, addr_t linear_block_address, uint8_t number_of_sectors);