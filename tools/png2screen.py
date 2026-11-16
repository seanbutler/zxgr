#!/usr/bin/env python3
"""
png2screen.py — Convert PNG to a ZX81 title screen C header.

Treats the image as a 64x48 bitmap (2x2 pixels per ZX81 character cell).
Each 2x2 block maps to one of the 16 ZX81 quadrant block characters,
giving a full 64x48 black-and-white pixel resolution on the 32x24 screen.

Output: a flat const char[768] array (32 * 24 chars).

Usage:
    python3 png2screen.py <input.png> <output.h> [varname]
"""

import sys, os

try:
    from PIL import Image
except ImportError:
    sys.exit("Pillow required: pip install pillow")

SCREEN_W = 32
SCREEN_H = 24
PIX_W    = SCREEN_W * 2   # 64
PIX_H    = SCREEN_H * 2   # 48
THRESHOLD = 128            # >= white, < black

# Index = TL | (TR<<1) | (BL<<2) | (BR<<3)
QUAD_LOOKUP = [
    0x00, 0x01, 0x02, 0x03,   # 0000  0001  0010  0011
    0x04, 0x05, 0x06, 0x07,   # 0100  0101  0110  0111
    0x87, 0x86, 0x85, 0x84,   # 1000  1001  1010  1011
    0x83, 0x82, 0x81, 0x80,   # 1100  1101  1110  1111
]

def convert(input_path):
    img = Image.open(input_path).convert('L')
    img = img.resize((PIX_W, PIX_H), Image.LANCZOS)
    px  = img.load()

    chars = []
    for cy in range(SCREEN_H):
        for cx in range(SCREEN_W):
            tl = 1 if px[cx*2,   cy*2  ] < THRESHOLD else 0
            tr = 1 if px[cx*2+1, cy*2  ] < THRESHOLD else 0
            bl = 1 if px[cx*2,   cy*2+1] < THRESHOLD else 0
            br = 1 if px[cx*2+1, cy*2+1] < THRESHOLD else 0
            chars.append(QUAD_LOOKUP[tl | (tr<<1) | (bl<<2) | (br<<3)])
    return chars


def save_preview(chars, output_path):
    from PIL import ImageDraw
    CSCALE = 8
    img = Image.new('L', (PIX_W * CSCALE // 2, PIX_H * CSCALE // 2), 255)
    px  = img.load()
    # render each char as 2x2 block of pixels scaled up
    for cy in range(SCREEN_H):
        for cx in range(SCREEN_W):
            code = chars[cy * SCREEN_W + cx]
            bits = QUAD_LOOKUP.index(code)
            tl = (bits >> 0) & 1
            tr = (bits >> 1) & 1
            bl = (bits >> 2) & 1
            br = (bits >> 3) & 1
            for dy, row_bits in enumerate([tl * 0x11 + tr * 0x11, bl * 0x11 + br * 0x11]):
                pass
            # just draw 2x2 blocks
            for (dx, dy, v) in [(0,0,tl),(1,0,tr),(0,1,bl),(1,1,br)]:
                colour = 0 if v else 255
                bx = (cx * 2 + dx) * (CSCALE // 2)
                by = (cy * 2 + dy) * (CSCALE // 2)
                for ry in range(CSCALE // 2):
                    for rx in range(CSCALE // 2):
                        px[bx + rx, by + ry] = colour
    img.save(output_path)
    print(f'Preview  {output_path}  ({PIX_W}x{PIX_H}px effective)')


def write_header(chars, varname, output_path, input_path):
    guard = os.path.basename(output_path).upper().replace('.', '_').replace('-', '_')
    with open(output_path, 'w') as f:
        f.write(f'#ifndef {guard}\n')
        f.write(f'#define {guard}\n\n')
        f.write(f'#include <string.h>\n')
        f.write(f'#include "dfile.h"\n\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write(f'/* 32x24 chars, quad-pixel block graphics (64x48 effective resolution) */\n\n')
        f.write(f'static const char {varname}[{SCREEN_H} * {SCREEN_W}] = {{\n')
        for row in range(SCREEN_H):
            vals = ', '.join(f'0x{chars[row*SCREEN_W + col]:02X}' for col in range(SCREEN_W))
            f.write(f'    {vals},\n')
        f.write('};\n\n')
        f.write(f'static void {varname}_draw(void)\n')
        f.write(f'{{\n')
        f.write(f'    int r;\n')
        f.write(f'    for (r = 0; r < 24; r++)\n')
        f.write(f'        memcpy(dfile + r * DFILE_STRIDE, {varname} + r * 32, 32);\n')
        f.write(f'}}\n\n')
        f.write(f'#endif /* {guard} */\n')
    total = SCREEN_W * SCREEN_H
    print(f'Header   {output_path}  ({total} bytes)')


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        print(__doc__)
        sys.exit(1)
    input_path  = args[0]
    output_path = args[1]
    varname     = args[2] if len(args) > 2 else 'title_screen'

    base = os.path.splitext(output_path)[0]
    chars = convert(input_path)
    save_preview(chars, base + '_preview.png')
    write_header(chars, varname, output_path, input_path)


if __name__ == '__main__':
    main()
