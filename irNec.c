#include <8052.h>

#define FREQ 11529  // kHz

unsigned long hundredSeconds = 0;

// state:
// 0 means not working
// 1 means the first process
// 2 means the second process
unsigned char state = 0;

unsigned long lastTime = 0;

void timer0() __interrupt(IE0_VECTOR)
{
    P2 = hundredSeconds/10000UL;
}

void timer1() __interrupt(IE1_VECTOR)
{
    state = 0;
}


unsigned char irCode[32] = {0}, counter;

void int1() __interrupt(TF0_VECTOR)
{
    unsigned long thisTime, interval;
    thisTime = hundredSeconds;
    interval = thisTime - lastTime;
    switch(state)
    {
        case 0:
            state = 1;
            break;
        case 1:
            state = 2;
            counter = 0;
            break;
        case 2:
            if(interval >= 9 && interval <=13)
                irCode[counter] = 0;
            if(interval >=21 && interval <=24)
                irCode[counter] = 1;
            counter++;
            if(counter==32)
            {
                state = 0;
            }
            break;
    }
}

void setup()
{
    IT0 = 1;
    EX0 = 1;
    EA = 1;

    TMOD |= 0x02;  // Time0 is running at mod 3
    TH0 = 256-FREQ/120;
    TL0 = 256-FREQ/120;
    TR0 = 1;
    ET0 = 1;

    TMOD |= 0x10;  // Time0 is running at mod 3
    TH1 = 0;
    TL1 = 0;
    TR1 = 1;
    ET1 = 1;

    EA = 1;
}

int main()
{
    unsigned char i;
    setup();
    while(1)
    {
        P2 = ~state;
/*
        for(i = 16; i <24; i++)
        {
            P2 |= irCode[i] << (23 - i);
        }
*/
    }
    return 0;
}
