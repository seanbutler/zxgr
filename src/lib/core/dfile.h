#ifndef DFILE_H
#define DFILE_H

#include <stdint.h>

#define DFILE_STRIDE  33   /* 32 chars + 0x76 newline per row */

extern char *dfile;

void dfile_init(void);
void dfile_put_char(int x, int y, char c);
void dfile_put_num(int x, int y, int n);            /* 2-digit decimal at (x,y) */
void dfile_put_string(int x, int y, const char *s);  /* ASCII string → ZX81 codes */
void dfile_put_zstr(int x, int y, const char *s);           /* pre-encoded, 0xFF terminated — has loop  */
void dfile_put_zdata(int x, int y, const char *s, int len); /* pre-encoded, known length — LDIR memcpy  */
void dfile_clear(char c);                                   /* fill every char cell with c              */
#define ZSTR_LEN(s) (sizeof(s) - 1)                         /* compile-time length, excluding 0xFF      */

#endif
