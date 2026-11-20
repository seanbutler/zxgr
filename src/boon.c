#include "boon.h"
#include "shape_boon.h"
#include "dfile.h"
#include "world_api.h"
#include "player_state.h"
#include "sprite.h"

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

void boon_check_collision(Sprite *boons, int player_x, int player_y, int player_w, int player_h,
                           int scroll_x, int scroll_y)
{
    Boon *b;
    int i;
    for (b = boon_map; b->type != BOON_NONE; b++) {
        if (b->type == BOON_NONE) continue;

        int sx = (int)b->wx - scroll_x;
        int sy = (int)b->wy - scroll_y;
        int boon_w = SHAPE_BOON_W;
        int boon_h = SHAPE_BOON_H;

        if (player_x < sx + boon_w && player_x + player_w > sx &&
            player_y < sy + boon_h && player_y + player_h > sy) {

            switch (b->type) {
                case BOON_HEALTH:
                    player_state.health++;
                    break;
                case BOON_AMMO:
                    player_state.ammo += 3;
                    break;
                case BOON_COMPASS:
                    player_state.has_compass = 1;
                    break;
            }

            b->type = BOON_NONE;

            for (i = 0; i < 4; i++) {
                if (boons[i].x == sx && boons[i].y == sy) {
                    sprite_erase(&boons[i], world_bg_scene());
                    boons[i].dx = 0;
                    boons[i].dy = 0;
                    break;
                }
            }
        }
    }
}

void boons_load_screen(Sprite *boons, int max, int scroll_x, int scroll_y)
{
    Boon *b;
    int count, i;

    for (i = 0; i < max; i++) {
        boons[i].dx = 0;
        boons[i].dy = 0;
    }

    count = 0;
    for (b = boon_map; b->type != BOON_NONE && count < max; b++) {
        if (b->type == BOON_NONE) continue;

        int sx = (int)b->wx - scroll_x;
        int sy = (int)b->wy - scroll_y;
        if (sx >= 0 && sx + SHAPE_BOON_W <= GAME_W &&
            sy >= 0 && sy + SHAPE_BOON_H <= GAME_H) {
            sprite_init(&boons[count], sx, sy,
                        SHAPE_BOON_W, SHAPE_BOON_H,
                        0, 0,
                        boon_shapes[b->type]);
            count++;
        }
    }
}
