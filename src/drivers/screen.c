#include "screen.h"
#include "../cpu/ports.h"
#include "../libc/mem.h"

#define VIDEO_ADDRESS   0xb8000
#define MAX_ROWS        25
#define MAX_COLS        80

#define DEFAULT_ATTR    0x07

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

int get_screen_offset(int col, int row) { return ((row * MAX_COLS) + col) * 2; }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

int handle_scrolling(int offset) {
    if (offset < MAX_ROWS * MAX_COLS * 2) {
        return offset;
    }

    int i;
    for (i = 1; i < MAX_ROWS; i++) {
        mem_copy((char *)get_screen_offset(0, i) + VIDEO_ADDRESS,
                 (char *)get_screen_offset(0, i - 1) + VIDEO_ADDRESS,
                 MAX_COLS * 2
        );
    }

    char *last_line = (char *)get_screen_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS;
    for (i = 0; i < MAX_COLS * 2; i++) {
        last_line[i] = 0;
    }

    offset -= 2 * MAX_COLS;

    return offset;
}

int print_char(char character, int col, int row, char attribute_byte) {
    unsigned char * vidmem = (unsigned char *)VIDEO_ADDRESS;

    if (!attribute_byte) {
        attribute_byte = DEFAULT_ATTR;
    }

    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);
    } else {
        offset = get_cursor_offset();
    }

    if (character == '\n' || character == '\r') {
        row = get_offset_row(offset);
        offset = get_screen_offset(0, row + 1);
    } else {
        vidmem[offset] = character;
        vidmem[offset + 1] = attribute_byte;
        offset += 2;
    }

    offset = handle_scrolling(offset);
    set_cursor_offset(offset);

    return offset;
}

void kprint_backspace() {
    int offset = get_cursor_offset() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(' ', col, row, DEFAULT_ATTR);
    set_cursor_offset(offset);
}

void kprint_at(char *message, int col, int row) {
    if (col >= MAX_COLS || row >= MAX_ROWS) return;

    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);
    } else {
        offset = get_cursor_offset();
        col = get_offset_col(offset);
        row = get_offset_row(offset);
    }

    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, DEFAULT_ATTR);
        col = get_offset_col(offset);
        row = get_offset_row(offset);
    }
    set_cursor_offset(get_screen_offset(col, row));
}

void kprint(char *message) {
    kprint_at(message, -1, -1);
}

void move_cursor_to(int col, int row) {
    set_cursor_offset(get_screen_offset(col, row));
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = (char *)VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) {
        screen[i * 2] = ' ';
        screen[i * 2 + 1] = DEFAULT_ATTR;
    }
    move_cursor_to(0, 0);
}

