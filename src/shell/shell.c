#include "shell.h"

#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../kernel/memory.h"
#include "../cpu/isr.h"

#include "../kernel/mem_manager.h"

static char displayScancode = 0;

static void *last_ptr[100];
static int index = -1;

void process_command(char *buff);
void displayUsableMemory();
void displayFullMemory();
void displayMemoryMap(int dtype);
void displayMemorySegments();
void displayMemory();
void allocateRam();

void start_shell()
{
    kprint("Type something!\n");
    kprint("> ");

    char buff[256];
    char str[2];
    str[1] = '\0';
    while (1)
    {
        if (displayScancode)
        {
            unsigned char code = waitForScancode();
            int_to_ascii(code, buff);
            kprint("{");
            kprint(buff);
            kprint("}\n");
        }
        else
        {
            str[0] = waitForAscii();
            if (str[0] == '\r')
            {
                kprint(str);
                trim_trailing(buff);
                process_command(buff);
                kprint("\n> ");
            }
            else if (str[0] == '\b')
            {
                int len = strlen(buff);
                if (len > 0)
                {
                    kprint_backspace();
                    buff[len - 1] = '\0';
                }
            }
            else
            {
                kprint(str);
                append(buff, str[0]);
            }
        }
    }
}

void process_command(char *buff)
{
    if (strcmp(buff, "scancode") == 0)
    {
        kprint("\nSwitching to scancode mode\n");
        displayScancode = 1;
    }
    else if (strcmp(buff, "memmap") == 0)
    {
        displayUsableMemory();
    }
    else if (strcmp(buff, "memmap full") == 0)
    {
        displayFullMemory();
    }
    else if (strcmp(buff, "mem") == 0)
    {
        displayMemory();
    }
    else if (strcmp(buff, "mem segments") == 0)
    {
        displayMemorySegments();
    }
    else if (strcmp(buff, "alloc") == 0)
    {
        allocateRam();
    }
    else if (strncmp(buff, "free", 4) == 0)
    {
        freeRam(buff);
    }
    else if (strcmp(buff, "test") == 0)
    {
        runTest();
    }
    else
    {
        kprint("-> ");
        kprint(buff);
    }

    buff[0] = '\0';
}

void runTest()
{
    for (int i = 0x100000; i < 1024 * 1024 * 1024; i++)
    {
        char buff[10];
        hex_to_ascii(i, buff);
        kprint(buff);
        kprint("\n");
        if (isInt13Thrown())
        {
            kprint("Got exception!\n");
            break;
        }
    }
}

void freeRam(char *buff)
{
    int mem_block = -1;

    if (index < 0)
        return;

    char *tok = strtok(buff, ' ');
    size_t tok_index = 0;
    while (tok)
    {
        if (tok_index == 1)
        {
            mem_block = atoi(tok);
            kprint(tok);
            kprint("\n");
        }
        tok = strtok(NULL, ' ');
        tok_index++;
    }

    if (mem_block >= 0)
    {
        free(last_ptr[mem_block]);
        last_ptr[mem_block] = 0;
    }
    else
    {
        free(last_ptr[index]);
        last_ptr[index--] = 0;
    }

    displayMemory();
    displayMemorySegments();
}

void allocateRam()
{
    void *ptr = allocate(4096);
    if (!ptr)
    {
        kprint("Uh-oh!\n");
    }
    else
    {
        last_ptr[++index] = ptr;
        kprint("Got it!  Starting address: ");
        char buff[20];
        hex_to_ascii_padded(ptr, buff, 16);
        kprint(buff);
        kprint("\n");
    };
    displayMemory();
    displayMemorySegments();
}

void displayMemoryValue(unsigned int value, char *descr)
{
    char buff[50];
    long_to_ascii(value, buff);
    pretty_number(buff);
    kprint(descr);
    kprint(buff);
    kprint("\n");
}

void displayMemorySegments()
{
    struct Memory_Segment *curr = get_segments();

    kprint("Memory Segments:\n");
    char buff[50];
    kprint("           start                length          type\n");
    while (1)
    {
        kprint("   ");
        hex_to_ascii_padded((unsigned long)curr, buff, 16);
        kprint("[ ");
        kprint(buff);
        kprint(" : ");
        hex_to_ascii_padded(curr->length, buff, 16);
        kprint(buff);
        kprint("] : ");
        switch (curr->type)
        {
        case TYPE_FREE:
            kprint("free");
            break;
        case TYPE_OS:
            kprint("system");
            break;
        case TYPE_USER:
            kprint("user");
            break;
        case TYPE_EOM:
            kprint("End-of-Memory marker");
            break;
        default:
            kprint("unknown");
            break;
        }
        kprint("\n");

        if (curr->type == TYPE_EOM)
        {
            break;
        }

        curr = (struct Memory_Segment *)((void *)curr + curr->length + sizeof(struct Memory_Segment));
    }
    kprint("\n\n");
}

void displayMemory()
{
    displayMemoryValue(get_total_memory(), "Total Memory: ");
    displayMemoryValue(get_os_memory(), "  System : ");
    displayMemoryValue(get_user_memory(), "  User   : ");
    displayMemoryValue(get_free_memory(), "  Free   : ");
}

void displayMemoryMap(int dtype)
{
    kprint("Memory map:\n");
    char buff[50];
    int count = get_entry_count();
    kprint("  index         start                length           type\n");
    for (int i = 0; i < count; i++)
    {
        unsigned long baseMemory, length;
        int type;

        get_entry(i, &baseMemory, &length, &type);

        if (dtype > 0 && type != dtype)
            continue;

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

void displayUsableMemory()
{
    displayMemoryMap(1);
}

void displayFullMemory()
{
    displayMemoryMap(0);
}
