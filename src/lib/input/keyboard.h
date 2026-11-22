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


/* port 0xFEFE row: SHIFT(0) Z(1) X(2) C(3) V(4)       */
/* port 0xFDFE row: A(0) S(1) D(2) F(3) G(4)            */
/* port 0xBFFE row: ENTER(0) L(1) K(2) J(3) H(4)        */
/* port 0x7FFE row: SPACE(0) .(1) ,(2) M(3) N(4)        */

#define KEY_Z      (!(read_port(0xFEFE) & 0x02))
#define KEY_X      (!(read_port(0xFEFE) & 0x04))
#define KEY_K      (!(read_port(0xBFFE) & 0x04))
#define KEY_L      (!(read_port(0xBFFE) & 0x02))
#define KEY_M      (!(read_port(0x7FFE) & 0x04))
#define KEY_ENTER  (!(read_port(0xBFFE) & 0x01))
#define KEY_SPACE  (!(read_port(0x7FFE) & 0x01))

#define KEY_LEFT   KEY_Z
#define KEY_RIGHT  KEY_X
#define KEY_UP     KEY_K
#define KEY_DOWN   KEY_M
#define KEY_ACTION KEY_L
#define KEY_SHOOT  KEY_L

#endif
