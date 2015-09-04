#include <8052.h>

// F_CPU = 11529000UL

void delay(unsigned int n)
{
    while(--n);             // 8 us for one time
}

unsigned char irCode[32] = {0};

void irGetCode()
{
    unsigned int i = 0;
    while(!P3_2);
    while(i < 32)
    {
        delay(100);
        if(P3_2)
        {
            irCode[i] = 1;
            while(P3_2);
        }
        else
        {
            irCode[i] = 0;
        }
        while(!P3_2);
        i++;
    }
    P1_0 = 0;
    delay(65535);
    P1_0 = 1;
}

void irScan()
{
    while(P3_2);

    delay(500);
    if(P3_2)
        return;
    while(!P3_2);

    delay(250);
    if(!P3_2)
        return;
    while(P3_2);
    irGetCode();
}

int main()
{
    while(1)
    {
        irScan();
    }
    return 0;
}

