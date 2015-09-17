#include <8051.h>
#include "binary.h"

#define _nop_()	__asm NOP __endasm
#define IO P1_2
#define CS P1_3
#define CLK P1_4

#define IPT P3_2

void sendbyte(unsigned char dat)
{
    unsigned char i,temp;
    _nop_();
    for (i=0;i<8;i++)
   {
       temp=dat&0x80;
       dat=dat<<1;
       if(temp)
           IO=1;
       else
           IO=0;
       CLK=0;
       _nop_();
       CLK=1;
   }
}

void sendByte(unsigned char byte)
{
    unsigned char i, tmp;
    _nop_();
    for(i=0; i<8; i++)
    {
        tmp = byte & 0x80;
        byte = byte << 1;
        if(tmp)
            IO = 1;
        else
            IO = 0;

        CLK = 0;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        CLK = 1;
    }
}

void sendData(unsigned int data)
{
    unsigned char high, low;
    high = data >> 8;
    low = data;
    CS = 0;
    sendByte(high);
    sendByte(low);
    CS = 1;
}

__code unsigned char img[][8] = {
    {
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00000000
    },
    {
        B01100110,
        B01100110,
        B01100110,
        B01111110,
        B01111110,
        B01100110,
        B01100110,
        B01100110
    },
    {
        B00111100,
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00111100
    },
    {
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00011000,
        B00000000,
        B00011000
    },
};

void display(unsigned char p[8])
{
    unsigned char i;
    for(i=1; i<=8; i++)
        sendData((0x0100 * i) | p[i - 1]);
}

void timer0() __interrupt(TF0_VECTOR)
{
    static unsigned int t;
    t++;
    if(t > 39)
        t = 10;
    display(img[t / 10]);
}

int main()
{
    sendData(0x0c01);
    sendData(0x0a00);
    sendData(0x0b07);
    sendData(0x0900);
    sendData(0x0f00);
    display(img[0]);

    TMOD = 0x01;
    TH0 = 0;
    TL0 = 0;
    ET0 = 1;
    EA = 1;

    while(1)
    {
        if(IPT)
        {
            TR0 = 0;
            display(img[0]);
        }
        else
            TR0 = 1;
    }
    return 0;
}

