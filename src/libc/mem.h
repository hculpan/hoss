#ifndef MEM_H
#define MEM_H

#include "types.h"

void memcpy(void *dest, const void *source, int no_bytes);

void mem_set(u8 *dest, u8 val, u32 len);

u32 kmalloc(u32 size, int align, u32 *phys_addr);

#endif