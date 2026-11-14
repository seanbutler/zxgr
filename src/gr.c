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
#include "zx81_semigraphics.h"
#include "sprite.h"
#include "world_api.h"
#include "world_bg.h"
#include "rng.h"

/* D_FILE system variable holds address of the display buffer */
#define D_FILE_SYSVAR 16396

char *dfile;

/* ------------------------------------------------------------------ */
/* Display helpers                                                      */
/* ------------------------------------------------------------------ */

static void init_dfile(void)
{
    dfile = (char *)(*(unsigned int *)D_FILE_SYSVAR) + 1;
}

void put_char(int x, int y, char c)
{
    dfile[y * 33 + x] = c;
}

/* ------------------------------------------------------------------ */
/* World                                                                */
/* ------------------------------------------------------------------ */

static char bg_scene[SCREEN_H * SCREEN_W];

static void fill_bg_scene(void)
{
    int x, y;
    for (y = 0; y < SCREEN_H; y++) {
        const char *src = dfile + y * 33;
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

    sprite_init(&player, 8, 8, 0, 0);

    for (i = 0; i < MAX_NUM_ENEMIES; i++) {
        sprite_init(&enemies[i],
            rng_next() % (SCREEN_W - SPR_W),
            rng_next() % (SCREEN_H - SPR_H),
            (rng_next() & 1) ? 1 : -1,
            (rng_next() & 1) ? 1 : -1);
    }

    init_dfile();
    world_draw(&world_bg, scroll_x, scroll_y);
    fill_bg_scene();

    while (1)
    {
        wait_frame();

        player_erase(&player, bg_scene);
        player.dx = 0;
        player.dy = 0;
        if (key_pressed(KEY_UP)    && player.y > 0)                  player.dy = -1;
        if (key_pressed(KEY_DOWN)  && player.y < SCREEN_H-PLAYER_H)  player.dy =  1;
        if (key_pressed(KEY_LEFT)  && player.x > 0)                  player.dx = -1;
        if (key_pressed(KEY_RIGHT) && player.x < SCREEN_W-PLAYER_W)  player.dx =  1;
        sprite_move(&player);

        for (i = 0; i < MAX_NUM_ENEMIES; i++)
            sprite_erase(&enemies[i], bg_scene);

        for (i = 0; i < MAX_NUM_ENEMIES; i++) {
            sprite_move(&enemies[i]);
            if (enemies[i].x > SCREEN_W - SPR_W) { enemies[i].x = SCREEN_W - SPR_W; enemies[i].dx = -1; }
            if (enemies[i].x < 0)                { enemies[i].x = 0;                enemies[i].dx =  1; }
            if (enemies[i].y > SCREEN_H - SPR_H) { enemies[i].y = SCREEN_H - SPR_H; enemies[i].dy = -1; }
            if (enemies[i].y < 0)                { enemies[i].y = 0;                enemies[i].dy =  1; }
            sprite_draw(&enemies[i]);
        }

        player_draw(&player);
    }
}
