#include <8051.h>

unsigned char pos = 0;

void ie0() __interrupt(IE0_VECTOR)
{
    P1_0 = !P1_0;
}

void ie1() __interrupt(IE1_VECTOR)
{
    if(P3_4 == 1)
        pos++;
    else
        pos--;
}

int main()
{
    IT0 = 1;
    EX0 = 1;
    IT1 = 1;
    EX1 = 1;
    EA = 1;

    while(1)
    {
        P2 = pos;
    }
    return 0;
}
