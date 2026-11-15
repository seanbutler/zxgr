#include "zx81_semigraphics.h"

const char shape_enemy[] = {
    CH_INV_TR, CH_SPACE, CH_INV_TL,
    CH_INV_BL, ZX81_INV(CH_QUOTE), CH_INV_BR,
};

const char shape_enemy2[] = {
    CH_INV_TL, ZX81_INV(CH_QUOTE), CH_INV_TR,
    CH_INV_BR, CH_SPACE, CH_INV_BL,
};
