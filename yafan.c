#include <8051.h>
#define FREQ 11529  // kHz

unsigned long hundredSeconds = 0;

void int0() __interrupt(1)
{
    hundredSeconds++;
}

void delay(unsigned long ms)
{
    unsigned long lastTime;
    lastTime = hundredSeconds;
    while((hundredSeconds-lastTime) < ms*10);
}

void initDelay()
{
    TMOD |= 0x02;  // Time0 is running at mod 3
    TH0 = 256-FREQ/120;
    TL0 = 256-FREQ/120;
    TR0 = 1;
    ET0 = 1;
    EA = 1;
}

unsigned int keyScan()
{
    // Higher bits of tmp store rows while lower store columns
    unsigned char tmp1, tmp2;

    unsigned int result = 0;
    unsigned char row, col;

    // Scan colums
    P3 = 0x0f;
    delay(1);
    tmp1 = P3;
    // Scan rows
    P3 = 0xf0;
    delay(1);
    tmp1 |= P3 & 0xf0;

    delay(8);  // Avoid the shaking

    P3 = 0x0f;
    delay(1);
    tmp2 = P3;
    P3 = 0xf0;
    delay(1);
    tmp2 |= P3 & 0xf0;

    if(tmp1 == tmp2)
    {
        tmp1 = ~tmp1;
        for(row=0; row<4; row++)
        {
            for(col=0; col<4; col++)
            {
                if((tmp1&(1<<3-col)) && (tmp1&(1<<7-row)))
                    result |= 1<<(row*4+col);
            }
        }
    }
    return result;
}

int main()
{
    initDelay();

    while(1)
    {
        P2 = keyScan()>>8;
    }
    return 0;
}

