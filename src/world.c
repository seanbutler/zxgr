#include "world_api.h"
#include "dfile.h"

static char bg_scene[GAME_H * GAME_W];

void world_cache_bg(void)
{
    int x, y;
    for (y = 0; y < GAME_H; y++) {
        const char *src = dfile + y * DFILE_STRIDE;
        char *dst = bg_scene + y * GAME_W;
        for (x = 0; x < GAME_W; x++)
            dst[x] = src[x];
    }
}

const char *world_bg_scene(void)
{
    return bg_scene;
}

static void blit_tile(const World *w, int ti, int screen_x, int screen_y)
{
    int stride = (int)w->tile_w * (int)w->tile_h;
    const char *src = w->tiles + ti * stride;
    int r, c, sy, sx;

    for (r = 0; r < w->tile_h; r++) {
        sy = screen_y + r;
        if (sy >= 0 && sy < GAME_H) {
            char *dst = dfile + sy * DFILE_STRIDE + screen_x;
            for (c = 0; c < w->tile_w; c++) {
                sx = screen_x + c;
                if (sx >= 0 && sx < GAME_W)
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
        for (r = 0; r < GAME_H; r++)
            for (c = 0; c < GAME_W; c++)
                dfile_put_char(c, r, 0x00);
    }

    p = w->map;
    while (*p != 0xFF) {
        tx = (int)p[0];
        ty = (int)p[1];
        ti = (int)p[2];
        p += 3;

        sx = tx * (int)w->tile_w - scroll_x;
        sy = ty * (int)w->tile_h - scroll_y;

        if (sx + (int)w->tile_w <= 0 || sx >= GAME_W) continue;
        if (sy + (int)w->tile_h <= 0 || sy >= GAME_H) continue;

        blit_tile(w, ti, sx, sy);
    }
}

char world_char_at(const World *w, int wx, int wy)
{
    const unsigned char *p;
    int tx, ty, ti, lx, ly, stride;

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
