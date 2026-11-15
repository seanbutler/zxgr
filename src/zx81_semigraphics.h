#ifndef ZX81_SEMIGRAPHICS_H
#define ZX81_SEMIGRAPHICS_H

/*
 * ZX81 block-graphics character codes — verified against ROM at 0x1E00.
 *
 * Each character is 8x8 pixels.  Bit 7 of the display-file byte = inverse
 * (all pixel values flipped).  Characters 0x00-0x0A are block graphics;
 * 0x0B onwards are text/punctuation characters.
 *
 * Quadrant layout (each quadrant = 4x4 pixels):
 *   TL | TR
 *   ---+---
 *   BL | BR
 *
 * Grey on TV: characters with ~50% pixel fill appear grey because the TV
 * phosphor spatially averages alternating black/white pixels.
 * Best grey: 0x08 (fine pixel checkerboard, 1-pixel grain).
 * Coarser grey: 0x03/0x83 (4-row horizontal stripes), 0x05/0x85 (vertical).
 */

/* ---- 0% fill ---------------------------------------------------------- */
#define CH_SPACE    0x00   /* blank (all white)                              */

/* ---- 25% fill (one quadrant) ------------------------------------------ */
#define CH_QTR_TL   0x01   /* top-left quadrant                              */
#define CH_QTR_TR   0x02   /* top-right quadrant                             */
#define CH_QTR_BL   0x04   /* bottom-left quadrant                           */

/* ---- 50% fill — solid halves (grey on TV, 4-row grain) ---------------- */
#define CH_HALF_L   0x05   /* left half  (all 8 rows, left  4 cols)          */
#define CH_HALF_T   0x03   /* top  half  (top  4 rows, all 8 cols)           */

/* ---- 50% fill — diagonal halves --------------------------------------- */
#define CH_DIAG_A   0x06   /* TR + BL diagonal (top-right + bottom-left)     */

/* ---- 50% fill — fine checkerboard (best grey on TV, 1-pixel grain) ---- */
#define CH_GREY     0x08   /* full-cell checkerboard  █·█·  all 8 rows       */
#define CH_GREY_B   0x09   /* bottom-half checkerboard (25% total fill)      */
#define CH_GREY_T   0x0A   /* top-half    checkerboard (25% total fill)      */

/* ---- 75% fill (three quadrants) --------------------------------------- */
#define CH_3QTR     0x07   /* TL+TR+BL filled (bottom-right quadrant white)  */
#define CH_INV_BR   0x07   /* all except BR  (75% fill: TL+TR+BL)           */

/* ---- 100% fill -------------------------------------------------------- */
#define CH_FULL     0x80   /* full block (all black) = inverse space         */

/* ---- Inverse variants (bit 7 set) ------------------------------------- */
#define CH_INV_TL   0x81   /* all except TL  (75% fill: TR+BL+BR)           */
#define CH_INV_TR   0x82   /* all except TR  (75% fill: TL+BL+BR)           */
#define CH_HALF_B   0x83   /* bottom half    (inverse top-half)              */
#define CH_INV_BL   0x84   /* all except BL  (75% fill: TL+TR+BR)           */
#define CH_HALF_R   0x85   /* right half     (inverse left-half)             */
#define CH_DIAG_B   0x86   /* TL + BR diagonal (inverse of DIAG_A)          */
#define CH_QTR_BR   0x87   /* bottom-right quadrant (inverse 3QTR)          */
#define CH_GREY_INV 0x88   /* inverse checkerboard ·█·█  (same grey on TV)  */

/* ---- Text / punctuation characters ------------------------------------ */
#define CH_QUOTE    0x0B   /* "                                              */
#define CH_POUND    0x0C   /* £                                              */
#define CH_DOLLAR   0x0D   /* $                                              */
#define CH_COLON    0x0E   /* :                                              */
#define CH_QUERY    0x0F   /* ?                                              */
#define CH_LPAREN   0x10   /* (                                              */
#define CH_RPAREN   0x11   /* )                                              */
#define CH_GT       0x12   /* >                                              */
#define CH_LT       0x13   /* <                                              */
#define CH_EQUALS   0x14   /* =                                              */
#define CH_PLUS     0x15   /* +                                              */
#define CH_MINUS    0x16   /* -                                              */
#define CH_STAR     0x17   /* *                                              */
#define CH_SLASH    0x18   /* /                                              */
#define CH_SEMI     0x19   /* ;                                              */
#define CH_COMMA    0x1A   /* ,                                              */
#define CH_PERIOD   0x1B   /* .                                              */
/* digits 0-9: 0x1C-0x25, letters A-Z: 0x26-0x3F                           */
#define CH_0        0x1C
#define CH_1        0x1D
#define CH_2        0x1E
#define CH_3        0x1F
#define CH_4        0x20
#define CH_5        0x21
#define CH_6        0x22
#define CH_7        0x23
#define CH_8        0x24
#define CH_9        0x25

#define CH_A        0x26
#define CH_B        0x27
#define CH_C        0x28
#define CH_D        0x29
#define CH_E        0x2A
#define CH_F        0x2B
#define CH_G        0x2C
#define CH_H        0x2D
#define CH_I        0x2E
#define CH_J        0x2F
#define CH_K        0x30
#define CH_L        0x31
#define CH_M        0x32
#define CH_N        0x33
#define CH_O        0x34
#define CH_P        0x35
#define CH_Q        0x36
#define CH_R        0x37
#define CH_S        0x38
#define CH_T        0x39
#define CH_U        0x3A
#define CH_V        0x3B
#define CH_W        0x3C
#define CH_X        0x3D
#define CH_Y        0x3E
#define CH_Z        0x3F
#define CH_INV_QUOTE  0x8B   /* inverse "                                    */

/* Utility macro */
#define ZX81_INV(c) ((c) ^ 0x80)

#endif /* ZX81_SEMIGRAPHICS_H */
