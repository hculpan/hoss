#ifndef CPU_TYPES_H
#define CPU_TYPES_H

#include "../libc/types.h"

#define low_16(address) (u16)((address)&0xffff);
#define high_16(address) (u16)(((address) >> 16) & 0xffff);

#endif