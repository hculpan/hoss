#ifndef STRING_H
#define STRING_H

#include "types.h"

void int_to_ascii(int n, char str[]);

void long_to_ascii(long n, char str[]);

int atoi(const char *str);

void pretty_number(char *input);

void reverse(char s[]);

size_t strlen(const char s[]);

void append(char s[], char n);

void backspace(char s[]);

int strcmp(char s1[], char s2[]);

int strncmp(char s1[], char s2[], size_t len);

void hex_to_ascii(long n, char str[]);

void hex_to_ascii_padded(long n, char str[], int bytes_length);

char *strcat(char *dest, const char *src);

char *strcpy(char *destination, const char *source);

char *strncpy(char *destination, const char *source, size_t len);

void trim_trailing(char *s);

char isspace(char c);

char *strtok(char *input, const char delim);

/*
    Only supports %d, %%, and %s at the moment
*/
size_t sprintf(char *str, const char *format, ...);

size_t printf(const char *format, ...);

int indexof(char *str, char c, int startindex);

#endif