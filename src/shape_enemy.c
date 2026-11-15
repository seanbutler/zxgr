#include "zx81_semigraphics.h"
#include "shape_enemy.h"

static const char shape_enemy_0[] = {
    CH_INV_TR, CH_SPACE, CH_INV_TL,
    CH_INV_BL, ZX81_INV(CH_QUOTE), CH_INV_BR,
};

static const char shape_enemy_1[] = {
    CH_INV_TL, ZX81_INV(CH_QUOTE), CH_INV_TR,
    CH_INV_BR, CH_SPACE, CH_INV_BL,
};

const char * const shape_enemy_frames[SHAPE_ENEMY_FRAMES] = {
    shape_enemy_0,
    shape_enemy_1,
};
