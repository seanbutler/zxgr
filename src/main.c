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

/* ------------------------------------------------------------------ */
/* World                                                                */
/* ------------------------------------------------------------------ */

static char bg_scene[SCREEN_H * SCREEN_W];

static void fill_bg_scene(void)
{
    int x, y;
    for (y = 0; y < SCREEN_H; y++) {
        const char *src = dfile + y * DFILE_STRIDE;
        char *dst = bg_scene + y * SCREEN_W;
        for (x = 0; x < SCREEN_W; x++)
            dst[x] = src[x];
    }
}

/* ------------------------------------------------------------------ */
/* Keyboard                                                             */
/* ------------------------------------------------------------------ */

static unsigned char read_port(unsigned int port)
{
#asm
    ld b, h
    ld c, l
    in a, (c)
    ld l, a
    ld h, 0
#endasm
}

#define key_pressed(k)  (k)
#define KEY_UP    (!(read_port(0xFBFE) & 0x01))  /* Q */
#define KEY_DOWN  (!(read_port(0xFDFE) & 0x01))  /* A */
#define KEY_LEFT  (!(read_port(0xDFFE) & 0x02))  /* O */
#define KEY_RIGHT (!(read_port(0xDFFE) & 0x01))  /* P */

/* ------------------------------------------------------------------ */
/* Frame sync: spin until the FRAMES counter (50 Hz) ticks             */
/* ------------------------------------------------------------------ */

static void wait_frame(void)
{
#asm
    ld hl,0x4034
wf_read:
    ld a,(hl)
wf_spin:
    cp (hl)
    jr z,wf_spin
#endasm
}

/* ------------------------------------------------------------------ */
/* Entry point                                                          */
/* ------------------------------------------------------------------ */

#define MAX_NUM_ENEMIES 4

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
    fill_bg_scene();

    while (1)
    {
        wait_frame();

        sprite_erase(&player, bg_scene);

        for (i = 0; i < MAX_NUM_ENEMIES; i++)
        {
            sprite_erase(&enemies[i], bg_scene);
        }


        if (key_pressed(KEY_UP)    && player.y > 0)
        {
            player.dy = -1;
        }
        else if (key_pressed(KEY_DOWN)  && player.y < SCREEN_H-player.h)
        {
            player.dy =  1;
        }   
        else 
        {
            player.dy = 0;
        }   

        if (key_pressed(KEY_LEFT)  && player.x > 0)
        {
            player.dx = -1; 
        }
        else if (key_pressed(KEY_RIGHT) && player.x < SCREEN_W-player.w)
        {
            player.dx =  1;
        }
        else 
        {
            player.dx = 0;
        }

        sprite_move(&player);


        //
        // Handle the Enemy Sprites
        //

        for (i = 0; i < MAX_NUM_ENEMIES; i++) {
            
            sprite_move(&enemies[i]);

            if (enemies[i].x > SCREEN_W - enemies[i].w) 
            { 
                enemies[i].x = SCREEN_W - enemies[i].w;
                enemies[i].dx = -1; 
            }
            else if (enemies[i].x < 0)
            { 
                enemies[i].x = 0;
                enemies[i].dx =  1;
            }

            if (enemies[i].y > SCREEN_H - enemies[i].h) 
            { 
                enemies[i].y = SCREEN_H - enemies[i].h;  
                enemies[i].dy = -1; 
            }
            
            if (enemies[i].y < 0) 
            { 
                enemies[i].y = 0;                        
                enemies[i].dy =  1; 
            }

            sprite_draw(&enemies[i]);
        }




        sprite_draw(&player);
    }
}
