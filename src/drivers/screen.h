#ifndef SCREEN_H
#define SCREEN_H

void clear_screen();

void move_cursor_to(int col, int row);

void kprint_at(char *message, int col, int row);

void kprint(char *message);

void kprint_backspace();

#endif