#include "mem_manager.h"
#include "memory.h"

#include "../drivers/screen.h"
#include "../libc/string.h"

static struct Memory_Segment *memory_segments;
static int num_segments;

void get_largest_contiguous_block(struct Memory_Block *memory_block);
struct Memory_Segment *divide_segment(struct Memory_Segment *segment, const unsigned int size, const unsigned char type);
struct Memory_Segment *find_first_free(const unsigned int size);

void output(unsigned long num)
{
    char buff[25];
    hex_to_ascii(num, buff);
    kprint(buff);
    kprint("\n");
}

void output_msg(unsigned long num, void *buff)
{
    if (buff)
    {
        kprint(buff);
        kprint(": ");
    }
    output(num);
}

void init_memory_manager()
{
    struct Memory_Block memory_block;
    get_largest_contiguous_block(&memory_block);

    memory_segments = (struct Memory_Segment *)memory_block.start_address;
    memory_segments[0].length = memory_block.length - (sizeof(struct Memory_Segment) * 2);
    memory_segments[0].type = TYPE_FREE;
    memory_segments[0].prev = 0;

    struct Memory_Segment *last_segment = (void *)memory_block.start_address + memory_block.length - sizeof(struct Memory_Segment);
    last_segment->type = TYPE_EOM;
    last_segment->length = 0;

    last_segment->prev = &memory_segments[0];

    num_segments = 2;
}

struct Memory_Segment *get_segments()
{
    return memory_segments;
}

unsigned int get_memory_of_type(const unsigned char type)
{
    struct Memory_Segment *curr = &memory_segments[0];
    unsigned int result = 0;

    while (curr->type != TYPE_EOM)
    {
        if (type == TYPE_ALL || curr->type == type)
        {
            result += curr->length;
        }
        curr = (void *)curr + curr->length + sizeof(struct Memory_Segment);
    }

    return result;
}

unsigned int get_free_memory()
{
    return get_memory_of_type(TYPE_FREE);
}

unsigned int get_os_memory()
{
    return get_memory_of_type(TYPE_OS) + num_segments * sizeof(struct Memory_Segment);
}

unsigned int get_user_memory()
{
    return get_memory_of_type(TYPE_USER);
}

unsigned int get_total_memory()
{
    return get_memory_of_type(TYPE_ALL) + num_segments * sizeof(struct Memory_Segment);
}

void get_largest_contiguous_block(struct Memory_Block *memory_block)
{
    int count = get_entry_count();
    for (int i = 0; i < count; i++)
    {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (type != 1)
            continue;

        if (length > memory_block->length)
        {
            memory_block->start_address = baseMemory;
            memory_block->length = length;
        }
    }
}

struct Memory_Segment *divide_segment(struct Memory_Segment *segment, const size_t size, const unsigned char type)
{
    if (segment->length == size)
    {
        segment->type = type;
    }
    else
    {
        struct Memory_Segment *new_segment = (void *)segment + size + sizeof(struct Memory_Segment);

        new_segment->type = segment->type;
        new_segment->prev = segment;
        new_segment->length = segment->length - (size + sizeof(struct Memory_Segment));

        segment->length = size;
        segment->type = type;

        num_segments++;
    }

    return segment;
}

struct Memory_Segment *find_first_free(const size_t size)
{
    struct Memory_Segment *result = 0;
    struct Memory_Segment *curr = &memory_segments[0];

    while (curr->type != TYPE_EOM)
    {
        if (curr->type == TYPE_FREE && curr->length >= size)
        {
            result = curr;
            break;
        }
        curr = (void *)curr + curr->length + sizeof(struct Memory_Segment);
    }

    return result;
}

void *allocate(const size_t size)
{
    void *result = 0;
    struct Memory_Segment *seg = find_first_free(size);
    if (seg)
    {
        seg = divide_segment(seg, size, TYPE_USER);
        result = (void *)seg + sizeof(struct Memory_Segment);
    }
    return result;
}

void free(const void *ptr)
{
    struct Memory_Segment *seg = (void *)ptr - sizeof(struct Memory_Segment);
    seg->type = TYPE_FREE;

    // Now let's see if neighboring segments are free
    struct Memory_Segment *next_seg = (void *)seg + seg->length + sizeof(struct Memory_Segment);
    if (next_seg->type == TYPE_FREE)
    {
        struct Memory_Segment *next2_seg = (void *)next_seg + next_seg->length + sizeof(struct Memory_Segment);
        next2_seg->prev = seg;
        seg->length = seg->length + sizeof(struct Memory_Segment) + next_seg->length;
        num_segments--;
    }

    if (seg->prev)
    {
        struct Memory_Segment *prev_seg = seg->prev;
        if (prev_seg->type == TYPE_FREE)
        {
            next_seg = (void *)seg + seg->length + sizeof(struct Memory_Segment);
            next_seg->prev = prev_seg;
            prev_seg->length = seg->length + sizeof(struct Memory_Segment) + prev_seg->length;
            num_segments--;
        }
    }
}