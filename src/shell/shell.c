#include "shell.h"

#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../kernel/memory.h"
#include "../cpu/isr.h"

#include "../kernel/mem_manager.h"

static char displayScancode = 0;

static struct Memory_Segment *last_ptr[100];
static int index = -1;

void process_command(char *buff);
char **parse_command(char *buff, size_t *max_elements);
void displayUsableMemory();
void displayFullMemory();
void displayMemoryMap(int dtype);
void displayMemorySegments();
void displayMemory();
void allocateRam(char *buff);

void start_shell()
{
    kprint("> ");

    char buff[255];
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

char **parse_command(char *buff, size_t *max_elements)
{
    size_t num_elements = 0;
    char **result = allocate(sizeof(char *) * *max_elements);
    for (int i = 0; i < *max_elements; i++)
    {
        result[i] = NULL;
    }

    for (int i = 0; i < *max_elements; i++)
    {
        if (i == 0)
            result[i] = strtok(buff, ' ');
        else
            result[i] = strtok(NULL, ' ');
        if (!result[i])
            break;
        num_elements++;
    }
    *max_elements = num_elements;
    return result;
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
    else if (strncmp(buff, "alloc", 5) == 0)
    {
        allocateRam(buff);
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
    int mem_block = 0;

    if (index < 0)
        return;

    size_t num_elements = 10;
    char **opts = parse_command(buff, &num_elements);

    switch (num_elements)
    {
    case 1:
        mem_block = 0;
        break;
    case 2:
        mem_block = atoi(opts[1]);
        break;
    default:
        kprint("Invalid parameter count for 'free'");
        return;
    }
    free(opts);

    if (last_ptr[mem_block]->type == TYPE_USER)
        free((void *)last_ptr[mem_block] + sizeof(struct Memory_Segment));

    displayMemory();
    displayMemorySegments();
}

void allocateRam(char *buff)
{
    u32 mem_size = 4096;
    size_t num_elements = 10;
    char **opts = parse_command(buff, &num_elements);
    char buffer[255];
    char buffer2[20];

    switch (num_elements)
    {
    case 1:
        break;
    case 2:
        mem_size = atoi(opts[1]);
        break;
    default:
        mem_size = 12;
        strcpy(buff, "alloc");
        int_to_ascii(buff, buffer2);
        kprint(buffer2);
        printf("Invalid parameter count for '%s' %d blah %d\n", buff, mem_size, num_elements);
        free(opts);
        return;
    }
    free(opts);

    void *ptr = allocate(mem_size);
    if (!ptr)
    {
        kprint("Uh-oh!\n");
    }
    else
    {
        last_ptr[++index] = ptr;
        kprint("Starting address: ");
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
    for (int i = 0; i < 100; i++)
        last_ptr[i] = NULL;

    struct Memory_Segment *curr = get_segments();

    kprint("Memory Segments:\n");
    char buff[50];
    kprint("           start                length          type\n");
    for (int i = 0; i < 100; i++)
    {
        kprint("   ");
        hex_to_ascii_padded((unsigned long)curr, buff, 16);
        last_ptr[i] = curr;
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
            break;

        curr = (struct Memory_Segment *)((void *)curr + curr->length + sizeof(struct Memory_Segment));
    }
    kprint("\n\n");
}

void displayMemory()
{
    displayMemoryValue(get_total_memory(), "Total Memory: ");
    displayMemoryValue(get_os_memory(), "  System overhead: ");
    displayMemoryValue(get_user_memory(), "  User           : ");
    displayMemoryValue(get_free_memory(), "  Free           : ");
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
