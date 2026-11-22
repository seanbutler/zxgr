#include "screen_rle.h"
#include "dfile.h"

#define SCREEN_RLE_MARK 0xFF

void screen_rle_draw_centered(const unsigned char *src, 
                                int content_w, int content_h, 
                                unsigned char clear_char)
{
    int x0 = (32 - content_w) / 2;
    int y0 = (24 - content_h) / 2;
    int x = 0;
    int y = 0;
    int ry, rx;

    for (ry = 0; ry < 24; ry++)
        for (rx = 0; rx < 32; rx++)
            dfile[ry * DFILE_STRIDE + rx] = clear_char;

    while (1) {
        unsigned char b = *src++;
        unsigned char n;
        unsigned char v;

        if (b != SCREEN_RLE_MARK) {
            dfile[(y0 + y) * DFILE_STRIDE + (x0 + x)] = b;
            x++;
            if (x == content_w) {
                x = 0;
                y++;
            }
            continue;
        }

        n = *src++;
        if (n == 0xFF) break;

        if (n == 0) {
            n = 1;
            v = SCREEN_RLE_MARK;
        } else {
            v = *src++;
        }

        while (n--) {
            dfile[(y0 + y) * DFILE_STRIDE + (x0 + x)] = v;
            x++;
            if (x == content_w) {
                x = 0;
                y++;
            }
        }
    }
}
