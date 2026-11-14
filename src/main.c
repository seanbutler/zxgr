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
#include "world_bg.h"
#include "rng.h"

#include "keyboard.h"

#include "timing.h"

/* ------------------------------------------------------------------ */
/* Entry point                                                          */
/* ------------------------------------------------------------------ */

#define MAX_NUM_ENEMIES 2

main()
{
    int scroll_x = 0;
    int scroll_y = 0;

    Sprite player;
    Sprite enemies[MAX_NUM_ENEMIES];
    int i;

    sprite_init(&player, 8, 8, 4, 3, 0, 0, shape_player);

    for (i = 0; i < MAX_NUM_ENEMIES; i++) {
        sprite_init(&enemies[i],
            rng_next() % (SCREEN_W - 2),
            rng_next() % (SCREEN_H - 2),
            2, 2,
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

        //
        //  enemies: erase, move, draw
        //

        for (i = 0; i < MAX_NUM_ENEMIES; i++) {
            sprite_erase(&enemies[i], world_bg_scene());
            sprite_move(&enemies[i]);
    
            if (enemies[i].x > SCREEN_W - enemies[i].w) { enemies[i].x = SCREEN_W - enemies[i].w; enemies[i].dx = -1; }
            else if (enemies[i].x < 0)                  { enemies[i].x = 0;                        enemies[i].dx =  1; }
            if (enemies[i].y > SCREEN_H - enemies[i].h) { enemies[i].y = SCREEN_H - enemies[i].h;  enemies[i].dy = -1; }
            else if (enemies[i].y < 0)                  { enemies[i].y = 0;                         enemies[i].dy =  1; }
            sprite_draw(&enemies[i]);
        }

        //
        // player: erase, move, draw — kept tight to minimise flicker
        //
        
        sprite_erase(&player, world_bg_scene());
        player.dx = 0;
        player.dy = 0;
        if      (key_pressed(KEY_UP)    && player.y > 0)                 player.dy = -1;
        else if (key_pressed(KEY_DOWN)  && player.y < SCREEN_H-player.h) player.dy =  1;
        if      (key_pressed(KEY_LEFT)  && player.x > 0)                 player.dx = -1;
        else if (key_pressed(KEY_RIGHT) && player.x < SCREEN_W-player.w) player.dx =  1;
        sprite_move(&player);
        sprite_draw(&player);

    }
}
