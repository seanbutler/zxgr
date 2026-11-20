#include "enemy_map.h"

#include "shape_enemy.h"
#include "world_api.h"
#include "rng.h"

/* Generated from world.png -- do not edit */
const EnemySpawn enemy_map[] = {
    {  41,  10, ENEMY_BAT },
    {  83,  18, ENEMY_BAT },
    {  66,  19, ENEMY_BAT },
    {  20,  31, ENEMY_BAT },
    {  31,  31, ENEMY_BAT },
    {  83,  34, ENEMY_BAT },
    {  88,  39, ENEMY_BAT },
    {   8,  49, ENEMY_BAT },
    {  44,  49, ENEMY_BAT },
    {  33,  50, ENEMY_BAT },
    {  71,  50, ENEMY_BAT },
    {  15,  53, ENEMY_BAT },
    {  66,  56, ENEMY_BAT },
    { 113,  65, ENEMY_BAT },
    { 105,  69, ENEMY_BAT },
    {   9,  72, ENEMY_BAT },
    {  50,  73, ENEMY_BAT },
    {  22,  74, ENEMY_BAT },
    {  35,  74, ENEMY_BAT },
    {   8,  83, ENEMY_BAT },
    {  21,  83, ENEMY_BAT },
    {  92,  85, ENEMY_BAT },
    { 105,  85, ENEMY_BAT },
    {   0,   0, ENEMY_NONE },  /* sentinel */
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
