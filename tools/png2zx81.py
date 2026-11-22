#!/usr/bin/env python3
"""
png2zx81.py — Convert PNG to a ZX81 C header using 4x4 sparse tile compression.

Pipeline:
  1. Load PNG, convert to greyscale — 1 source pixel = 1 character cell (both axes)
  2. Posterise each character cell to a ZX81 char code
  3. Group into 4x4 character tiles
  4. Build a palette of unique non-default tiles
  5. Emit a sparse world map: (tx, ty, tile_idx) triples, 0xFF-terminated

Default tile (all 0x00 / white) is implicit — not stored in the world map.
By default world size = source image size in pixels (1px = 1 char cell).
Override with --width N or --height N.

ZX81 grey palette used (verified from ROM at 0x1E00):
  0x00  =   0% fill  white
  0x08  =  50% fill  grey  (fine pixel checkerboard — best TV grey)
  0x80  = 100% fill  black

Posterisation thresholds (brightness: 1.0=white, 0.0=black):
  >= T_WHITE  ->  0x00
  >= T_BLACK  ->  0x08
  <  T_BLACK  ->  0x80

Saves a 256x192 preview PNG alongside the .h file.

Usage:
    python3 png2zx81.py <input.png> <output.h> [<varname>] [--width N] [--height N]
                       [--boon-map path/to/boon_map.c] [--enemy-map path/to/enemy_map.c]
                       [--exit-map path/to/exit_map.h]
"""

import sys
import os

try:
    from PIL import Image
except ImportError:
    sys.exit("Pillow required: pip install pillow")

# ---------------------------------------------------------------------------
# Posterisation thresholds
# ---------------------------------------------------------------------------
T_WHITE = 0.84   # >= white
T_BLACK = 0.62   # >= grey, < black

# ---------------------------------------------------------------------------
# Tile geometry
# ---------------------------------------------------------------------------
TILE_W  = 4    # chars per tile, horizontal
TILE_H  = 4    # chars per tile, vertical
DEFAULT = 0x00 # char code for the implicit background tile

# Marker colours in source map PNG.
# These are gameplay entities, not background tiles.
AMMO_MARKER_COLORS = ((255, 0, 0),)
ENEMY_MARKER_COLORS = ((255, 128, 0), (255, 165, 0))
HEALTH_MARKER_COLORS = ((0, 255, 0),)
COMPASS_MARKER_COLORS = ((255, 255, 0),)
CATAPULT_MARKER_COLORS = ((0, 0, 255),)
EXIT_MARKER_COLORS = ((255, 0, 255),)
MARKER_TOLERANCE = 48

# ---------------------------------------------------------------------------
# ZX81 character pixel patterns — 8x8 grid, True = black ink
# ---------------------------------------------------------------------------
def _char_grid(code):
    base = code & 0x7F
    inv  = bool(code & 0x80)
    g = [[False] * 8 for _ in range(8)]

    if base == 0x01:        # TL quarter
        for r in range(4):
            for c in range(4): g[r][c] = True
    elif base == 0x02:      # TR quarter
        for r in range(4):
            for c in range(4, 8): g[r][c] = True
    elif base == 0x03:      # top half
        for r in range(4):
            for c in range(8): g[r][c] = True
    elif base == 0x04:      # BL quarter
        for r in range(4, 8):
            for c in range(4): g[r][c] = True
    elif base == 0x05:      # left half
        for r in range(8):
            for c in range(4): g[r][c] = True
    elif base == 0x06:      # TR+BL diagonal
        for r in range(4):
            for c in range(4, 8): g[r][c] = True
        for r in range(4, 8):
            for c in range(4): g[r][c] = True
    elif base == 0x07:      # TL+TR+BL (3/4 fill, BR white)
        for r in range(4):
            for c in range(8): g[r][c] = True
        for r in range(4, 8):
            for c in range(4): g[r][c] = True
    elif base == 0x08:      # fine checkerboard — TRUE GREY
        for r in range(8):
            for c in range(8):
                if (r + c) % 2 == 0: g[r][c] = True
    elif base == 0x09:      # bottom-half checkerboard
        for r in range(4, 8):
            for c in range(8):
                if (r + c) % 2 == 0: g[r][c] = True
    elif base == 0x0A:      # top-half checkerboard
        for r in range(4):
            for c in range(8):
                if (r + c) % 2 == 0: g[r][c] = True

    if inv:
        g = [[not v for v in row] for row in g]
    return g


# ---------------------------------------------------------------------------
# Posterisation
# ---------------------------------------------------------------------------
def _pick_char(brightness):
    if brightness >= T_WHITE: return 0x00
    if brightness >= T_BLACK: return 0x08
    return 0x80


def _is_close_rgb(rgb, target, tolerance):
    return (abs(rgb[0] - target[0]) <= tolerance and
            abs(rgb[1] - target[1]) <= tolerance and
            abs(rgb[2] - target[2]) <= tolerance)


def _marker_type(rgb):
    for c in AMMO_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'BOON_AMMO'
    for c in ENEMY_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'ENEMY_BAT'
    for c in HEALTH_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'BOON_HEALTH'
    for c in COMPASS_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'BOON_COMPASS'
    for c in CATAPULT_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'BOON_CATAPULT'
    for c in EXIT_MARKER_COLORS:
        if _is_close_rgb(rgb, c, MARKER_TOLERANCE):
            return 'EXIT'
    return None


def convert(input_path, world_w, world_h):
    src = Image.open(input_path)
    img_l = src.convert('L').resize((world_w, world_h), Image.LANCZOS)
    img_c = src.convert('RGB').resize((world_w, world_h), Image.NEAREST)
    px_l = img_l.load()
    px_c = img_c.load()

    boons = []
    enemies = []
    exits = []
    chars = []

    for y in range(world_h):
        row = []
        for x in range(world_w):
            marker = _marker_type(px_c[x, y])
            if marker == 'BOON_AMMO' or marker == 'BOON_HEALTH' or marker == 'BOON_COMPASS' or marker == 'BOON_CATAPULT':
                boons.append((x, y, marker))
                row.append(DEFAULT)
            elif marker == 'ENEMY_BAT':
                enemies.append((x, y, marker))
                row.append(DEFAULT)
            elif marker == 'EXIT':
                exits.append((x, y))
                row.append(DEFAULT)
            else:
                row.append(_pick_char(px_l[x, y] / 255.0))
        chars.append(row)

    return chars, boons, enemies, exits


# ---------------------------------------------------------------------------
# Tile extraction
# ---------------------------------------------------------------------------
def _get_tile(chars, tx, ty, world_w, world_h):
    flat = []
    for r in range(TILE_H):
        for c in range(TILE_W):
            y = ty * TILE_H + r
            x = tx * TILE_W + c
            flat.append(chars[y][x] if x < world_w and y < world_h else DEFAULT)
    return tuple(flat)


def extract_tiles(chars, world_w, world_h):
    tiles_x = (world_w + TILE_W - 1) // TILE_W
    tiles_y = (world_h + TILE_H - 1) // TILE_H

    palette   = []          # list of flat tuples (TILE_W*TILE_H chars)
    lookup    = {}          # tuple -> palette index
    world_map = []          # (tx, ty, tile_idx)

    default_tile = (DEFAULT,) * (TILE_W * TILE_H)

    for ty in range(tiles_y):
        for tx in range(tiles_x):
            tile = _get_tile(chars, tx, ty, world_w, world_h)
            if tile == default_tile:
                continue
            if tile not in lookup:
                lookup[tile] = len(palette)
                palette.append(tile)
            world_map.append((tx, ty, lookup[tile]))

    usage_counts = [0] * len(palette)
    usage_locations = [[] for _ in palette]
    for tx, ty, ti in world_map:
        usage_counts[ti] += 1
        usage_locations[ti].append((tx, ty))

    return palette, world_map, usage_counts, usage_locations


# ---------------------------------------------------------------------------
# Preview PNG
# ---------------------------------------------------------------------------
def save_preview(chars, usage_counts, usage_locations, output_path, world_w, world_h):
    preview = Image.new('RGB', (world_w * 8, world_h * 8), (255, 255, 255))
    px = preview.load()

    for y, row in enumerate(chars):
        for x, code in enumerate(row):
            grid = _char_grid(code)
            ox, oy = x * 8, y * 8
            for r in range(8):
                for c in range(8):
                    px[ox + c, oy + r] = (0, 0, 0) if grid[r][c] else (255, 255, 255)

    # Yellow outlines mark tiles that are used only once (artist one-off candidates).
    tile_px = TILE_W * 8
    for ti, count in enumerate(usage_counts):
        if count != 1:
            continue
        tx, ty = usage_locations[ti][0]
        x0 = tx * tile_px
        y0 = ty * tile_px
        x1 = min(x0 + tile_px - 1, world_w * 8 - 1)
        y1 = min(y0 + tile_px - 1, world_h * 8 - 1)

        for x in range(x0, x1 + 1):
            px[x, y0] = (255, 215, 0)
            px[x, min(y0 + 1, y1)] = (255, 215, 0)
            px[x, y1] = (255, 215, 0)
            px[x, max(y1 - 1, y0)] = (255, 215, 0)
        for y in range(y0, y1 + 1):
            px[x0, y] = (255, 215, 0)
            px[min(x0 + 1, x1), y] = (255, 215, 0)
            px[x1, y] = (255, 215, 0)
            px[max(x1 - 1, x0), y] = (255, 215, 0)

    preview.save(output_path)
    print(f'Preview  {output_path}  ({world_w * 8}x{world_h * 8}px, single-use tiles in yellow)')


def save_tile_sheet(palette, usage_counts, usage_locations, output_path):
    from PIL import ImageDraw

    tile_px_w = TILE_W * 8
    tile_px_h = TILE_H * 8
    label_x = tile_px_w + 8
    sheet_px_w = 160
    img = Image.new('RGB', (sheet_px_w, tile_px_h * len(palette)), (255, 255, 255))
    px = img.load()
    draw = ImageDraw.Draw(img)

    for i, tile in enumerate(palette):
        oy = i * tile_px_h
        draw.line((tile_px_w + 3, oy, tile_px_w + 3, oy + tile_px_h - 1), fill=(180, 180, 180))

        for r in range(TILE_H):
            for c in range(TILE_W):
                code = tile[r * TILE_W + c]
                grid = _char_grid(code)
                ox = c * 8
                for gr in range(8):
                    for gc in range(8):
                        px[ox + gc, oy + r * 8 + gr] = (0, 0, 0) if grid[gr][gc] else (255, 255, 255)

        count = usage_counts[i]
        draw.text((label_x, oy + 2), f'Tile {i}', fill=(20, 20, 20))
        draw.text((label_x, oy + 11), f'Used {count}x', fill=(20, 20, 20))

        if count == 1:
            draw.text((label_x, oy + 20), 'FLAG-ONE-OFF', fill=(220, 40, 40))
            tx, ty = usage_locations[i][0]
            draw.text((label_x, oy + 29), f'at tx={tx}, ty={ty}', fill=(30, 90, 180))

    img.save(output_path)
    print(f'Tiles    {output_path}  ({len(palette)} tiles, {sheet_px_w}x{tile_px_h * len(palette)}px)')


def write_boon_map(boons, output_path, input_path):
    with open(output_path, 'w') as f:
        f.write('#include "boon.h"\n\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write('Boon boon_map[] = {\n')
        for wx, wy, boon_type in boons:
            f.write(f'    {{ {wx:3d}, {wy:3d}, {boon_type:12s} }},\n')
        f.write('    {   0,   0, BOON_NONE     },  /* sentinel */\n')
        f.write('};\n')
    print(f'Boons    {output_path}  ({len(boons)} entries)')


def write_enemy_map(enemies, output_path, input_path):
    with open(output_path, 'w') as f:
        f.write('#include "enemy_map.h"\n\n')
        f.write('#include "shape_enemy.h"\n')
        f.write('#include "world_api.h"\n')
        f.write('#include "rng.h"\n\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write('const EnemySpawn enemy_map[] = {\n')
        for wx, wy, enemy_type in enemies:
            f.write(f'    {{ {wx:3d}, {wy:3d}, {enemy_type:9s} }},\n')
        f.write('    {   0,   0, ENEMY_NONE },  /* sentinel */\n')
        f.write('};\n\n')
        f.write('void enemies_load_screen(Sprite *enemies, int *anim, int max, int scroll_x, int scroll_y)\n')
        f.write('{\n')
        f.write('    const EnemySpawn *sp;\n')
        f.write('    int count, i;\n\n')
        f.write('    for (i = 0; i < max; i++) {\n')
        f.write('        enemies[i].dx = 0;\n')
        f.write('        enemies[i].dy = 0;\n')
        f.write('    }\n\n')
        f.write('    count = 0;\n')
        f.write('    for (sp = enemy_map; sp->type != ENEMY_NONE && count < max; sp++) {\n')
        f.write('        int sx = (int)sp->wx - scroll_x;\n')
        f.write('        int sy = (int)sp->wy - scroll_y;\n')
        f.write('        if (sx >= 0 && sx + SHAPE_ENEMY_W <= GAME_W &&\n')
        f.write('            sy >= 0 && sy + SHAPE_ENEMY_H <= GAME_H) {\n')
        f.write('            anim[count] = 0;\n')
        f.write('            sprite_init(&enemies[count], sx, sy,\n')
        f.write('                        SHAPE_ENEMY_W, SHAPE_ENEMY_H,\n')
        f.write('                        (rng_next() & 1) ? 1 : -1,\n')
        f.write('                        (rng_next() & 1) ? 1 : -1,\n')
        f.write('                        shape_enemy_frames[0]);\n')
        f.write('            count++;\n')
        f.write('        }\n')
        f.write('    }\n')
        f.write('}\n\n')
        f.write('int enemy_check_projectile(Sprite *enemies, int max, const Sprite *projectile, const char *bg)\n')
        f.write('{\n')
        f.write('    int i;\n')
        f.write('    for (i = 0; i < max; i++) {\n')
        f.write('        Sprite *e = &enemies[i];\n')
        f.write('        if (sprite_is_moving(e) && sprite_overlaps(projectile, e)) {\n')
        f.write('            sprite_erase(e, bg);\n')
        f.write('            e->dx = 0;\n')
        f.write('            e->dy = 0;\n')
        f.write('            e->data = 0;\n')
        f.write('            return 1;\n')
        f.write('        }\n')
        f.write('    }\n')
        f.write('    return 0;\n')
        f.write('}\n\n')
        f.write('int enemy_check_player(Sprite *enemies, int max, const Sprite *player)\n')
        f.write('{\n')
        f.write('    int i;\n')
        f.write('    for (i = 0; i < max; i++) {\n')
        f.write('        Sprite *e = &enemies[i];\n')
        f.write('        if (sprite_is_moving(e) && sprite_overlaps(player, e))\n')
        f.write('            return 1;\n')
        f.write('    }\n')
        f.write('    return 0;\n')
        f.write('}\n')
    print(f'Enemies  {output_path}  ({len(enemies)} entries)')


def write_exit_map(exits, output_path, input_path):
    wx = exits[0][0] if exits else 0
    wy = exits[0][1] if exits else 0

    guard = os.path.basename(output_path).upper().replace('.', '_').replace('-', '_')
    with open(output_path, 'w') as f:
        f.write(f'#ifndef {guard}\n')
        f.write(f'#define {guard}\n\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write(f'#define EXIT_ENABLED {1 if exits else 0}\n')
        f.write(f'#define EXIT_WX {wx}\n')
        f.write(f'#define EXIT_WY {wy}\n\n')
        f.write(f'#endif /* {guard} */\n')

    print(f'Exit     {output_path}  ({wx}, {wy})')


# ---------------------------------------------------------------------------
# C header output
# ---------------------------------------------------------------------------
def write_header(palette, world_map, usage_counts, varname, output_path, input_path, world_w, world_h):
    guard = (os.path.basename(output_path)
             .upper().replace('.', '_').replace('-', '_'))

    tiles_x = (world_w + TILE_W - 1) // TILE_W
    tiles_y = (world_h + TILE_H - 1) // TILE_H

    with open(output_path, 'w') as f:
        f.write(f'#ifndef {guard}\n')
        f.write(f'#define {guard}\n')
        f.write(f'#include "world_api.h"\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write(f'/* World: {world_w} x {world_h} chars  ({tiles_x} x {tiles_y} tiles of {TILE_W}x{TILE_H}) */\n')
        f.write(f'/* Tiles: {len(palette)} unique  |  Sparse entries: {len(world_map)} */\n\n')

        f.write(f'#define {varname.upper()}_TILE_W    {TILE_W}\n')
        f.write(f'#define {varname.upper()}_TILE_H    {TILE_H}\n')
        f.write(f'#define {varname.upper()}_N_TILES   {len(palette)}\n')
        f.write(f'#define {varname.upper()}_WORLD_W   {world_w}\n')
        f.write(f'#define {varname.upper()}_WORLD_H   {world_h}\n\n')

        # Tile palette
        f.write(f'static const char {varname}_tiles[{len(palette)}][{TILE_H}][{TILE_W}] = {{\n')
        for i, tile in enumerate(palette):
            rows = []
            for r in range(TILE_H):
                vals = ', '.join(f'0x{tile[r*TILE_W + c]:02X}' for c in range(TILE_W))
                rows.append(f'{{ {vals} }}')
            f.write(f'    /* {i:2d} */ {{ {", ".join(rows)} }},\n')
        f.write('};\n\n')

        # Sparse world map — tx, ty, tile_idx triples, 0xFF sentinel
        f.write(f'/* tx, ty, tile_idx triples — 0xFF terminates */\n')
        f.write(f'static const unsigned char {varname}_world[] = {{\n')
        for tx, ty, ti in world_map:
            f.write(f'    {tx}, {ty}, {ti},\n')
        f.write('    0xFF\n')
        f.write('};\n\n')

        # Pre-built World struct instance
        f.write(f'static const World {varname} = {{\n')
        f.write(f'    (const char *){varname}_tiles,\n')
        f.write(f'    {varname}_world,\n')
        f.write(f'    {TILE_W},\n')
        f.write(f'    {TILE_H},\n')
        f.write(f'}};\n\n')

        # Tile usage report
        one_off = [i for i, count in enumerate(usage_counts) if count == 1]
        f.write(f'/* Tile usage: {len(palette)} unique, {len(one_off)} used once */\n')
        if one_off:
            f.write(f'/* One-off tile IDs: {", ".join(str(i) for i in one_off)} */\n')

        f.write(f'#endif /* {guard} */\n')

    tile_bytes  = len(palette) * TILE_W * TILE_H
    map_bytes   = len(world_map) * 3 + 1   # triples + 0xFF sentinel
    total_bytes = tile_bytes + map_bytes
    print(f'Header   {output_path}')
    print(f'  Tiles   : {len(palette):4d} unique  x {TILE_W*TILE_H} bytes = {tile_bytes:5d} bytes')
    print(f'  Map     : {len(world_map):4d} entries x 3 bytes = {map_bytes:5d} bytes')
    print(f'  Total   : {total_bytes:5d} bytes  ({total_bytes/1024:.1f} KB)')


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------
def main():
    args = sys.argv[1:]
    if len(args) < 2:
        print(__doc__)
        sys.exit(1)

    input_path  = args[0]
    output_path = args[1]
    varname     = args[2] if len(args) > 2 and not args[2].startswith('--') else 'bg'

    src_w, src_h = Image.open(input_path).size
    world_w = int(args[args.index('--width')  + 1]) if '--width'  in args else src_w
    world_h = int(args[args.index('--height') + 1]) if '--height' in args else src_h
    boon_map_path = args[args.index('--boon-map') + 1] if '--boon-map' in args else None
    enemy_map_path = args[args.index('--enemy-map') + 1] if '--enemy-map' in args else None
    exit_map_path = args[args.index('--exit-map') + 1] if '--exit-map' in args else None

    base, _       = os.path.splitext(output_path)
    preview_path  = base + '_preview.png'
    tiles_path    = base + '_tiles.png'

    chars, boons, enemies, exits = convert(input_path, world_w, world_h)
    palette, world_map, usage_counts, usage_locations = extract_tiles(chars, world_w, world_h)
    save_preview(chars, usage_counts, usage_locations, preview_path, world_w, world_h)
    save_tile_sheet(palette, usage_counts, usage_locations, tiles_path)
    write_header(palette, world_map, usage_counts, varname, output_path, input_path, world_w, world_h)
    if boon_map_path:
        write_boon_map(boons, boon_map_path, input_path)
    if enemy_map_path:
        write_enemy_map(enemies, enemy_map_path, input_path)
    if exit_map_path:
        write_exit_map(exits, exit_map_path, input_path)

    report_tile_usage(palette, usage_counts, usage_locations)


def report_tile_usage(palette, usage_counts, usage_locations):
    one_off = [i for i, count in enumerate(usage_counts) if count == 1]
    print(f'Tile usage: {len(palette)} unique, {len(one_off)} used once')
    for ti in one_off:
        tx, ty = usage_locations[ti][0]
        print(f'  TILE {ti:3d}: one-off at tx={tx}, ty={ty}')


if __name__ == '__main__':
    main()
