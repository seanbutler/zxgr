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
#include "shape_exit.h"
#include "exit_map.h"

#include "keyboard.h"

#include "timing.h"

/* ------------------------------------------------------------------ */
/* Entry point                                                          */
/* ------------------------------------------------------------------ */

#define MAX_NUM_ENEMIES 4
#define MAX_NUM_BOONS   4

// #define SHOT_CHAR CH_GREY
#define SHOT_CHAR CH_O

static const char shape_shot[] = { SHOT_CHAR };

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

        screen_rle_draw_centered(title_screen_rle, TITLE_SCREEN_CONTENT_W, TITLE_SCREEN_CONTENT_H);

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

        player_state_reset();

        int scroll_x = 0;
        int scroll_y = 0;

        Sprite player;
        Sprite enemies[MAX_NUM_ENEMIES];
        int enemy_anim[MAX_NUM_ENEMIES];
        Sprite boons[MAX_NUM_BOONS];
        Sprite exit;
        Sprite shot;
        uint8_t i;

        sprite_init(&player, player_state.wx - scroll_x, player_state.wy - scroll_y, SHAPE_PLAYER_W, SHAPE_PLAYER_H , 0, 0, shape_player);
        sprite_init(&exit,
                    EXIT_ENABLED ? EXIT_WX - scroll_x : -SHAPE_EXIT_W,
                    EXIT_ENABLED ? EXIT_WY - scroll_y : -SHAPE_EXIT_H,
                    SHAPE_EXIT_W, SHAPE_EXIT_H, 0, 0, shape_exit);
        sprite_init(&shot, 0, 0, 1, 1, 0, 0, shape_shot);

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
        if (EXIT_ENABLED && exit.x + exit.w > 0 && exit.x < GAME_W && exit.y + exit.h > 0 && exit.y < GAME_H)
            sprite_draw(&exit);


        int frame_counter= 0;
        int hurt_cooldown = 0;
        int restart_requested = 0;
        int action_down = 0;
        int shot_active = 0;
        int shot_wx = 0;
        int shot_wy = 0;
        int shot_dx = 2;
        int shot_dy = 0;
        int last_dx = 1;
        int last_dy = 0;


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
            if (player_state.has_compass) {
                char map_text[] = { ZX81_INV(CH_M), ZX81_INV(CH_A), ZX81_INV(CH_P) };

                dfile_put_zdata(x, y++, map_text, ZSTR_LEN(map_text)+1);

                dfile_put_num (x,   y, scroll_x / GAME_W);
                dfile_put_num (x+3, y, scroll_y / GAME_H);
            } 
            // else 
            // {
            //     int mx;
            //     for (mx = 0; mx < 8; mx++) {
            //         dfile_put_char(x + mx, y, CH_SPACE);
            //         dfile_put_char(x + mx, y + 1, CH_SPACE);
            //     }
            // }

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

            player.dx = (key_pressed(KEY_RIGHT) && player_state.wx < WORLD_BG_WORLD_W - player.w) - (key_pressed(KEY_LEFT)  && player_state.wx > 0);
            player.dy = (key_pressed(KEY_DOWN) && player_state.wy < WORLD_BG_WORLD_H - player.h) - (key_pressed(KEY_UP)    && player_state.wy > 0);

            if (player.dx != 0) {
                last_dx = player.dx;
                last_dy = 0;
            } else if (player.dy != 0) {
                last_dx = 0;
                last_dy = player.dy;
            }

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

            if (key_pressed(KEY_ACTION)) {
                if (!action_down && player_state.has_catapult && player_state.ammo > 0) {
                    player_state.ammo--;
                    shot_active = 1;
                    shot_wx = player_state.wx + (player.w / 2) + last_dx * 4;
                    shot_wy = player_state.wy + (player.h / 2) + last_dy * 4;
                    shot_dx = last_dx * 2;
                    shot_dy = last_dy * 2;
                    sprite_init(&shot, shot_wx - scroll_x, shot_wy - scroll_y, 1, 1, shot_dx, shot_dy, shape_shot);
                }
                action_down = 1;
            } else {
                action_down = 0;
            }

            if (sprite_is_moving(&player))
            {
                sprite_erase(&player, world_bg_scene());
                player_state.wx += player.dx;
                player_state.wy += player.dy;
                player.x = player_state.wx - scroll_x;
                player.y = player_state.wy - scroll_y;

                boon_check_collision(boons, MAX_NUM_BOONS, player.x, player.y, player.w, player.h, scroll_x, scroll_y);

                /* page flip: player crossed a screen edge */
                {
                    int flipped = 0;
                    if (player.x < 0 && scroll_x >= GAME_W) {
                        scroll_x -= GAME_W;
                        player.x   = GAME_W - player.w;
                        player_state.wx = scroll_x + player.x;
                        flipped = 1;
                    } else if (player.x + player.w > GAME_W && scroll_x + GAME_W < WORLD_BG_WORLD_W) {
                        scroll_x += GAME_W;
                        player.x   = 0;
                        player_state.wx = scroll_x;
                        flipped = 1;
                    }

                    if (player.y < 0 && scroll_y >= GAME_H) {
                        scroll_y -= GAME_H;
                        player.y   = GAME_H - player.h;
                        player_state.wy = scroll_y + player.y;
                        flipped = 1;
                    } else if (player.y + player.h > GAME_H && scroll_y + GAME_H < WORLD_BG_WORLD_H) {
                        scroll_y += GAME_H;
                        player.y   = 0;
                        player_state.wy = scroll_y;
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
                        if (EXIT_ENABLED) {
                            exit.x = EXIT_WX - scroll_x;
                            exit.y = EXIT_WY - scroll_y;
                        }
                        if (EXIT_ENABLED && exit.x + exit.w > 0 && exit.x < GAME_W && exit.y + exit.h > 0 && exit.y < GAME_H)
                            sprite_draw(&exit);
                    }
                }

                sprite_draw(&player);
            }

            if (EXIT_ENABLED &&
                player_state.wx + player.w > EXIT_WX && player_state.wx < EXIT_WX + SHAPE_EXIT_W &&
                player_state.wy + player.h > EXIT_WY && player_state.wy < EXIT_WY + SHAPE_EXIT_H) {
                char win_text[] = {
                    ZX81_INV(CH_Y), ZX81_INV(CH_O), ZX81_INV(CH_U), ZX81_INV(CH_SPACE),
                    ZX81_INV(CH_W), ZX81_INV(CH_I), ZX81_INV(CH_N)
                };
                char win_restart_text[] = {
                    ZX81_INV(CH_P), ZX81_INV(CH_R), ZX81_INV(CH_E), ZX81_INV(CH_S), ZX81_INV(CH_S),
                    ZX81_INV(CH_SPACE), ZX81_INV(CH_L)
                };

                dfile_init();
                dfile_clear(CH_GREY);
                dfile_put_zdata((32 - ZSTR_LEN(win_text))/2, 11, win_text, ZSTR_LEN(win_text)+1);
                dfile_put_zdata((32 - ZSTR_LEN(win_restart_text))/2, 22, win_restart_text, ZSTR_LEN(win_restart_text)+1);

                while (1) {
                    wait_frame();
                    if (key_pressed(KEY_L)) {
                        restart_requested = 1;
                        break;
                    }
                }
                break;
            }

            if (shot_active) {
                sprite_erase(&shot, world_bg_scene());
                shot_wx += shot.dx;
                shot_wy += shot.dy;

                if (shot_wx < 0 || shot_wy < 0 ||
                    shot_wx >= WORLD_BG_WORLD_W || shot_wy >= WORLD_BG_WORLD_H) {
                    shot_active = 0;
                } else if (world_char_at(&world_bg, shot_wx, shot_wy) != 0x00) {
                    shot_active = 0;
                } else {
                    shot.x = shot_wx - scroll_x;
                    shot.y = shot_wy - scroll_y;
                    if (enemy_check_projectile(enemies, MAX_NUM_ENEMIES, &shot, world_bg_scene()))
                        shot_active = 0;
                }

                if (shot_active) {
                    shot.x = shot_wx - scroll_x;
                    shot.y = shot_wy - scroll_y;
                    if (shot.x >= 0 && shot.x < GAME_W && shot.y >= 0 && shot.y < GAME_H)
                        sprite_draw(&shot);
                }
            }

            if (hurt_cooldown == 0) {
                if (enemy_check_player(enemies, MAX_NUM_ENEMIES, &player)) {
                    if (player_state.health > 0) player_state.health--;
                    hurt_cooldown = 25;
                    if (player_state.health <= 0) {
                        char restart_text[] = {
                            ZX81_INV(CH_P), ZX81_INV(CH_R), ZX81_INV(CH_E), ZX81_INV(CH_S), ZX81_INV(CH_S),
                            ZX81_INV(CH_SPACE), ZX81_INV(CH_L)
                        };

                        dfile_init();
                        screen_rle_draw_centered(game_over_rle, GAME_OVER_CONTENT_W, GAME_OVER_CONTENT_H);
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
                }
            }

            if (restart_requested) break;

            for (i = 0; i < MAX_NUM_BOONS; i++) {
                if (boons[i].data) sprite_draw(&boons[i]);
            }
            if (EXIT_ENABLED && exit.x + exit.w > 0 && exit.x < GAME_W && exit.y + exit.h > 0 && exit.y < GAME_H)
                sprite_draw(&exit);
        }

    }
}
