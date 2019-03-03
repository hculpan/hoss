#include "kernel.h"
#include "memory.h"

#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../shell/shell.h"
#include "../drivers/screen.h"

long availableMemory();

void kernel_main() {
    clear_screen();
    kprint("Booting HOSS v.0.0.0.1\n\n");

    isr_install();
    irq_install();

    start_shell();
}


