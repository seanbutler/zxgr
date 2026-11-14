#include "dfile.h"

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
