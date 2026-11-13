# ZX81 2-Layer Graphics Demo

A 2-layer graphics demo for the ZX81, built with [z88dk](https://github.com/z88dk/z88dk).

## Layers

- **Layer 1 (background):** Tile-based scrolling world rendered into the D_FILE character grid.
- **Layer 2 (foreground):** Player and 10 bouncing enemy sprites drawn over the background using ZX81 semigraphics characters.

## Display

The ZX81 D_FILE is a 32×24 character grid. Each row is 33 bytes: 32 character codes followed by a `0x76` (NEWLINE). The demo accesses it directly via the `D_FILE` system variable at address `16396`.

Frame sync spins on the hardware `FRAMES` counter at `0x4034` (50 Hz) to pace animation.

## Project structure

```
src/
  gr.c                 main loop, D_FILE helpers, scroll/erase/draw orchestration
  sprite.c/h           draw_sprite, erase_sprite, player_draw, player_erase
  world.c              world_draw, world_char_at
  world_api.h          World struct (tile palette + map triples)
  world_bg.h           world tile map data
  bg_scene.h           background scene tile data
  seiklus_bg.h         Seiklus-style background tiles
  zx81_semigraphics.h  semigraphics character helpers
build/
  gr                   raw binary
  gr.P                 ZX81 tape file (emulator input)
tools/                 asset conversion utilities
```

## Requirements

- [z88dk](https://github.com/z88dk/z88dk) installed at `~/Apps/z88dk`
- [ZEsarUX](https://github.com/chernandezba/zesarux) 13.0 installed at `~/Apps/ZEsarUX-13.0/zesarux`

## Build & run

```sh
make        # compiles to gr.P
make run    # launches gr.P in ZEsarUX
make clean  # removes build artifacts
```
