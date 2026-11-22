#ifndef SPRITE_H
#define SPRITE_H

#define SCREEN_W  32
#define SCREEN_H  24

#include <stdint.h>
#include "zx81_semigraphics.h"

/* There is a compiler issue here: 16 bit copies elsewhere mean these cant be uint8_t */

typedef struct {
    int x, y;
    int w, h;
    int dx, dy;
    const char *data;
} Sprite;

void sprite_init(Sprite *s, int x, int y, int w, int h, int dx, int dy, const char *data);
void sprite_move(Sprite *s);
void sprite_draw(const Sprite *s);
void sprite_erase(const Sprite *s, const char *bg);
int sprite_overlaps(const Sprite *a, const Sprite *b);

#define sprite_is_moving(s) ((s)->dx != 0 || (s)->dy != 0)

#endif
