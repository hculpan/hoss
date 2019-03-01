#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../shell/shell.h"
#include "../drivers/screen.h"

void kernel_main() {
    int *memSize;
    memSize = 0x7c02;

    clear_screen();
    kprint("Booting HOSS v.0.0.0.1\n\n");

    isr_install();
    irq_install();

    char buff[10];
    int_to_ascii(*memSize, buff);
    kprint("Memory result: ");
    kprint(buff);
    kprint("\n");

    start_shell();
}
