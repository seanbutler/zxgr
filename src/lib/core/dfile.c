#include "dfile.h"
#include <string.h>

#define D_FILE_SYSVAR  16396

char *dfile;

void dfile_init(void)
{
    dfile = (char *)(*(unsigned int *)D_FILE_SYSVAR) + 1;
}

void dfile_put_char(int x, int y, char c)
{
    dfile[y * DFILE_STRIDE + x] = c;
}

void dfile_put_num(int x, int y, int n)
{
    dfile[y * DFILE_STRIDE + x]     = 0x1C + (n / 10);
    dfile[y * DFILE_STRIDE + x + 1] = 0x1C + (n % 10);
}

static char ascii_to_zx81(char c)
{
    if (c >= '0' && c <= '9') return 0x1C + (c - '0');
    if (c >= 'A' && c <= 'Z') return 0x26 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 0x26 + (c - 'a');
    switch (c) {
        case ' ': return 0x00;
        case '"': return 0x0B;
        case ':': return 0x0E;
        case '?': return 0x0F;
        case '(': return 0x10;
        case ')': return 0x11;
        case '>': return 0x12;
        case '<': return 0x13;
        case '=': return 0x14;
        case '+': return 0x15;
        case '-': return 0x16;
        case '*': return 0x17;
        case '/': return 0x18;
        case ';': return 0x19;
        case ',': return 0x1A;
        case '.': return 0x1B;
        default:  return 0x00;
    }
}

void dfile_put_string(int x, int y, const char *s)
{
    char *dst = dfile + y * DFILE_STRIDE + x;
    while (*s)
        *dst++ = ascii_to_zx81(*s++);
}

void dfile_put_zstr(int x, int y, const char *s)
{
    char *dst = dfile + y * DFILE_STRIDE + x;
    while (*s != (char)0xFF)
        *dst++ = *s++;
}

void dfile_put_zdata(int x, int y, const char *s, int len)
{
    memcpy(dfile + y * DFILE_STRIDE + x, s, len);
}

void dfile_clear(char c)
{
    int r;
    for (r = 0; r < 24; r++)
        memset(dfile + r * DFILE_STRIDE, c, 32);
}
