#include "FAT16.h"
#include "drive.h"
#include "stddef.h"
#include "vbe_driver.h"

__attribute__((aligned(0x20)))
DirectoryEntry_t RootDirectory[512];
__attribute__((aligned(0x10)))
uint16_t         ClusterTable[0xC00];

void initialize_FATFS()
{
    DiskFSInfo_t FS_info = disk_get_info();
    ptr_t ct = (ptr_t)(0x7C00 + FS_info.ReservedSectors*FS_info.BytesPerSector);
    ptr_t rd = ((ptr_t)ct + FS_info.SectorsPerFAT*FS_info.BytesPerSector);
    memcpy(RootDirectory, rd, sizeof(RootDirectory));
    memcpy(ClusterTable, ct, sizeof(ClusterTable));
}
DirectoryEntry_t *find_entry(char name_ext[11])
{
    for(DirectoryEntry_t *entry = RootDirectory; entry->name[0] != 0; entry++)
    {
        if(memcmp(&entry->name[0], name_ext, 11) == 0) return entry;
    }
    return 0;
}
void read_all_to(ptr_t to, DirectoryEntry_t *entry)
{
    DiskFSInfo_t FS_info = disk_get_info();
    uint16_t first = entry->starting_cluster;
    uint16_t current = entry->starting_cluster;
    while(current < 0xFFF0)
    {
        uint16_t run = 1;
        while(current < 0xFFF0 && ClusterTable[current] == current + 1 && run < (256 / FS_info.SectorsPerCluster)) {
            run++;
            current = ClusterTable[current];
        }
        disk_read_from(to, first, run*FS_info.SectorsPerCluster);
        to += run*FS_info.SectorsPerCluster*FS_info.BytesPerSector;
        current = ClusterTable[current];
        first = current;
    }
}