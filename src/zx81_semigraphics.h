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

/* Utility macro */
#define ZX81_INV(c) ((c) ^ 0x80)

#endif /* ZX81_SEMIGRAPHICS_H */
