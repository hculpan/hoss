#include "string.h"
#include "../drivers/screen.h"

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

/**
 * Based on K&R implementation of int_to_ascii
 */
void long_to_ascii(long n, char str[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void pretty_number(char *input)
{
    int iInputLen = strlen(input);
    // char output[iInputLen + (iInputLen / 3) + 1];
    char output[25];
    int iOutputBufferPos = 0;
    for (int i = 0; i < iInputLen; i++)
    {
        if ((iInputLen - i) % 3 == 0 && i != 0)
        {
            output[iOutputBufferPos++] = ',';
        }

        output[iOutputBufferPos++] = input[i];
    }

    output[iOutputBufferPos] = '\0';
    strcpy(input, output);
}

void long_as_hex(long n, char str[])
{
    str[0] = '\0';
    char zeros = 0;

    int tmp;
    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0)
            continue;
        zeros = 1;
        if (tmp >= 0xA)
            append(str, tmp - 0xA + 'A');
        else
            append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
        append(str, tmp - 0xA + 'A');
    else
        append(str, tmp + '0');
}

void hex_to_ascii(long n, char str[])
{
    char buff[17];
    long_as_hex(n, buff);
    str[0] = '\0';
    strcat(str, "0x");
    strcat(str, buff);
}

void hex_to_ascii_padded(long n, char str[], int bytes_length)
{
    char buff[bytes_length < 18 ? 18 : bytes_length];
    long_as_hex(n, buff);
    str[0] = '\0';
    strcat(str, "0x");
    int l = strlen(buff);
    while (bytes_length > l)
    {
        append(str, '0');
        l++;
    }
    strcat(str, buff);
}

/* K&R */
void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(const char s[])
{
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void append(char s[], char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[])
{
    int len = strlen(s);
    s[len - 1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[])
{
    int i;
    for (i = 0; s1[i] == s2[i]; i++)
    {
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

int strncmp(char s1[], char s2[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
        if (s1[i] == s2[i] == '\0')
            break;
    }
    return 0;
}

char *strcat(char *dest, const char *src)
{
    int i, j;
    for (i = 0; dest[i] != '\0'; i++)
        ;

    for (j = 0; src[j] != '\0'; j++)
    {
        dest[i + j] = src[j];
    }
    dest[i + j] = '\0';
    return dest;
}

char *strcpy(char *destination, const char *source)
{
    int l = strlen(source);
    for (int i = 0; i < l; i++)
    {
        destination[i] = source[i];
    }
    destination[l] = '\0';
    return destination;
}

char *strncpy(char *destination, const char *source, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        destination[i] = source[i];
    }
    destination[len] = '\0';
    return destination;
}

void trim_trailing(char *s)
{
    int l = strlen(s) - 1;
    for (; l >= 0; l--)
    {
        if (!isspace(s[l]))
        {
            s[l + 1] = '\0';
            break;
        }
    }

    if (l == -1)
    {
        s[0] = '\0';
    }
}

char isspace(char c)
{
    return (c == ' ' ? 1 : 0);
}

int atoi(const char *str)
{
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}

char *strtok(char *input, const char delim)
{
    static char *vInput;
    static size_t lastIndex = 0;
    static size_t input_length;
    char *start;

    if (input)
    {
        lastIndex = 0;
        vInput = input;
        start = input;
        input_length = strlen(input);
    }
    else if (lastIndex >= input_length)
    {
        return NULL;
    }
    else
    {
        start = &vInput[lastIndex];
    }

    size_t i;
    for (i = lastIndex; i < input_length; i++)
    {
        if (vInput[i] == delim)
        {
            vInput[i] = '\0';
            break;
        }
    }
    lastIndex = i + 1;

    return start;
}