#include "zx81_semigraphics.h"
#include "shape_boon.h"

const char shape_boon_health[] = {
    ZX81_INV(CH_QTR_TL),  CH_HALF_B,    ZX81_INV(CH_QTR_TR),
    CH_FULL,    CH_FULL,    CH_FULL,
    CH_QTR_TR,  CH_FULL,   CH_QTR_TL,
};

const char shape_boon_compass[] = {
    CH_DIAG_A,  CH_HALF_T,  CH_DIAG_B,
    CH_HALF_L,  CH_SLASH,   CH_HALF_R,
    CH_DIAG_B,  CH_HALF_B,  CH_DIAG_A,
};

const char shape_boon_ammo[] = {
    CH_HALF_L,   CH_HALF_L,  CH_HALF_L,
    CH_HALF_L,  CH_HALF_L,    CH_HALF_L,
    ZX81_INV(CH_QTR_TR),   ZX81_INV(CH_QTR_TR),  ZX81_INV(CH_QTR_TR),
};
