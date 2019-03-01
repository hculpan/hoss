#include "shell.h"

#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/string.h"

static char displayScancode = 0;

void process_command(char *buff);

void start_shell() {
    kprint("Type something!  If you hit Enter/Return, it will echo\nwhat you typed and prompt you again.\n> ");

    char buff[1024];
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
    kprint("-> ");
    kprint(buff);
    if (strcmp(buff, "scancode") == 0) {
        kprint("\nSwitching to scancode mode\n");
        displayScancode = 1;
    }

    buff[0] = '\0';
}