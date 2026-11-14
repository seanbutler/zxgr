#ifndef DFILE_H
#define DFILE_H

#define DFILE_STRIDE  33   /* 32 chars + 0x76 newline per row */

extern char *dfile;

void dfile_init(void);
void put_char(int x, int y, char c);

#endif
