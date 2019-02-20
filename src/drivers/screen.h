#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS   0xb8000
#define MAX_ROWS        25
#define MAX_COLS        80

#define DEFAULT_ATTR    0x07

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

void clear_screen();

void move_cursor_to(int col, int row);

void kprint_at(char *message, int col, int row);

void kprint(char *message);

void kprint_backspace();

#endif