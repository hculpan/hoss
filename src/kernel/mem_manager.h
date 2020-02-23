#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#include "../cpu/types.h"

#define TYPE_ALL 0
#define TYPE_FREE 1
#define TYPE_USER 2
#define TYPE_OS 3
#define TYPE_EOM 4

#define PAGE_SIZE 1024

struct Memory_Segment
{
    size_t length;
    unsigned char type;
    struct Memory_Segment *prev;
} __attribute((packed)) __;

void init_memory_manager();

struct Memory_Segment *get_segments();

unsigned int get_total_memory();
unsigned int get_free_memory();
unsigned int get_os_memory();
unsigned int get_user_memory();

void *allocate(const size_t size);
void free(const void *ptr);

#endif