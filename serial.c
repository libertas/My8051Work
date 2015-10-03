#include <8051.h>

unsigned char buff;

void serial(void) __interrupt(SI0_VECTOR)
{
    if(RI == 1)
    {
        RI = 0;
        buff = SBUF;
        switch(buff)
        {
            case '0':
                P1_0 = 0;
                SBUF = '0';
                break;
            case '1':
                P1_0 = 1;
                SBUF = '1';
                break;
        }
    }
    else
    {
        TI = 0;
    }
}

int main()
{
    SCON = 0x50;
    TMOD = 0x20;
    // 11.529 Mhz
    TH1 = 0xfd;
    TL1 = 0xfd;
    TR1 = 1;
    ES = 1;
    EA = 1;
    P1_0 = 0;
    while(1)
    {
    }
    return 0;
}

