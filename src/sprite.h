#ifndef SPRITE_H
#define SPRITE_H

#define SCREEN_W  32
#define SCREEN_H  24

#include "zx81_semigraphics.h"

#define SPR_W  2
#define SPR_H  2

#define PLAYER_W  4
#define PLAYER_H  3

typedef struct {
    int x, y;
    int dx, dy;
} Sprite;

void sprite_init(Sprite *s, int x, int y, int dx, int dy);
void sprite_move(Sprite *s);
void sprite_draw(const Sprite *s);
void sprite_erase(const Sprite *s, const char *bg);

void player_draw(const Sprite *s);
void player_erase(const Sprite *s, const char *bg);

#endif
