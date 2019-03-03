#ifndef MEMORY_H
#define MEMORY_H

/*
*  Memory layout:
*       0x1000          Kernel (may overwrite boot sector)
*       0x7c00          Boot sector
*       0x9000          Stack
*       0xA000          Memrory map
*
*
*/

#define MMAP_ENTRY_COUNT 0xA000
#define MEMORY_MAP_START MMAP_ENTRY_COUNT + 4

struct Memory_Block {
    unsigned long start_address;
    unsigned long length;
};

int get_entry_count();

void get_entry(int index, unsigned long *baseAddress, unsigned long *length, int *type);

void map_type(int type, char *buffer);

void totalAvailableMemory(struct Memory_Block *memory_block);

long totalUsableMemory();

#endif