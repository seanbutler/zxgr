#ifndef SPRITE_H
#define SPRITE_H

#define SCREEN_W  32
#define SCREEN_H  24

#include "zx81_semigraphics.h"

typedef struct {
    int x, y;
    int w, h;
    int dx, dy;
    const char *data;
} Sprite;

extern const char shape_enemy[];
extern const char shape_player[];

void sprite_init(Sprite *s, int x, int y, int w, int h, int dx, int dy, const char *data);
void sprite_move(Sprite *s);
void sprite_draw(const Sprite *s);
void sprite_erase(const Sprite *s, const char *bg);

#endif
