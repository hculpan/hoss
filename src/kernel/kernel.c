#include "kernel.h"
#include "mem_manager.h"

#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../shell/shell.h"

long availableMemory();

void kernel_main() {
    clear_screen();
    kprint("Booting HOSS v.0.0.0.1\n\n");

    isr_install();
    irq_install();

    init_memory_manager();

    start_shell();
}


