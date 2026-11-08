#ifndef SPRITE_H
#define SPRITE_H

#define SCREEN_W  32
#define SCREEN_H  24

#include "zx81_semigraphics.h"

#define SPR_W  3
#define SPR_H  2

void draw_sprite(int x, int y);
void erase_sprite(int x, int y, char *bg_scene);


#endif
