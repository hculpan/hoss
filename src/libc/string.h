#ifndef STRING_H
#define STRING_H

void int_to_ascii(int n, char str[]);

void long_to_ascii(long n, char str[]);

void pretty_number(char* input);

void reverse(char s[]);

int strlen(const char s[]);

void append(char s[], char n);

void backspace(char s[]);

int strcmp(char s1[], char s2[]);

void hex_to_ascii(long n, char str[]);

void hex_to_ascii_padded(long n, char str[], int bytes_length);

char *strcat(char *dest, const char *src);

char * strcpy ( char * destination, const char * source );

void trim_trailing(char *s);

char isspace(char c);

#endif