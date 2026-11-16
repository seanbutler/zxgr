#include "enemy_map.h"
#include "shape_enemy.h"
#include "world_api.h"
#include "rng.h"

const EnemySpawn enemy_map[] = {
    /*  screen (0,0) */
    {  4,  4, ENEMY_BAT },
    { 16,  8, ENEMY_BAT },
    {  8, 16, ENEMY_BAT },
    { 20, 20, ENEMY_BAT },
    /*  screen (1,0) */
    { 28,  6, ENEMY_BAT },
    { 40, 14, ENEMY_BAT },
    /*  sentinel */
    {  0,  0, ENEMY_NONE },
};

void enemies_load_screen(Sprite *enemies, int *anim, int max, int scroll_x, int scroll_y)
{
    const EnemySpawn *sp;
    int count, i;

    for (i = 0; i < max; i++) {
        enemies[i].dx = 0;
        enemies[i].dy = 0;
    }

    count = 0;
    for (sp = enemy_map; sp->type != ENEMY_NONE && count < max; sp++) {
        int sx = (int)sp->wx - scroll_x;
        int sy = (int)sp->wy - scroll_y;
        if (sx >= 0 && sx + SHAPE_ENEMY_W <= GAME_W &&
            sy >= 0 && sy + SHAPE_ENEMY_H <= GAME_H) {
            anim[count] = 0;
            sprite_init(&enemies[count], sx, sy,
                        SHAPE_ENEMY_W, SHAPE_ENEMY_H,
                        (rng_next() & 1) ? 1 : -1,
                        (rng_next() & 1) ? 1 : -1,
                        shape_enemy_frames[0]);
            count++;
        }
    }
}
