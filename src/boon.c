#include "boon.h"
#include "shape_boon.h"
#include "dfile.h"
#include "world_api.h"

static const char * const boon_shapes[] = {
    0,
    shape_boon_health,
    shape_boon_ammo,
    shape_boon_compass,
};

void boon_draw_all(int scroll_x, int scroll_y)
{
    Boon *b;
    for (b = boon_map; b->type != BOON_NONE; b++) {
        int sx = (int)b->wx - scroll_x;
        int sy = (int)b->wy - scroll_y;
        const char *src = boon_shapes[b->type];
        int r, c;
        if (sx + SHAPE_BOON_W <= 0 || sx >= GAME_W) continue;
        if (sy + SHAPE_BOON_H <= 0 || sy >= GAME_H) continue;
        for (r = 0; r < SHAPE_BOON_H; r++) {
            for (c = 0; c < SHAPE_BOON_W; c++) {
                char ch = src[r * SHAPE_BOON_W + c];
                int px = sx + c;
                int py = sy + r;
                if (ch && px >= 0 && px < GAME_W && py >= 0 && py < GAME_H)
                    dfile_put_char(px, py, ch);
            }
        }
    }
}
