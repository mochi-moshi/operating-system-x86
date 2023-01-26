#pragma once
#include "stddef.h"
#include "bootcode.h"

typedef struct __attribute__((packed))
{
    char name[8];
    char ext[3];
    uint8_t attributes;
    uint8_t sudo_rights;
    char first_char_deleted;
    uint8_t _reserved[4];
    uint16_t owner_id;
    uint16_t access_rights;
    uint16_t last_modified_time;
    uint16_t last_modified_date;
    uint16_t starting_cluster;
    uint32_t size;
} DirectoryEntry_t;


void initialize_FATFS();
DirectoryEntry_t *find_entry(char *name_ext);
void read_all_to(ptr_t to, DirectoryEntry_t *entry);