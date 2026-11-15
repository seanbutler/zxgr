/*
 * ZX81 2-Layer Graphics Demo
 *
 * Layer 1 (background): Tile-based world from seiklus_bg.h
 * Layer 2 (foreground): Bouncing saucer sprites
 *
 * The display file (D_FILE) is a 32x24 character grid accessed directly.
 * Each row is 33 bytes: 32 character codes + 0x76 (newline).
 * D_FILE itself starts with an extra 0x76 before row 0.
 *
 * Build:  make
 * Run:    make run
 */

#pragma string name GrDemo

#include <zx81.h>
#include "dfile.h"
#include "zx81_semigraphics.h"
#include "sprite.h"
#include "world_api.h"
#include "shape_player.h"
#include "shape_enemy.h"
#include "world_bg.h"
#include "rng.h"

#include "keyboard.h"

#include "timing.h"

/* ------------------------------------------------------------------ */
/* Entry point                                                          */
/* ------------------------------------------------------------------ */

#define MAX_NUM_ENEMIES 4

main()
{
    int scroll_x = 0;
    int scroll_y = 0;

    int player_wx = 8;   /* player position in world space */
    int player_wy = 8;

    Sprite player;
    Sprite enemies[MAX_NUM_ENEMIES];
    int i;

    sprite_init(&player, player_wx - scroll_x, player_wy - scroll_y, SHAPE_PLAYER_W, SHAPE_PLAYER_H , 0, 0, shape_player);

    for (i = 0; i < MAX_NUM_ENEMIES; i++) {
        sprite_init(&enemies[i],
            rng_next() % (GAME_W - SHAPE_ENEMY_W),
            rng_next() % (GAME_H - SHAPE_ENEMY_H),
            SHAPE_ENEMY_W, SHAPE_ENEMY_H,
            (rng_next() & 1) ? 1 : -1,
            (rng_next() & 1) ? 1 : -1,
            shape_enemy);
    }

    dfile_init();
    world_draw(&world_bg, scroll_x, scroll_y);
    world_cache_bg();

    while (1)
    {

        wait_frame();

        /* HUD: screen coords top-right  "X:nn Y:nn" at cols 24-31 */
        int x=25;
        int y=0;
        dfile_put_char(x++, y, ZX81_INV(CH_M));           /* M */
        dfile_put_char(x++, y, ZX81_INV(CH_A));           /* A */
        dfile_put_char(x++, y, ZX81_INV(CH_P));           /* P */

        x=25;
        y=1;
        dfile_put_num (x, y++, scroll_x / GAME_W);
        dfile_put_num (x, y++, scroll_y / GAME_H);

        //
        //  enemies: erase, move, draw
        //

        for (i = 0; i < MAX_NUM_ENEMIES; i++) {
            if (sprite_is_moving(&enemies[i])) {    
                sprite_erase(&enemies[i], world_bg_scene());
                sprite_move(&enemies[i]);
                if (enemies[i].x > GAME_W - enemies[i].w) { enemies[i].x = GAME_W - enemies[i].w; enemies[i].dx = -1; }
                else if (enemies[i].x < 0)                 { enemies[i].x = 0;                       enemies[i].dx =  1; }
                if (enemies[i].y > GAME_H - enemies[i].h) { enemies[i].y = GAME_H - enemies[i].h;   enemies[i].dy = -1; }
                else if (enemies[i].y < 0)                 { enemies[i].y = 0;                        enemies[i].dy =  1; }
                sprite_draw(&enemies[i]);
            }
        }

        player.dx = 0;
        player.dy = 0;
        if      ((key_pressed(KEY_UP)    || key_pressed(KEY_ALT_UP))    && player_wy > 0)                          player.dy = -1;
        else if ((key_pressed(KEY_DOWN)  || key_pressed(KEY_ALT_DOWN))  && player_wy < WORLD_BG_WORLD_H - player.h)  player.dy =  1;
        if      ((key_pressed(KEY_LEFT)  || key_pressed(KEY_ALT_LEFT))  && player_wx > 0)                          player.dx = -1;
        else if ((key_pressed(KEY_RIGHT) || key_pressed(KEY_ALT_RIGHT)) && player_wx < WORLD_BG_WORLD_W - player.w)  player.dx =  1;
        if (sprite_is_moving(&player)) {
            sprite_erase(&player, world_bg_scene());
            player_wx += player.dx;
            player_wy += player.dy;
            player.x = player_wx - scroll_x;
            player.y = player_wy - scroll_y;

            /* page flip: player crossed a screen edge */
            {
                int flipped = 0;
                if (player.x < 0 && scroll_x >= GAME_W) {
                    scroll_x -= GAME_W;
                    player.x   = GAME_W - player.w;
                    player_wx  = scroll_x + player.x;
                    flipped = 1;
                } else if (player.x + player.w > GAME_W && scroll_x + GAME_W < WORLD_BG_WORLD_W) {
                    scroll_x += GAME_W;
                    player.x   = 0;
                    player_wx  = scroll_x;
                    flipped = 1;
                }
                if (player.y < 0 && scroll_y >= GAME_H) {
                    scroll_y -= GAME_H;
                    player.y   = GAME_H - player.h;
                    player_wy  = scroll_y + player.y;
                    flipped = 1;
                } else if (player.y + player.h > GAME_H && scroll_y + GAME_H < WORLD_BG_WORLD_H) {
                    scroll_y += GAME_H;
                    player.y   = 0;
                    player_wy  = scroll_y;
                    flipped = 1;
                }
                if (flipped) {
                    world_draw(&world_bg, scroll_x, scroll_y);
                    world_cache_bg();
                    for (i = 0; i < MAX_NUM_ENEMIES; i++)
                        sprite_draw(&enemies[i]);
                }
            }

            sprite_draw(&player);
        }

    }
}
