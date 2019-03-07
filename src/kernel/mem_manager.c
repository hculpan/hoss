#include "mem_manager.h"
#include "memory.h"

#include "../drivers/screen.h"
#include "../libc/string.h"

static struct Memory_Segment *memory_segments;
static int num_segments;

void get_largest_contiguous_block(struct Memory_Block *memory_block);
void add_memory_segment(struct Memory_Segment *memory_segment);
void split_segment(struct Memory_Segment *segment);
struct Memory_Segment *divide_segment(struct  Memory_Segment *segment, unsigned char pages);
struct Memory_Segment *find_first_free(unsigned char pages);
struct Memory_Segment *find_first_free_at_least(unsigned char pages);

void output(unsigned long num) {
    char buff[25];
    hex_to_ascii(num, buff);
    kprint(buff);
    kprint("\n");
}

void output_msg(unsigned long num, char *buff) {
    if (buff) {
        kprint(buff);
        kprint(": ");
    }
    output(num);
}

void init_memory_manager() {
    struct Memory_Block memory_block;
    get_largest_contiguous_block(&memory_block);

    memory_segments = (struct Memory_Segment *)memory_block.start_address;
    memory_segments[0].length = memory_block.length - (sizeof(struct Memory_Segment) * 2);
    memory_segments[0].type = TYPE_FREE;
    memory_segments[0].prev = 0;

    struct Memory_Segment *last_segment = (char *)&memory_segments[0] + sizeof(struct Memory_Segment) + memory_segments[0].length;
    last_segment->type = TYPE_EOM;
    last_segment->length = 0;
    last_segment->prev = &memory_segments[0];

    num_segments = sizeof(struct Memory_Segment) * 2;
}

struct Memory_Segment *get_segments() {
    return memory_segments;
}

unsigned int get_memory_of_type(const unsigned char type) {
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr->type != TYPE_EOM) {
        if (type == TYPE_ALL || curr->type == type) {
            result += curr->length;
        }
        curr = (char *)curr + curr->length + sizeof(struct Memory_Segment);
    }

    return result;
}

unsigned int get_free_memory() {
    return get_memory_of_type(TYPE_FREE);
}

unsigned int get_os_memory() {
    return get_memory_of_type(TYPE_OS) + num_segments * sizeof(struct Memory_Segment);
}

unsigned int get_user_memory() {
    return get_memory_of_type(TYPE_USER);
}

unsigned int get_total_memory() {
    return get_memory_of_type(TYPE_ALL) + num_segments * sizeof(struct Memory_Segment);
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
//    divide_segment(segment, segment->pages / 2);
}

struct Memory_Segment *divide_segment(struct  Memory_Segment *segment, unsigned char pages) {
/*    unsigned int pages2 = segment->pages - pages;
    segment->pages = pages;

    memory_segments[next_index].starting_address = segment->starting_address + (segment->pages * PAGE_SIZE);
    memory_segments[next_index].pages = pages2;
    memory_segments[next_index].type = segment->type;
    memory_segments[next_index].prev = segment;
    memory_segments[next_index].next = segment->next;

    segment->next = &memory_segments[next_index];

    num_segments++;
    next_index++;

    return segment;*/
    return 0;
}

struct Memory_Segment *find_first_free(unsigned char pages) {
/*    struct Memory_Segment *curr = &memory_segments[0];

    while (curr) {
        if (curr->type == TYPE_FREE && curr->pages == pages) {
            return curr;
        }
        curr = curr->next;
    }
*/
    return 0;
}

struct Memory_Segment *find_first_free_at_least(unsigned char pages) {
    struct Memory_Segment *curr = &memory_segments[0];
/*
    while (curr) {
        if (curr->type == TYPE_FREE && curr->pages >= pages) {
            return curr;
        }
        curr = curr->next;
    }
*/
    return 0;
}

void *allocate(unsigned char pages) {
    void *result = 0;
/*
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
*/
    return result;
}