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
RLE_MARK = 0xFF

# Index = TL | (TR<<1) | (BL<<2) | (BR<<3)
QUAD_LOOKUP = [
    0x00, 0x01, 0x02, 0x03,   # 0000  0001  0010  0011
    0x04, 0x05, 0x06, 0x07,   # 0100  0101  0110  0111
    0x87, 0x86, 0x85, 0x84,   # 1000  1001  1010  1011
    0x83, 0x82, 0x81, 0x80,   # 1100  1101  1110  1111
]

def convert(input_path):
    img = Image.open(input_path).convert('L')
    src_w, src_h = img.size
    content_w = (src_w + 1) // 2
    content_h = (src_h + 1) // 2
    px = img.load()

    chars = []
    for cy in range(content_h):
        for cx in range(content_w):
            def sample(dx, dy):
                x = cx * 2 + dx
                y = cy * 2 + dy
                if x >= src_w or y >= src_h:
                    return 0
                return 1 if px[x, y] < THRESHOLD else 0

            tl = sample(0, 0)
            tr = sample(1, 0)
            bl = sample(0, 1)
            br = sample(1, 1)
            chars.append(QUAD_LOOKUP[tl | (tr << 1) | (bl << 2) | (br << 3)])

    return chars, content_w, content_h


def rle_encode(chars):
    out = []
    i = 0
    n = len(chars)

    while i < n:
        b = chars[i]
        j = i + 1
        while j < n and chars[j] == b and (j - i) < 255:
            j += 1
        run = j - i

        if run >= 3:
            out.extend([RLE_MARK, run, b])
            i = j
            continue

        if b == RLE_MARK:
            out.extend([RLE_MARK, 0])
        else:
            out.append(b)
        i += 1

    out.extend([RLE_MARK, 0xFF])
    return out


def crop_content(chars):
    min_x = SCREEN_W
    min_y = SCREEN_H
    max_x = -1
    max_y = -1

    for y in range(SCREEN_H):
        for x in range(SCREEN_W):
            if chars[y * SCREEN_W + x] != 0x00:
                if x < min_x:
                    min_x = x
                if y < min_y:
                    min_y = y
                if x > max_x:
                    max_x = x
                if y > max_y:
                    max_y = y

    if max_x < min_x or max_y < min_y:
        return [0x00], 1, 1

    crop_w = (max_x - min_x) + 1
    crop_h = (max_y - min_y) + 1
    cropped = []

    for y in range(min_y, max_y + 1):
        row_start = y * SCREEN_W + min_x
        cropped.extend(chars[row_start:row_start + crop_w])

    return cropped, crop_w, crop_h


def save_preview(chars, content_w, content_h, output_path):
    CSCALE = 8
    img = Image.new('L', (content_w * 2 * CSCALE // 2, content_h * 2 * CSCALE // 2), 255)
    px = img.load()
    for cy in range(content_h):
        for cx in range(content_w):
            code = chars[cy * content_w + cx]
            bits = QUAD_LOOKUP.index(code)
            tl = (bits >> 0) & 1
            tr = (bits >> 1) & 1
            bl = (bits >> 2) & 1
            br = (bits >> 3) & 1
            for dx, dy, v in [(0, 0, tl), (1, 0, tr), (0, 1, bl), (1, 1, br)]:
                colour = 0 if v else 255
                bx = (cx * 2 + dx) * (CSCALE // 2)
                by = (cy * 2 + dy) * (CSCALE // 2)
                for ry in range(CSCALE // 2):
                    for rx in range(CSCALE // 2):
                        px[bx + rx, by + ry] = colour
    img.save(output_path)
    print(f'Preview  {output_path}  ({content_w * 2}x{content_h * 2}px effective)')


def write_header(chars, content_w, content_h, varname, output_path, input_path):
    guard = os.path.basename(output_path).upper().replace('.', '_').replace('-', '_')
    upper_name = varname.upper()
    rle = rle_encode(chars)

    with open(output_path, 'w') as f:
        f.write(f'#ifndef {guard}\n')
        f.write(f'#define {guard}\n\n')
        f.write(f'#include "screen_rle.h"\n\n')
        f.write(f'/* Generated from {os.path.basename(input_path)} -- do not edit */\n')
        f.write(f'/* Source PNG: {content_w * 2}x{content_h * 2} effective pixels */\n')
        f.write(f'/* Content: {content_w}x{content_h} chars, drawn centered */\n')
        f.write(f'/* RLE stream format: */\n')
        f.write(f'/*   literal byte != 0x{RLE_MARK:02X} -> output directly */\n')
        f.write(f'/*   0x{RLE_MARK:02X}, 0x00            -> output literal 0x{RLE_MARK:02X} */\n')
        f.write(f'/*   0x{RLE_MARK:02X}, N, V            -> output V repeated N times (N>=1) */\n')
        f.write(f'/*   0x{RLE_MARK:02X}, 0xFF            -> end of stream */\n\n')

        f.write(f'#define {upper_name}_CONTENT_W {content_w}\n')
        f.write(f'#define {upper_name}_CONTENT_H {content_h}\n\n')
        f.write(f'static const unsigned char {varname}_rle[{len(rle)}] = {{\n')
        for i in range(0, len(rle), 16):
            vals = ', '.join(f'0x{b:02X}' for b in rle[i:i+16])
            f.write(f'    {vals},\n')
        f.write('};\n\n')
        f.write(f'#endif /* {guard} */\n')

    print(f'Header   {output_path}  ({content_w}x{content_h} chars, rle {len(rle)} bytes)')


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        print(__doc__)
        sys.exit(1)
    input_path  = args[0]
    output_path = args[1]
    varname     = args[2] if len(args) > 2 else 'title_screen'

    base = os.path.splitext(output_path)[0]
    chars, content_w, content_h = convert(input_path)
    save_preview(chars, content_w, content_h, base + '_preview.png')
    write_header(chars, content_w, content_h, varname, output_path, input_path)


if __name__ == '__main__':
    main()
