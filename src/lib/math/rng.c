#include "rng.h"

static unsigned char rng_state = 0xAC;

void rng_seed(unsigned char seed)
{
    if (seed == 0) seed = 1;
    rng_state = seed;
}

/* 8-bit xorshift (Marsaglia triple 7,5,3) — period 255, branch-free */
int rng_next(void)
{
    rng_state ^= rng_state << 7;
    rng_state ^= rng_state >> 5;
    rng_state ^= rng_state << 3;
    return rng_state;
}


