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
#define KEY_UP    (!(read_port(0xFBFE) & 0x01))  /* Q */
#define KEY_DOWN  (!(read_port(0xFDFE) & 0x01))  /* A */
#define KEY_LEFT  (!(read_port(0xDFFE) & 0x02))  /* O */
#define KEY_RIGHT (!(read_port(0xDFFE) & 0x01))  /* P */

#endif
