#pragma once
#include "stddef.h"

#define VMM_PAGES_PER_TABLE 1024
#define VMM_TABLES_PER_DIRECTORY 1024
#define VMM_PAGE_SIZE 4096

#define VMM_PD_INDEX(vaddr) ((vaddr) >> 22)
#define VMM_PT_INDEX(vaddr) (((vaddr) >> 12) & 0x3FF)
#define VMM_PAGE_PHYSICAL_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFF)
#define VMM_SET_ATTRIBUTE(entry, attr) (*entry |= attr)
#define VMM_CLEAR_ATTRIBUTE(entry, attr) (*entry &= ~attr)
#define VMM_TEST_ATTRIBUTE(entry, attr) (*entry & attr)
#define VMM_SET_FRAME(entry, addr) (*entry = (*entry & ~0x7FFFF000) | addr)

// typedef struct __pt_entry {
//     char present : 1;
//     char read_write : 1;
//     char user : 1;
//     char write_through : 1;
//     char cache_disable : 1;
//     char __accessed : 1;
//     char dirty : 1;
//     char pat : 1;
//     char global : 1;
//     char reserved : 3;
//     addr_t frame : 19;
// } pt_entry_t;
// typedef struct __pd_entry {
//     char present : 1;
//     char read_write : 1;
//     char user : 1;
//     char write_through : 1;
//     char cache_disable : 1;
//     char __accessed : 1;
//     char dirty : 1;
//     char page_size : 1;
//     char global : 1;
//     char reserved : 3;
//     addr_t frame : 19;
// } pd_entry_t;

typedef uint32_t pt_entry_t __attribute__((aligned (4)));
typedef uint32_t pd_entry_t __attribute__((aligned (4)));

typedef enum {
	PTE_PRESENT			= 0x01,
	PTE_READ_WRITE		= 0x02,
	PTE_USER			= 0x04,
	PTE_WRITE_THROUGH	= 0x08,
	PTE_CACHE_DISABLE	= 0x10,
	PTE__ACCESSED		= 0x20,
	PTE_DIRTY			= 0x40,
	PTE_PAT				= 0x80,
	PTE_GLOBAL			= 0x100,
	PTE_FRAME			= 0x7FFFF000
} VMM_PAGE_TABLE_FLAGS;

typedef enum {
	PDE_PRESENT			= 0x01,
	PDE_READ_WRITE		= 0x02,
	PDE_USER			= 0x04,
	PDE_WRITE_THROUGH	= 0x08,
	PDE_CACHE_DISABLE	= 0x10,
	PDE__ACCESSED		= 0x20,
	PDE_DIRTY			= 0x40,		// 4MB entry only
	PDE_PAGE_SIZE		= 0x80,		// 0 = 4KB page, 1 = 4MB page
	PDE_GLOBAL			= 0x100,		// 4MB entry only
	PDE_PAT				= 0x2000,		// 4MB entry only
	PDE_FRAME			= 0x7FFFF000	// 
} VMM_PAGE_DIRECTORY_FLAGS;

typedef struct __attribute__((packed)) {
	pt_entry_t entries[VMM_PAGES_PER_TABLE];
} PageTable_t;

typedef struct __attribute__((packed)) {
	pd_entry_t entries[VMM_TABLES_PER_DIRECTORY];
} PageDirectory_t;

typedef struct _malloc_block {
	size_t size;
	bool_t free;
	struct _malloc_block *next;
} MallocBlock_t;

bool_t initialize_virtual_memory_manager(void);

pt_entry_t *vmm_get_pt_entry(PageTable_t *pt,     addr_t vaddr);
pd_entry_t *vmm_get_pd_entry(PageDirectory_t *pd, addr_t vaddr);
pt_entry_t *vmm_get_page    (addr_t addr);

ptr_t vmm_allocate_page(pt_entry_t *page);
void  vmm_free_page    (pt_entry_t *page);

bool_t vmm_set_page_directory(PageDirectory_t *pd);
void vmm_flush_table_entry (addr_t address);
bool_t vmm_map_page          (ptr_t physical_address, ptr_t virtual_address);
void vmm_unmap_page        (ptr_t vaddr);

void  vmm_malloc_init      (const size_t size);
void  vmm_malloc_split     (MallocBlock_t *node, const size_t size);
ptr_t vmm_malloc_next_block(const size_t size);
void  vmm_malloc_free      (ptr_t ptr);
void  vmm_malloc_defrag    (void);