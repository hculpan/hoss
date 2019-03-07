#ifndef MEM_MANAGER_H
#define MEM_MANAGER_H

#define TYPE_FREE   0
#define TYPE_USER   1
#define TYPE_OS     2

#define PAGE_SIZE   4096

struct Memory_Segment {
    unsigned long starting_address;
    unsigned long pages;
    unsigned char type;
    struct Memory_Segment *next;
    struct Memory_Segment *prev;
};

void init_memory_manager();

struct Memory_Segment *get_segments();

unsigned int get_total_memory();
unsigned int get_free_memory();
unsigned int get_os_memory();
unsigned int get_user_memory();

void *allocate(unsigned char pages);

#endif