#include "mem.h"

void memcpy(void *dest, const void *source, int no_bytes) {
    char *destbuff = dest, *source_buff = source;
    int i;
    for (i = 0; i < no_bytes; i++) {
        *(destbuff + i) = *(source_buff + i);
    }
}

void mem_set(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

