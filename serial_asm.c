#include <8051.h>

unsigned char buff;

void serial(void) __interrupt(SI0_VECTOR)
{
    __asm

    mov psw, #0x00

    jbc _RI, lreceive
    sjmp lsend
    lreceive:
    mov a, _SBUF
    mov _SBUF, a

    cjne a, #'1', lclr

lsetb:
    setb _P1_0
    sjmp lrend

lclr:
    cjne a, #'0', lrend

    clr _P1_0
    sjmp lrend

lrend:

    clr _RI

    sjmp luartend

    lsend:

    clr _TI

    sjmp luartend

luartend:

    __endasm;

}

int main()
{
    __asm

    mov _SCON, #0x50
    mov _TMOD, #0x20
    // 11.529 Mhz
    mov _TH1, #0xfd
    mov _TL1, #0xfd
    setb _TR1
    setb _ES
    setb _EA

    mov _P1_0, #2

mainloop:
    sjmp mainloop

    __endasm;

    return 0;
}
