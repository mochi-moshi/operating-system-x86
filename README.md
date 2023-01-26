# Operating System Specification

## FAT16 File Structure

### FAT Entry

    0x00:    Filename - 8 characters + 3 char ext
        Start with 0 = free, 0x2e = '.' entry, 0xe5 = deleted not erased
    0x0b:    Attributes - read only, hidden, sysfile, volume label, subdirectory, archive, unused x 2
    0x0c:    read need pass, write need pass, delete need pass, reserved x 5
    0x0d:    First character of deleted file
    0x0e-11: Reserved
    0x12-13: Owner ID
    0x14-15: Access Rights
        owner delete/rename/attr change, owner execute, owner write, owner read/copy, <- group, <- world, reserved x4
    0x16-17: Last Modified Time (5/6/5 bits, for hour/minutes/doubleseconds)
    0x18-19: Last Modified Date (7/4/5 bits, for year-since-1980/month/day)
    0x1a-1b: Starting cluster (0 for an empty file)
    0x1c-1f: Filesize in bytes

### VFAT Long Name

    0x00-01: Sequence Number (bit 6: last logical, first physical LFN entry, bit 5: 0; bits 4-0: number 0x01..0x14 (0x1F), deleted entry: 0xE5)
    0x01-0a: Name characters (five UCS-2 characters)
    0x0b:    always 0x0F
    0x0c:    always 0x00
    0x0d:    Checksum of DOS file name
    0x0e-19: Name characters (six UCS-2 characters)
    0x1a-1b: First cluster (always 0x0000)
    0x1c-1f: Name characters (two UCS-2 characters)

### Checksum


    unsigned char lfn_checksum(const unsigned char *pFCBName)
    {
    int i;
    unsigned char sum = 0;

    for (i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;

    return sum;
    }

### Clusters

    0x0000: Free Cluster
    0x0002-FFEF: Used Cluster
    0xFFF0-FFF6: Reserved
    0xFFF7: Bad Cluster
    0xFFF8-FFFF: End-of-file

## Image Structure

    Reserved: Bootloader, Kernel Loader
    FAT - 12 sectors
    Root Directory - 32 sectors
    Data

## TODO

- [x] setup VBE
- [ ] virtual memory
- [ ] load final kernel to higher memory
