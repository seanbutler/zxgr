#ifndef TIMING_H
#define TIMING_H

static void wait_frame(void)
{
#asm
    ld hl,0x4034
wf_read:
    ld a,(hl)
wf_spin:
    cp (hl)
    jr z,wf_spin
#endasm
}

#endif
