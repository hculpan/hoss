#include "mem_manager.h"
#include "memory.h"

static unsigned int pages;
static struct Memory_Segment *memory_segments;
static unsigned int num_segments;
static unsigned int next_index;
static unsigned int max_segments;

void get_largest_contiguous_block(struct Memory_Block *memory_block);
void add_memory_segment(struct Memory_Segment *memory_segment);
void split_segment(struct Memory_Segment *segment);
struct Memory_Segment *divide_segment(struct  Memory_Segment *segment, unsigned char pages);
struct Memory_Segment *find_first_free(unsigned char pages);
struct Memory_Segment *find_first_free_at_least(unsigned char pages);

void init_memory_manager() {
    struct Memory_Block memory_block;
    get_largest_contiguous_block(&memory_block);
    memory_segments = (void *)(memory_block.start_address);
    pages = memory_block.length / PAGE_SIZE;
    unsigned int pages_seg_block = pages * sizeof(struct Memory_Segment);
    max_segments = (pages_seg_block / PAGE_SIZE) + 1;
    
    memory_segments[0].starting_address = (unsigned long)memory_segments;
    memory_segments[0].pages = max_segments;
    memory_segments[0].type = TYPE_OS;
    memory_segments[0].prev = 0;
    memory_segments[0].next = 0;

    memory_segments[1].starting_address = memory_segments[0].starting_address + (memory_segments[0].pages * PAGE_SIZE);
    memory_segments[1].pages = pages - memory_segments[0].pages;
    memory_segments[1].type = TYPE_FREE;
    memory_segments[1].prev = &memory_segments[0];
    memory_segments[1].next = 0;

    memory_segments[0].next = &memory_segments[1];

    num_segments = 2;
    next_index = 2;
}

struct Memory_Segment *get_segments() {
    return memory_segments;
}

unsigned int get_free_memory() {
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr) {
        if (curr->type == TYPE_FREE) {
            result += curr->pages;
        }
        curr = curr->next;
    }

    return result * PAGE_SIZE;
}

unsigned int get_os_memory() {
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr) {
        if (curr->type == TYPE_OS) {
            result += curr->pages;
        }
        curr = curr->next;
    }

    return result * PAGE_SIZE;
}

unsigned int get_user_memory() {
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr) {
        if (curr->type == TYPE_USER) {
            result += curr->pages;
        }
        curr = curr->next;
    }

    return result * PAGE_SIZE;
}

unsigned int get_total_memory() {
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr) {
        result += curr->pages;
        curr = curr->next;
    }

    return result * PAGE_SIZE;
}

void get_largest_contiguous_block(struct Memory_Block *memory_block) {
    int count = get_entry_count();
    for (int i = 0; i < count; i++) {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (type != 1) continue;

        if (length > memory_block->length) {
            memory_block->start_address = baseMemory;
            memory_block->length = length;
        }
    }
}

void split_segment(struct Memory_Segment *segment) {
    divide_segment(segment, segment->pages / 2);
}

struct Memory_Segment *divide_segment(struct  Memory_Segment *segment, unsigned char pages) {
    unsigned int pages2 = segment->pages - pages;
    segment->pages = pages;

    memory_segments[next_index].starting_address = segment->starting_address + (segment->pages * PAGE_SIZE);
    memory_segments[next_index].pages = pages2;
    memory_segments[next_index].type = segment->type;
    memory_segments[next_index].prev = segment;
    memory_segments[next_index].next = segment->next;

    segment->next = &memory_segments[next_index];

    num_segments++;
    next_index++;

    return segment;
}

struct Memory_Segment *find_first_free(unsigned char pages) {
    struct Memory_Segment *curr = &memory_segments[0];

    while (curr) {
        if (curr->type == TYPE_FREE && curr->pages == pages) {
            return curr;
        }
        curr = curr->next;
    }

    return 0;
}

struct Memory_Segment *find_first_free_at_least(unsigned char pages) {
    struct Memory_Segment *curr = &memory_segments[0];

    while (curr) {
        if (curr->type == TYPE_FREE && curr->pages >= pages) {
            return curr;
        }
        curr = curr->next;
    }

    return 0;
}

void *allocate(unsigned char pages) {
    void *result = 0;

    struct Memory_Segment *test = find_first_free(pages);
    if (!test) {
        test = find_first_free_at_least(pages);
        if (test) {
            test = divide_segment(test, pages);
            test->type = TYPE_USER;
            result = (void *)test->starting_address;
        }
    } else {
        result = (void *)test->starting_address;
        test->type = TYPE_USER;
    }

    return result;
}