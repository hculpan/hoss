#include "memory.h"

#include "../libc/string.h"

int get_entry_count() {
    int *count = (int *)MMAP_ENTRY_COUNT;
    return *count;
}

void get_entry(int index, unsigned long *baseAddress, unsigned long *length, int *type) {
    void *ptr = (void *)(MEMORY_MAP_START + index * 24);
    *baseAddress = *(unsigned long *)(ptr);
    *length = *(unsigned long *)(ptr + 8);
    *type = *(int *)(ptr + 16);
}

void map_type(int type, char *buffer) {
    buffer[0] = '\0';
    switch (type) {
        case 1:
            strcat(buffer, "usable");
            break;
        case 2:
            strcat(buffer, "reserved");
            break;
        case 3:
            strcat(buffer, "ACPI reclaimable");
            break;
        case 4:
            strcat(buffer, "ACPI NVS memory");
            break;
        case 5:
            strcat(buffer, "Bad memory");
            break;
        default:
            strcat(buffer, "unknown");
            break;
    }
}

void totalAvailableMemory(struct Memory_Block *memory_block) {
    unsigned long largestBlockSize = 0;
    int count = get_entry_count();
    for (int i = 0; i < count; i++) {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (type == 1 && length > largestBlockSize) {
            memory_block->start_address = baseMemory;
            memory_block->length = length;
        }
    }
}

long totalUsableMemory() {
    int result = 0;
    int count = get_entry_count();
    for (int i = 0; i < count; i++) {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (type == 1 && baseMemory >= (1024 * 1024)) {
            result += length;
        }
    }

    return result;
}