#include "sprite.h"


extern char *dfile;
extern void put_char(int x, int y, char c);


/*
 * 3x2 "flying saucer":
 *   ▄█▄   dome: 3/4 blocks with white notch at outer top corners
 *   ███   solid body
 */
static const char spr[SPR_H][SPR_W] = {
    { CH_INV_TR, CH_FULL, CH_INV_TL },
    { CH_FULL,   CH_FULL, CH_FULL   },
};

void draw_sprite(int x, int y)
{
    int i;
    char *dst = dfile + y * 33 + x;
    const char *src = &spr[0][0];

    for (i = 0; i < SPR_W * SPR_H; i++) {
        dst[0] = src[0];
        dst++;
        src++;
        if (i == SPR_W - 1)
            dst += (33 - SPR_W);
    }
}

void erase_sprite(int x, int y, char *bg_scene)
{
    int dx, dy;

    for (dy = 0; dy < SPR_H; dy++)
    {
        for (dx = 0; dx < SPR_W; dx++)
        {
            int subx = x + dx;
            int suby = y + dy;
            
            if (subx < SCREEN_W && suby < SCREEN_H)
            {
                put_char(subx, suby, bg_scene[suby * SCREEN_W + subx]);
            }
        }
    }
}