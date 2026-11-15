#include "sprite.h"
#include "dfile.h"
#include "world_api.h"

void sprite_init(Sprite *s, int x, int y, int w, int h, int dx, int dy, const char *data)
{
    s->x    = x;
    s->y    = y;
    s->w    = w;
    s->h    = h;
    s->dx   = dx;
    s->dy   = dy;
    s->data = data;
}

void sprite_move(Sprite *s)
{
    s->x += s->dx;
    s->y += s->dy;
}

void sprite_draw(const Sprite *s)
{
    int r, c;
    for (r = 0; r < s->h; r++) {
        char *dst = dfile + (s->y + r) * DFILE_STRIDE + s->x;
        const char *src = s->data + r * s->w;
        for (c = 0; c < s->w; c++)
            dst[c] = src[c];
    }
}

void sprite_erase(const Sprite *s, const char *bg)
{
    int r, c;
    for (r = 0; r < s->h; r++) {
        int sy = s->y + r;
        if (sy >= GAME_H) continue;
        for (c = 0; c < s->w; c++) {
            int sx = s->x + c;
            if (sx < GAME_W)
                dfile_put_char(sx, sy, bg[sy * GAME_W + sx]);
        }
    }
}
