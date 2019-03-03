#include "shell.h"

#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../kernel/memory.h"

static char displayScancode = 0;

void process_command(char *buff);
void displayUsableMemory();
void displayFullMemory();
void displayMemoryMap(int dtype);
void displayMemory();

void start_shell() {
    kprint("Type something!\n"); 
    kprint("> ");

    char buff[256];
    char str[2];
    str[1] = '\0';
    while (1) {
        if (displayScancode) {
            unsigned char code = waitForScancode();
            int_to_ascii(code, buff);
            kprint("{");
            kprint(buff);
            kprint("}\n");
        } else {
            str[0] = waitForAscii();
            if (str[0] == '\r') {
                kprint(str);
                trim_trailing(buff);
                process_command(buff);
                kprint("\n> ");
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

}

void process_command(char *buff) {
    if (strcmp(buff, "scancode") == 0) {
        kprint("\nSwitching to scancode mode\n");
        displayScancode = 1;
    } else if (strcmp(buff, "memmap") == 0) {
        displayUsableMemory();
    } else if (strcmp(buff, "memmap full") == 0) {
        displayFullMemory();
    } else if (strcmp(buff, "mem") == 0) {
        displayMemory();
    } else {
        kprint("-> ");
        kprint(buff);
    }

    buff[0] = '\0';
}

void displayMemory() {
    struct Memory_Block memory_block;
    char buff[50];
    totalAvailableMemory(&memory_block);
    long_to_ascii(memory_block.length, buff);
    pretty_number(buff);
    kprint("Memory available: ");
    kprint(buff);
    kprint("\n");
}

void displayMemoryMap(int dtype) {
    kprint("Memory map:\n") ; 
    char buff[50];
    int count = get_entry_count();
    kprint("  index         start                length           type\n");
    for (int i = 0; i < count; i++) {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (dtype > 0 && type != dtype) continue;

        kprint("    ");
        int_to_ascii(i, buff);
        kprint(buff);
        kprint("   ");
        hex_to_ascii_padded(baseMemory, buff, 16);
        kprint("[ ");
        kprint(buff);
        kprint(" : ");
        hex_to_ascii_padded(length, buff, 16);
        kprint(buff);
        kprint("] : ");
        map_type(type, buff);
        kprint(buff);
        kprint("\n");
    }
    kprint("\n\n");
}

void displayUsableMemory() {
    displayMemoryMap(1);
}

void displayFullMemory() {
    displayMemoryMap(0);
}