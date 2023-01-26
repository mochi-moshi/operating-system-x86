#pragma once
#include "stddef.h"

#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCKS_PER_BYTE 8

void   initialize_physical_memory_manager(const uint64_t start_address, const uint64_t size);
void   pmm_intialize_memory_region       (const uint64_t start_address, const uint64_t size);
void   pmm_deintialize_memory_region     (const uint64_t start_address, const uint64_t size);

addr_t pmm_find_first_free    (void);
addr_t pmm_find_first_free_run(const size_t size);
ptr_t  pmm_allocate_blocks    (const size_t size);
void   pmm_free_blocks        (const ptr_t address, const size_t size);