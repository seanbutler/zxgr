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
    int enemy_anim[MAX_NUM_ENEMIES];
    uint8_t i;

    sprite_init(&player, player_wx - scroll_x, player_wy - scroll_y, SHAPE_PLAYER_W, SHAPE_PLAYER_H , 0, 0, shape_player);

    for (i = 0; i < MAX_NUM_ENEMIES; i++) {
        enemy_anim[i] = 0;
        sprite_init(&enemies[i],
            rng_next() % (GAME_W - SHAPE_ENEMY_W),
            rng_next() % (GAME_H - SHAPE_ENEMY_H),
            SHAPE_ENEMY_W, SHAPE_ENEMY_H,
            (rng_next() & 1) ? 1 : -1,
            (rng_next() & 1) ? 1 : -1,
            shape_enemy_frames[0]);
    }

    dfile_init();
    world_draw(&world_bg, scroll_x, scroll_y);
    world_cache_bg();
    sprite_draw(&player);


    int frame_counter= 0;


    while (1)
    {
        frame_counter++;

        wait_frame();

        /* HUD: screen coords top-right */
        int x=24;
        int y=0;

        /* HEALTH UNDERNEATH */

        char health_text[] = {
            ZX81_INV(CH_H), ZX81_INV(CH_E), ZX81_INV(CH_A), ZX81_INV(CH_L), ZX81_INV(CH_T), ZX81_INV(CH_H)
        };

        /* MAP UNDERNEATH */
        char map_text[] = { ZX81_INV(CH_M), ZX81_INV(CH_A), ZX81_INV(CH_P) };

        dfile_put_zdata(x, y++, health_text, ZSTR_LEN(health_text));
        dfile_put_zdata(x, y++, map_text, ZSTR_LEN(map_text));

        dfile_put_num (x, y++, scroll_x / GAME_W);
        dfile_put_num (x, y++, scroll_y / GAME_H);

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
                    int r, c, tries, ok, ex, ey;
                    const char *bg;
                    world_draw(&world_bg, scroll_x, scroll_y);
                    world_cache_bg();
                    bg = world_bg_scene();
                    for (i = 0; i < MAX_NUM_ENEMIES; i++) {
                        Sprite *e = &enemies[i];
                        ok = 0;
                        for (tries = 0; tries < 64 && !ok; tries++) {
                            ex = rng_next() % (GAME_W - e->w);
                            ey = rng_next() % (GAME_H - e->h);
                            ok = 1;
                            for (r = 0; r < e->h && ok; r++)
                                for (c = 0; c < e->w && ok; c++)
                                    if (bg[(ey + r) * GAME_W + (ex + c)])
                                        ok = 0;
                        }
                        if (ok) {
                            e->x = ex;
                            e->y = ey;
                        }
                    }
                }
            }

            sprite_draw(&player);
        }

    }
}
