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
    uint8_t r, c;
    for (r = 0; r < s->h; r++) {
        char *dst = dfile + (s->y + r) * DFILE_STRIDE + s->x;
        const char *src = s->data + r * s->w;
        for (c = 0; c < s->w; c++)
            dst[c] = src[c];
    }
}

void sprite_erase(const Sprite *s, const char *bg)
{
    uint8_t r, c;
    int sy, sx;
    for (r = 0; r < s->h; r++) {
        sy = s->y + r;
        if (sy < 0 || sy >= GAME_H) continue;
        for (c = 0; c < s->w; c++) {
            sx = s->x + c;
            if (sx >= 0 && sx < GAME_W)
                dfile_put_char(sx, sy, bg[sy * GAME_W + sx]);
        }
    }
}

int sprite_overlaps(const Sprite *a, const Sprite *b)
{
    return a->x < b->x + b->w && a->x + a->w > b->x &&
           a->y < b->y + b->h && a->y + a->h > b->y;
}
