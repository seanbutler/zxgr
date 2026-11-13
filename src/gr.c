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
#include "bg_scene.h"

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

static void fill_bg_scene(int scroll_x, int scroll_y)
{
    int x, y;
    for (y = 0; y < SCREEN_H; y++)
        for (x = 0; x < SCREEN_W; x++)
            bg_scene[y * SCREEN_W + x] = world_char_at(&world_bg, scroll_x + x, scroll_y + y);
}

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

struct enemy {
    int x, y;
    int dx;
};

#define NUM_ENEMIES 10

main()
{
    int scroll_x = 0;
    int scroll_y = 0;
    int px = 14, py = 12;   /* player position (center of screen) */
    struct enemy enemies[NUM_ENEMIES] = {
        {1,  1,   1},
        {2,  2,   1},
        {3,  3,  -1},
        {4,  4,   1},
        {5,  5,  -1},
        {6,  6,  -1},
        {7,  7,  -1},
        {8,  8,   1},
        {9,  9,  -1},
        {10, 10,  1},
    };

    init_dfile();
    world_draw(&world_bg, scroll_x, scroll_y);
    fill_bg_scene(scroll_x, scroll_y);

    while (1)
    {
        int i;
        wait_frame();

        player_erase(px, py, bg_scene);
        for (i = 0; i < NUM_ENEMIES; i++)
            erase_sprite(enemies[i].x, enemies[i].y, bg_scene);

        for (i = 0; i < NUM_ENEMIES; i++)
        {
            enemies[i].x += enemies[i].dx;
            if (enemies[i].x > SCREEN_W - SPR_W) { enemies[i].x = SCREEN_W - SPR_W; enemies[i].dx = -1; }
            if (enemies[i].x < 0)                { enemies[i].x = 0;                enemies[i].dx =  1; }
            draw_sprite(enemies[i].x, enemies[i].y);
        }

        player_draw(px, py);
    }
}
