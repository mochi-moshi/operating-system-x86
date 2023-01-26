#include "physical_memory_manager.h"

static byte*    pmm_memory_blocks 	   __attribute__((aligned (4)));
static uint64_t pmm_max_memory_blocks  __attribute__((aligned (4)));
static uint64_t pmm_used_memory_blocks __attribute__((aligned (4)));
static uint64_t pmm_total_memory 	   __attribute__((aligned (4)));

#define pmm_set_block(address) \
    (pmm_memory_blocks[((uint64_t)address)/PMM_BLOCKS_PER_BYTE] |= (0x80 >> (((uint64_t)address) % PMM_BLOCKS_PER_BYTE)))
#define pmm_unset_block(address) \
	(pmm_memory_blocks[((uint64_t)address)/PMM_BLOCKS_PER_BYTE] &= ~(0x80 >> (((uint64_t)address) % PMM_BLOCKS_PER_BYTE)))
#define pmm_test_block(address) \
	((bool)(pmm_memory_blocks[((uint64_t)address)/PMM_BLOCKS_PER_BYTE] & (0x80 >> (((uint64_t)address) % PMM_BLOCKS_PER_BYTE))))

void initialize_physical_memory_manager(const uint64_t start_address, const uint64_t size) {
	pmm_total_memory = size;
	pmm_memory_blocks = (byte*)start_address;
	pmm_max_memory_blocks = size / PMM_BLOCK_SIZE;
	pmm_used_memory_blocks = pmm_max_memory_blocks;
	memset(pmm_memory_blocks, 0xFF, pmm_max_memory_blocks/PMM_BLOCKS_PER_BYTE);
}

void pmm_intialize_memory_region(const uint64_t start_address, const uint64_t size) {
	uint64_t end = (start_address+size);
    if((start_address+size) % PMM_BLOCK_SIZE)
         { end = (end/PMM_BLOCK_SIZE) + 1; }
    else { end /= PMM_BLOCK_SIZE;          }

	for(uint64_t block_address = start_address / PMM_BLOCK_SIZE; block_address <= end; block_address++,pmm_used_memory_blocks--)
		pmm_unset_block(block_address);

	pmm_set_block(0);
}
void pmm_deintialize_memory_region(const uint64_t start_address, const uint64_t size) {
	uint64_t end = (start_address+size);
    if((start_address+size) % PMM_BLOCK_SIZE)
         { end = (end/PMM_BLOCK_SIZE) + 1; }
    else { end /= PMM_BLOCK_SIZE;          }
    
	for(uint64_t block_address = start_address / PMM_BLOCK_SIZE; block_address <= end; block_address++,pmm_used_memory_blocks++)
		pmm_set_block(block_address);
}
addr_t pmm_find_first_free(void) {
    if(pmm_used_memory_blocks == pmm_max_memory_blocks) return -1;
	for(size_t i=0; i<pmm_max_memory_blocks/PMM_BLOCKS_PER_BYTE; i++) {
        if(pmm_memory_blocks[i] == 0xFF) continue;
        for(uint8_t j = 0; j < 8; j++) {
            if(!(pmm_memory_blocks[i]&(0x80 >> j)))
                return i*PMM_BLOCKS_PER_BYTE + j;
        }
	}
	return 0;
}
addr_t pmm_find_first_free_run(const size_t size) {
	size_t run = size;
	for(size_t i=0; i<pmm_max_memory_blocks/PMM_BLOCKS_PER_BYTE; i++) {
		if(((run >= 8) && (pmm_memory_blocks[i] != 0)) || (pmm_memory_blocks[i] == 0xFF)) {
            run = size;
        } else {
            for(uint8_t j = 0; j < 8; j++) {
                if(pmm_memory_blocks[i]&(0x80 >> j))
                     { run = size; }
                else { run--;   }
                if(run == 0)
                    return i*PMM_BLOCKS_PER_BYTE + j - (size-1);
            }
        }
	}
	return 0;
}
ptr_t pmm_allocate_blocks(const size_t size) {
	// sc_print_rhex(0, 100, &pmm_used_memory_blocks, 8);
	// sc_print_rhex(0, 116, &pmm_max_memory_blocks, 8);
	if(pmm_max_memory_blocks-pmm_used_memory_blocks < size) return 0;

	addr_t start = pmm_find_first_free_run(size);
	if(!start) return 0;

	for(addr_t i=0; i<size; i++)
		pmm_set_block(start+i);

	pmm_used_memory_blocks += size;

	return (ptr_t)(start*PMM_BLOCK_SIZE);
}
void pmm_free_blocks(const ptr_t address, const size_t size) {
	for(addr_t i=0; i<size; i++)
		pmm_unset_block((uint64_t)address/PMM_BLOCK_SIZE+i);
	pmm_used_memory_blocks -= size;
}