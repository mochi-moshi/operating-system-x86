#include "virtual_memory_manager.h"
#include "physical_memory_manager.h"


static PageDirectory_t *vmm_currentDirectory;

pt_entry_t *vmm_get_pt_entry(PageTable_t *pt, addr_t vaddr) {
	return (pt)?&pt->entries[VMM_PT_INDEX(vaddr)]:0;
}
pd_entry_t *vmm_get_pd_entry(PageDirectory_t *pd, addr_t vaddr) {
	return (pd)?&pd->entries[VMM_PD_INDEX(vaddr)]:0;
}
pt_entry_t *vmm_get_page(addr_t vaddr) {
	pd_entry_t *entry = &vmm_currentDirectory->entries[VMM_PD_INDEX(vaddr)];
	PageTable_t *table = (PageTable_t *) VMM_PAGE_PHYSICAL_ADDRESS(entry);
	return &table->entries[VMM_PT_INDEX(vaddr)];
}
ptr_t vmm_allocate_page(pt_entry_t *page) {
	ptr_t block = pmm_allocate_blocks(1);
	if(block) {
		VMM_SET_FRAME(page, (addr_t)block);
		VMM_SET_ATTRIBUTE(page, PTE_PRESENT);
	}
	return block;
}
void vmm_free_page(pt_entry_t *page) {
	ptr_t address = (ptr_t)VMM_PAGE_PHYSICAL_ADDRESS(page);
	if(address) pmm_free_blocks(address, 1);
	VMM_CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}
bool_t vmm_set_page_directory(PageDirectory_t *pd) {
	if(!pd) return false;
	vmm_currentDirectory = pd;
	__asm__ __volatile__ ("movl %%eax, %%cr3" : : "a"(vmm_currentDirectory));
	return true;
}
void vmm_flush_table_entry(addr_t address) {
	__asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r"(address));
}
bool_t vmm_map_page(ptr_t physical_address, ptr_t virtual_address) {
	pd_entry_t *entry = &vmm_currentDirectory->entries[VMM_PD_INDEX((addr_t)virtual_address)];

	if(!VMM_TEST_ATTRIBUTE(entry, PDE_PRESENT)) {
		PageTable_t *table = (PageTable_t *)pmm_allocate_blocks(1);
		if(!table) return false;
		memset(table, 0, sizeof(PageTable_t));
		VMM_SET_ATTRIBUTE(entry, PDE_PRESENT);
		VMM_SET_ATTRIBUTE(entry, PDE_READ_WRITE);
		VMM_SET_FRAME(entry, (addr_t)table);
	}

	PageTable_t *table = (PageTable_t *)VMM_PAGE_PHYSICAL_ADDRESS(entry);
	pt_entry_t *page = &table->entries[VMM_PT_INDEX((addr_t)virtual_address)];
	VMM_SET_ATTRIBUTE(page, PTE_PRESENT);
	VMM_SET_FRAME(page, (addr_t)physical_address);
	return true;
}
void vmm_unmap_page(ptr_t vaddr) {
	pt_entry_t *page = vmm_get_page((addr_t)vaddr);
	VMM_SET_FRAME(page, 0);
	VMM_CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}
bool_t initialize_virtual_memory_manager(void) {
	PageDirectory_t *dir = (PageDirectory_t *)pmm_allocate_blocks(3);

	if(!dir) return false;

	memset(dir, 0, sizeof(PageDirectory_t));
	for(uint32_t i = 0; i < VMM_TABLES_PER_DIRECTORY; i++)
		dir->entries[i] = PDE_READ_WRITE; // supervisor, read/write, not present

	PageTable_t *table = (PageTable_t *)pmm_allocate_blocks(1);
	if(!table) return false;
	PageTable_t *table3GB = (PageTable_t *)pmm_allocate_blocks(1);
	if(!table3GB) return false;
	memset(table,    0, sizeof(PageTable_t));
	memset(table3GB, 0, sizeof(PageTable_t));

	for(addr_t i = 0, frame = 0, vaddr = 0; i < VMM_PAGES_PER_TABLE; i++, frame += VMM_PAGE_SIZE, vaddr += VMM_PAGE_SIZE) {
		pt_entry_t page = 0;
		VMM_SET_ATTRIBUTE(&page, PTE_PRESENT);
		VMM_SET_ATTRIBUTE(&page, PTE_READ_WRITE);
		VMM_SET_FRAME(&page, frame);

		table3GB->entries[VMM_PT_INDEX(vaddr)] = page;
	}

	// for(addr_t i = 0, frame = 0x10000, vaddr = 0xC0000000; i < VMM_PAGES_PER_TABLE; i++, frame += VMM_PAGE_SIZE, vaddr += VMM_PAGE_SIZE) {
	// 	pt_entry_t page = 0;
	// 	VMM_SET_ATTRIBUTE(&page, PTE_PRESENT);
	// 	VMM_SET_ATTRIBUTE(&page, PTE_READ_WRITE);
	// 	VMM_SET_FRAME(&page, frame);

	// 	table->entries[VMM_PT_INDEX(vaddr)] = page;
	// }

	// pd_entry_t *entry = &dir->entries[VMM_PD_INDEX(0xC0000000)];
	// VMM_SET_ATTRIBUTE(entry, PDE_PRESENT);
	// VMM_SET_ATTRIBUTE(entry, PDE_READ_WRITE);
	// VMM_SET_FRAME(entry, (addr_t)table);

	pd_entry_t *entry = &dir->entries[VMM_PD_INDEX(0x00000000)];
	VMM_SET_ATTRIBUTE(entry, PDE_PRESENT);
	VMM_SET_ATTRIBUTE(entry, PDE_READ_WRITE);
	VMM_SET_FRAME(entry, (addr_t)table3GB);

	vmm_set_page_directory(dir);

	__asm__ __volatile__ (
		"movl %cr0, %eax\n\t"
		"orl $0x80000001, %eax\n\t"
		"movl %eax, %cr0");
	return true;
}

MallocBlock_t *malloc_head  = 0;
ptr_t malloc_vaddr          = 0;
ptr_t malloc_paddr          = 0;
size_t   total_malloc_pages = 0;

void vmm_malloc_init(const size_t size) {
	malloc_vaddr = (ptr_t)0xF00000;
	total_malloc_pages = size/VMM_PAGE_SIZE;
	if(size % VMM_PAGE_SIZE) total_malloc_pages++;
	for(size_t i = 0; i < total_malloc_pages; i++) {
		pt_entry_t *page = vmm_get_page((addr_t)malloc_vaddr + i*VMM_PAGE_SIZE);
		ptr_t paddr = vmm_allocate_page(page);
		VMM_SET_ATTRIBUTE(page, PTE_READ_WRITE);
		vmm_map_page(paddr, malloc_vaddr + i*VMM_PAGE_SIZE);
	}
	malloc_head =       (MallocBlock_t*)malloc_vaddr;
	malloc_head->size = (total_malloc_pages*VMM_PAGE_SIZE)-sizeof(MallocBlock_t);
	malloc_head->free = true;
	malloc_head->next = 0;
}
void vmm_malloc_split(MallocBlock_t *before, const size_t size) {
	MallocBlock_t *after = (MallocBlock_t*)((ptr_t)(before+1) + size);
	after->size = before->size - size - sizeof(MallocBlock_t);
	after->free = true;
	after->next = before->next;

	before->size = size;
	before->free = false;
	before->next = after;
}
ptr_t vmm_malloc_next_block(const size_t size) {
	if(size == 0)          return 0;
	if(!malloc_head->size) vmm_malloc_init(size);
	MallocBlock_t *cur = malloc_head;
	for(; ((cur->size < size) || !cur->free) && cur->next; cur = cur->next);
	if(cur->size == size) {
		cur->free = false;
	} else if(cur->size > size + sizeof(MallocBlock_t)) {
		vmm_malloc_split(cur, size);
	} else { // should be last element since it couldnt find a big enough block
		size_t additional_pages = 1;
		if(cur->free) {
			for(;cur->size + additional_pages*VMM_PAGE_SIZE < size; additional_pages++);
		} else {
			for(;cur->size + additional_pages*VMM_PAGE_SIZE < size + sizeof(MallocBlock_t); additional_pages++);
		}
		ptr_t end = malloc_vaddr + total_malloc_pages*VMM_PAGE_SIZE;
		for(uint8_t i = 0; i < additional_pages; i++) {
			pt_entry_t *page = vmm_get_page((addr_t)end + i*VMM_PAGE_SIZE);
			ptr_t paddr = vmm_allocate_page(page);
			vmm_map_page(paddr, end + i*VMM_PAGE_SIZE);
			VMM_SET_ATTRIBUTE(page, PTE_READ_WRITE);
		}
		total_malloc_pages += additional_pages;
		if(cur->free) {
			cur->size += additional_pages*VMM_PAGE_SIZE;
			vmm_malloc_split(cur, size);
		} else {
			MallocBlock_t *new = (MallocBlock_t*)end;
			new->size = additional_pages*VMM_PAGE_SIZE;
			new->free = true;
			new->next = 0;
			vmm_malloc_split(new, size);
			cur = new;
		}
	}
	return (ptr_t)(cur+1);
}
void vmm_malloc_defrag(void) {
	if(!malloc_head) return;
	for(MallocBlock_t *cur = malloc_head; cur->next; cur = cur->next) {
		if(cur->free) {
			while(cur->next->free) {
				cur->size += cur->next->size + sizeof(MallocBlock_t);
				cur->next =  cur->next->next;
				if(!cur->next) break;
			}
		}
	}
}
void vmm_malloc_free(ptr_t ptr) {
	for(MallocBlock_t *cur = malloc_head, *prev = (MallocBlock_t *)0; cur->next; prev = cur, cur = cur->next) {
		if((ptr_t)(cur+1) == ptr) {
			cur->free = true;
			if(prev) {
				if(prev -> free) {
					prev->size += cur->size+sizeof(MallocBlock_t);
					prev->next =  cur->next;
					cur = prev;
				}
			}
			if(cur->next) {
				while(cur->next->free) {
					cur->size += cur->next->size + sizeof(MallocBlock_t);
					cur->next =  cur->next->next;
					if(!cur->next) break;
				}
			}
		}
	}
}