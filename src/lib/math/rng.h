#ifndef RNG_H
#define RNG_H

// #include <stdint.h>

void rng_seed(unsigned char seed);
unsigned char rng_next(void);

#endif
