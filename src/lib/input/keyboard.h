#ifndef KEYBOARD_H
#define KEYBOARD_H

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

/* cursor keys (QAOP) */
#define KEY_UP    (!(read_port(0xFBFE) & 0x01))  /* Q */
#define KEY_DOWN  (!(read_port(0xFDFE) & 0x01))  /* A */
#define KEY_LEFT  (!(read_port(0xDFFE) & 0x02))  /* O */
#define KEY_RIGHT (!(read_port(0xDFFE) & 0x01))  /* P */

/* ZX = left/right, KM = up/down, L = action */
#define KEY_Z      (!(read_port(0xFEFE) & 0x02))  /* Z */
#define KEY_X      (!(read_port(0xFEFE) & 0x04))  /* X */
#define KEY_K      (!(read_port(0xBFFE) & 0x04))  /* K */
#define KEY_M      (!(read_port(0x7FFE) & 0x04))  /* M */
#define KEY_L      (!(read_port(0xBFFE) & 0x08))  /* L */

#define KEY_ALT_LEFT   KEY_Z
#define KEY_ALT_RIGHT  KEY_X
#define KEY_ALT_UP     KEY_K
#define KEY_ALT_DOWN   KEY_M
#define KEY_ACTION     KEY_L

#endif
