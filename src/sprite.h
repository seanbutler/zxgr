#ifndef SPRITE_H
#define SPRITE_H

#define SCREEN_W  32
#define SCREEN_H  24

#include "zx81_semigraphics.h"

#define SPR_W  3
#define SPR_H  2

#define PLAYER_W  4
#define PLAYER_H  3

void draw_sprite(int x, int y);
void erase_sprite(int x, int y, char *bg_scene);
void player_draw(int x, int y);
void player_erase(int x, int y, char *bg_scene);


#endif
