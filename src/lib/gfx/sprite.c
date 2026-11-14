#include "sprite.h"

#include "dfile.h"

const char shape_enemy[] = {
    CH_INV_TR, CH_INV_TL,
    CH_INV_BL, CH_INV_BR,
};

const char shape_player[] = {
    CH_SPACE, CH_GREY,  CH_GREY,  CH_SPACE,
    CH_FULL,  CH_FULL,  CH_FULL,  CH_FULL,
    CH_GREY,  CH_SPACE, CH_SPACE, CH_GREY,
};

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
        if (sy >= SCREEN_H) continue;
        for (c = 0; c < s->w; c++) {
            int sx = s->x + c;
            if (sx < SCREEN_W)
                dfile_put_char(sx, sy, bg[sy * SCREEN_W + sx]);
        }
    }
}
