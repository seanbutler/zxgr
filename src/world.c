#include "world_api.h"

#define SCREEN_W  32
#define SCREEN_H  24

#include "dfile.h"

static void blit_tile(const World *w, int ti, int screen_x, int screen_y)
{
    int stride = (int)w->tile_w * (int)w->tile_h;
    const char *src = w->tiles + ti * stride;
    int r, c, sy;

    for (r = 0; r < w->tile_h; r++) {
        sy = screen_y + r;
        if (sy >= 0 && sy < SCREEN_H) {
            char *dst = dfile + sy * DFILE_STRIDE + screen_x;
            for (c = 0; c < w->tile_w; c++) {
                int sx = screen_x + c;
                if (sx >= 0 && sx < SCREEN_W)
                    dst[c] = src[c];
            }
        }
        src += w->tile_w;
    }
}

void world_draw(const World *w, int scroll_x, int scroll_y)
{
    const unsigned char *p;
    int tx, ty, ti, sx, sy;

    /* clear screen to background (white/space) */
    {
        int r, c;
        for (r = 0; r < SCREEN_H; r++)
            for (c = 0; c < SCREEN_W; c++)
                // put_char(c, r, 0x80);
                put_char(c, r, 0x00);
    }

    p = w->map;
    while (*p != 0xFF) {
        tx = (int)p[0];
        ty = (int)p[1];
        ti = (int)p[2];
        p += 3;

        sx = tx * (int)w->tile_w - scroll_x;
        sy = ty * (int)w->tile_h - scroll_y;

        if (sx + (int)w->tile_w <= 0 || sx >= SCREEN_W) continue;
        if (sy + (int)w->tile_h <= 0 || sy >= SCREEN_H) continue;

        blit_tile(w, ti, sx, sy);
    }
}

char world_char_at(const World *w, int wx, int wy)
{
    const unsigned char *p;
    int tx, ty, ti, lx, ly;
    int stride;

    tx = wx / (int)w->tile_w;
    ty = wy / (int)w->tile_h;
    lx = wx % (int)w->tile_w;
    ly = wy % (int)w->tile_h;

    p = w->map;
    while (*p != 0xFF) {
        if ((int)p[0] == tx && (int)p[1] == ty) {
            ti     = (int)p[2];
            stride = (int)w->tile_w * (int)w->tile_h;
            return w->tiles[ti * stride + ly * (int)w->tile_w + lx];
        }
        p += 3;
    }
    return 0x00;
}
