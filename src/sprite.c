#include "sprite.h"

extern char *dfile;
extern void put_char(int x, int y, char c);

static const char spr[SPR_H][SPR_W] = {
    { CH_INV_TR, CH_INV_TL },
    { CH_INV_BL, CH_INV_BR },
};

static const char player_shape[PLAYER_H][PLAYER_W] = {
    { CH_SPACE, CH_GREY,  CH_GREY,  CH_SPACE },
    { CH_FULL,  CH_FULL,  CH_FULL,  CH_FULL  },
    { CH_GREY,  CH_SPACE, CH_SPACE, CH_GREY  },
};

void sprite_init(Sprite *s, int x, int y, int dx, int dy)
{
    s->x  = x;
    s->y  = y;
    s->dx = dx;
    s->dy = dy;
}

void sprite_move(Sprite *s)
{
    s->x += s->dx;
    s->y += s->dy;
}

void sprite_draw(const Sprite *s)
{
    int r, c;
    for (r = 0; r < SPR_H; r++) {
        char *dst = dfile + (s->y + r) * 33 + s->x;
        for (c = 0; c < SPR_W; c++)
            dst[c] = spr[r][c];
    }
}

void sprite_erase(const Sprite *s, const char *bg)
{
    int r, c;
    for (r = 0; r < SPR_H; r++) {
        int sy = s->y + r;
        if (sy >= SCREEN_H) continue;
        for (c = 0; c < SPR_W; c++) {
            int sx = s->x + c;
            if (sx < SCREEN_W)
                put_char(sx, sy, bg[sy * SCREEN_W + sx]);
        }
    }
}

void player_draw(const Sprite *s)
{
    int r, c;
    for (r = 0; r < PLAYER_H; r++) {
        char *dst = dfile + (s->y + r) * 33 + s->x;
        for (c = 0; c < PLAYER_W; c++)
            dst[c] = player_shape[r][c];
    }
}

void player_erase(const Sprite *s, const char *bg)
{
    int r, c;
    for (r = 0; r < PLAYER_H; r++) {
        int sy = s->y + r;
        if (sy >= SCREEN_H) continue;
        for (c = 0; c < PLAYER_W; c++) {
            int sx = s->x + c;
            if (sx < SCREEN_W)
                put_char(sx, sy, bg[sy * SCREEN_W + sx]);
        }
    }
}
