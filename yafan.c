#include <8051.h>
#define FREQ 11529  // kHz

unsigned long hundredSeconds = 0;

void timer0() __interrupt(1)
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

    delay(8);  // In case of shaking

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

    delay(100);  // In case of shaking
    return result;
}

unsigned char minutes = 0, seconds = 0;
__bit statusP2 = 1;

void timer1() __interrupt(3)
{
    static unsigned int t;
    t++;
    if(t == (unsigned int)(FREQ/3.072))  // 1 second
    {
        t = 0;
        if(minutes)
        {
            if(seconds)
                seconds--;
            else
            {
                minutes--;
                if(minutes)
                    seconds = 59;
            }
            if(seconds == 57)  // Turn off the leds
            {
                P2 = 0xff;
                statusP2 = 0;
            }
        }
    }
}

// Get the lowest number from the keys pressed
unsigned char fromKey2Int(unsigned int key)
{
    unsigned char i;
    for(i=0; i<16; i++)
    {
        if(key&1<<i)
            return i;
    }
    return 0;
}

void initTimer1()
{
    TMOD |= 0x20;  // Time1 is running at mod 3
    TH1 = 0;
    TL1 = 0;
    TR1 = 1;
    ET1 = 1;
    EA = 1;
}

int main()
{
    unsigned int key;
    initDelay();
    initTimer1();
    while(1)
    {
        key = keyScan();
        if(key)
        {
            if(statusP2)
            {
                minutes = fromKey2Int(key) * 10;
                seconds = 0;
            }
            else
            {
                statusP2 = 1;
            }
            P2 = ~minutes;  // Show the minutes left on P2

            if(minutes)
                P1_0 = 0;  // Power on
            else
                P1_0 = 1;  // Power off

            delay(500);  // Make the buttons perform better
        }
    }
    return 0;
}

