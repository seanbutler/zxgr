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

#include <stdint.h>

#include <zx81.h>
#include "dfile.h"
#include "zx81_semigraphics.h"
#include "keyboard.h"
#include "sprite.h"
#include "world_api.h"
#include "shape_player.h"
#include "shape_enemy.h"
#include "world_bg.h"
#include "rng.h"
#include "boon.h"
#include "enemy_map.h"
#include "title_screen.h"
#include "game_over.h"
#include "player_state.h"

#include "keyboard.h"

#include "timing.h"

/* ------------------------------------------------------------------ */
/* Entry point                                                          */
/* ------------------------------------------------------------------ */

#define MAX_NUM_ENEMIES 4
#define MAX_NUM_BOONS   4

main()
{


    
    while (1)
    {

    //
    // SPLASH SCREEN HERE
    //
    
    int x = 12;
    int y = 0;



    dfile_init();
    
    dfile_clear(CH_FULL);

    title_screen_draw();


    char instr_text[] = {
        ZX81_INV(CH_Z), ZX81_INV(CH_X), ZX81_INV(CH_PLUS), ZX81_INV(CH_K), ZX81_INV(CH_M), 
        ZX81_INV(CH_SPACE), ZX81_INV(CH_2), ZX81_INV(CH_SPACE),  
        ZX81_INV(CH_M), ZX81_INV(CH_O), ZX81_INV(CH_V), ZX81_INV(CH_E)
    };

    char go_text[] = {
        ZX81_INV(CH_P), ZX81_INV(CH_R), ZX81_INV(CH_E), ZX81_INV(CH_S), ZX81_INV(CH_S), ZX81_INV(CH_SPACE), ZX81_INV(CH_L),
        ZX81_INV(CH_SPACE), ZX81_INV(CH_T), ZX81_INV(CH_O), ZX81_INV(CH_SPACE), ZX81_INV(CH_S), ZX81_INV(CH_T), ZX81_INV(CH_A), ZX81_INV(CH_R), ZX81_INV(CH_T)
    };

    dfile_put_zdata((32 - ZSTR_LEN(instr_text))/2, 0, instr_text, ZSTR_LEN(instr_text)+1);
    dfile_put_zdata((32 - ZSTR_LEN(go_text))/2, 23, go_text, ZSTR_LEN(go_text)+1);

    int go_time = 0;
    while(! go_time)
    {
        wait_frame();
        go_time = (key_pressed(KEY_L) );
    }


    dfile_clear(CH_GREY);

    //
    // GAMEPLAY HERE
    //

    player_state.health = 3;
    player_state.ammo = 6;
    player_state.has_compass = 0;

    int scroll_x = 0;
    int scroll_y = 0;

    int player_wx = 8;   /* player position in world space */
    int player_wy = 8;

    Sprite player;
    Sprite enemies[MAX_NUM_ENEMIES];
    int enemy_anim[MAX_NUM_ENEMIES];
    Sprite boons[MAX_NUM_BOONS];
    uint8_t i;

    sprite_init(&player, player_wx - scroll_x, player_wy - scroll_y, SHAPE_PLAYER_W, SHAPE_PLAYER_H , 0, 0, shape_player);

    enemies_load_screen(enemies, enemy_anim, MAX_NUM_ENEMIES, scroll_x, scroll_y);
    boons_load_screen(boons, MAX_NUM_BOONS, scroll_x, scroll_y);

    dfile_init();
    world_draw(&world_bg, scroll_x, scroll_y);
    world_cache_bg();
    for (i = 0; i < MAX_NUM_BOONS; i++) {
        if (boons[i].dx == 0 && boons[i].dy == 0 && boons[i].data) {
            sprite_draw(&boons[i]);
        }
    }
    sprite_draw(&player);


    int frame_counter= 0;
    int hurt_cooldown = 0;
    int restart_requested = 0;


    while (1)
    {
        frame_counter++;
        if (hurt_cooldown > 0) hurt_cooldown--;

        wait_frame();

        /* HUD: screen coords top-right */
        int x=24;
        int y=0;

        /* HEALTH UNDERNEATH */

        char health_text[] = {
            ZX81_INV(CH_H), ZX81_INV(CH_E), ZX81_INV(CH_A), ZX81_INV(CH_L), ZX81_INV(CH_T), ZX81_INV(CH_H)
        };


        dfile_put_zdata(x, y++, health_text, ZSTR_LEN(health_text)+1);
        dfile_put_num (x, y++, player_state.health);

        y++;


        /* AMMO UNDERNEATH */
        char ammo_text[] = { ZX81_INV(CH_A), ZX81_INV(CH_M), ZX81_INV(CH_M), ZX81_INV(CH_O) };

        dfile_put_zdata(x, y++, ammo_text, ZSTR_LEN(ammo_text)+1);
        dfile_put_num (x, y++, player_state.ammo);


    y++;

        y++;

        /* MAP UNDERNEATH */
        char map_text[] = { ZX81_INV(CH_M), ZX81_INV(CH_A), ZX81_INV(CH_P) };

        dfile_put_zdata(x, y++, map_text, ZSTR_LEN(map_text)+1);

        dfile_put_num (x,   y, scroll_x / GAME_W);
        dfile_put_num (x+3, y, scroll_y / GAME_H);

        y++;
        y++;



        //
        //  enemies: erase, move, draw
        //

        {
            int ei = frame_counter % MAX_NUM_ENEMIES;
            Sprite *e = &enemies[ei];
            if (sprite_is_moving(e)) {
                const char *bg = world_bg_scene();
                int nx, ny, cx, cy, r, c;
                sprite_erase(e, bg);

                nx = e->x + e->dx;
                ny = e->y + e->dy;

                if (nx < 0 || nx + e->w > GAME_W) {
                    e->dx = -e->dx;
                } else {
                    cx = (e->dx > 0) ? nx + e->w - 1 : nx;
                    for (r = 0; r < e->h; r++) {
                        int sy = e->y + r;
                        if (sy >= 0 && sy < GAME_H && bg[sy * GAME_W + cx]) {
                            e->dx = -e->dx;
                            break;
                        }
                    }
                }

                if (ny < 0 || ny + e->h > GAME_H) {
                    e->dy = -e->dy;
                } else {
                    cy = (e->dy > 0) ? ny + e->h - 1 : ny;
                    for (c = 0; c < e->w; c++) {
                        int sx = e->x + c;
                        if (sx >= 0 && sx < GAME_W && bg[cy * GAME_W + sx]) {
                            e->dy = -e->dy;
                            break;
                        }
                    }
                }

                sprite_move(e);
                enemy_anim[ei]++;
                e->data = shape_enemy_frames[enemy_anim[ei] & 1];
                sprite_draw(e);
            }
        }

        player.dx = (key_pressed(KEY_RIGHT) && player_wx < WORLD_BG_WORLD_W - player.w) - (key_pressed(KEY_LEFT)  && player_wx > 0);
        player.dy = (key_pressed(KEY_DOWN) && player_wy < WORLD_BG_WORLD_H - player.h) - (key_pressed(KEY_UP)    && player_wy > 0);

        /* environment collision — only test positions inside the current screen */
        {
            const char *bg = world_bg_scene();
            int nx, ny, r, c;
            if (player.dx != 0) {
                nx = player.x + player.dx;
                if (nx >= 0 && nx + player.w <= GAME_W) {
                    int cx = (player.dx > 0) ? nx + player.w - 1 : nx;
                    for (r = 0; r < player.h; r++) {
                        int sy = player.y + r;
                        if (sy >= 0 && sy < GAME_H && bg[sy * GAME_W + cx]) {
                            player.dx = 0;
                            break;
                        }
                    }
                }
            }
            if (player.dy != 0) {
                ny = player.y + player.dy;
                if (ny >= 0 && ny + player.h <= GAME_H) {
                    int cy = (player.dy > 0) ? ny + player.h - 1 : ny;
                    for (c = 0; c < player.w; c++) {
                        int sx = player.x + c;
                        if (sx >= 0 && sx < GAME_W && bg[cy * GAME_W + sx]) {
                            player.dy = 0;
                            break;
                        }
                    }
                }
            }
        }

        if (sprite_is_moving(&player))
        {
            sprite_erase(&player, world_bg_scene());
            player_wx += player.dx;
            player_wy += player.dy;
            player.x = player_wx - scroll_x;
            player.y = player_wy - scroll_y;

            boon_check_collision(boons, MAX_NUM_BOONS, player.x, player.y, player.w, player.h, scroll_x, scroll_y);

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
                    boons_load_screen(boons, MAX_NUM_BOONS, scroll_x, scroll_y);
                    for (i = 0; i < MAX_NUM_BOONS; i++) {
                        if (boons[i].data) {
                            sprite_draw(&boons[i]);
                        }
                    }
                    enemies_load_screen(enemies, enemy_anim, MAX_NUM_ENEMIES, scroll_x, scroll_y);
                }
            }

            sprite_draw(&player);
        }

        if (hurt_cooldown == 0) {
            for (i = 0; i < MAX_NUM_ENEMIES; i++) {
                Sprite *e = &enemies[i];
                if (!sprite_is_moving(e)) continue;

                if (player.x < e->x + e->w && player.x + player.w > e->x &&
                    player.y < e->y + e->h && player.y + player.h > e->y) {
                    if (player_state.health > 0) player_state.health--;
                    hurt_cooldown = 25;
                    if (player_state.health <= 0) {
                        char restart_text[] = {
                            ZX81_INV(CH_P), ZX81_INV(CH_R), ZX81_INV(CH_E), ZX81_INV(CH_S), ZX81_INV(CH_S),
                            ZX81_INV(CH_SPACE), ZX81_INV(CH_L)
                        };

                        dfile_init();
                        game_over_draw();
                        dfile_put_zdata((32 - ZSTR_LEN(restart_text))/2, 22, restart_text, ZSTR_LEN(restart_text)+1);

                        while (1) {
                            wait_frame();
                            if (key_pressed(KEY_L)) {
                                restart_requested = 1;
                                break;
                            }
                        }
                        break;
                    }
                    break;
                }
            }
        }

        if (restart_requested) {
            break;
        }

        for (i = 0; i < MAX_NUM_BOONS; i++) {
            if (boons[i].data) {
                sprite_draw(&boons[i]);
            }
        }

    }

    }
}
