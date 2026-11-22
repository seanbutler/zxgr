#ifndef ENEMY_MAP_H
#define ENEMY_MAP_H

#include "sprite.h"

#define ENEMY_NONE  0
#define ENEMY_BAT   1

typedef struct {
    unsigned char wx, wy;
    unsigned char type;
} EnemySpawn;

extern const EnemySpawn enemy_map[];

void enemies_load_screen(Sprite *enemies, int *anim, int max, int scroll_x, int scroll_y);
int enemy_check_projectile(Sprite *enemies, int max, const Sprite *projectile, const char *bg);
int enemy_check_player(Sprite *enemies, int max, const Sprite *player);

#endif
