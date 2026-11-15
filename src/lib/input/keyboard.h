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


/* ZX = left/right, KM = up/down, L = action */
#define KEY_Z      (!(read_port(0xFEFE) & 0x02))  /* Z */
#define KEY_X      (!(read_port(0xFEFE) & 0x04))  /* X */
#define KEY_K      (!(read_port(0xBFFE) & 0x04))  /* K */
#define KEY_M      (!(read_port(0x7FFE) & 0x04))  /* M */
#define KEY_L      (!(read_port(0xBFFE) & 0x08))  /* L */

#define KEY_LEFT   KEY_Z
#define KEY_RIGHT  KEY_X
#define KEY_UP     KEY_K
#define KEY_DOWN   KEY_M
#define KEY_ACTION     KEY_L

#endif
