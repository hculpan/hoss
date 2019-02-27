#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/keyboard.h"

void kernel_main() {
    clear_screen();
    kprint("Booting HOSS v.0.0.0.1\n\n");

    isr_install();
    irq_install();

    kprint("Type something, it will go through the kernel\n"
        "Type end to halt the CPU or PAGE to request a kmalloc()\n> ");

    char buff[256];
    char str[2];
    str[1] = '\0';
    while (1) {
        str[0] = waitForAscii();
        if (str[0] == '\r') {
            kprint(str);
            kprint("-> ");
            kprint(buff);
            buff[0] = '\0';
            kprint("\n> ");
            if (strcmp(buff, "end") == 0) {
                kprint("\nStopping the CPU. Bye!\n");
                asm volatile("hlt");
            }
        } else if (str[0] == '\b') {
            int len = strlen(buff);
            if (len > 0) {
                kprint_backspace();
                buff[len - 1] = '\0';
            }
        } else {
            kprint(str);
            append(buff, str[0]);
        }
    }
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "PAGE") == 0) {
        /* Lesson 22: Code to test kmalloc, the rest is unchanged */
        u32 phys_addr;
        u32 page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}
