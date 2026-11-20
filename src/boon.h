#ifndef BOON_H
#define BOON_H

#include "sprite.h"

#define BOON_NONE     0
#define BOON_HEALTH   1
#define BOON_AMMO     2
#define BOON_COMPASS  3

typedef struct {
    unsigned char wx, wy;  /* world position in chars */
    unsigned char type;    /* BOON_* — set to BOON_NONE when collected */
} Boon;

extern Boon boon_map[];

void boon_draw_all(int scroll_x, int scroll_y);
void boon_check_collision(Sprite *boons, int player_x, int player_y, int player_w, int player_h,
                           int scroll_x, int scroll_y);
void boons_load_screen(Sprite *boons, int max, int scroll_x, int scroll_y);

#endif
