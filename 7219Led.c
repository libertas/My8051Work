#include <8051.h>
#define _nop_()	__asm NOP __endasm
#define IO P1_2
#define CS P1_3
#define CLK P1_4

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

int main()
{
    P1 = 1;
    P2 = 0;

    sendData(0x0c01);
    sendData(0x0a00);
    sendData(0x0b07);
    sendData(0x0900);
    sendData(0x0f00);

    while(1)
    {
        sendData(0x0101);
        sendData(0x0202);
        sendData(0x0303);
        sendData(0x0404);
        sendData(0x0505);
        sendData(0x0606);
        sendData(0x0707);
        sendData(0x0808);
    }
    return 0;
}

