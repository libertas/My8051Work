#include <8051.h>

#define WRITE_SECOND 0x80 
#define WRITE_MINUTE 0x82 
#define WRITE_HOUR 0x84 
#define READ_SECOND 0x81 
#define READ_MINUTE 0x83 
#define READ_HOUR 0x85 
#define WRITE_PROTECT 0x8E 

#define CE P3_5
#define IO P3_6
#define CLK P3_7

void write1302(unsigned char addr, unsigned char data)
{
    unsigned char i, tmp;

    CE = 0;
    CLK = 0;
    CE = 1;
    for(i=0; i<8; i++)
    {
        CLK = 0;
        tmp = addr & 0x01;
        addr = addr >> 1;
        if(tmp)
            IO = 1;
        else
            IO = 0;
        CLK = 1;
        for(tmp=0; tmp<4; tmp++);  // delay
    }

    for(i=0; i<8; i++)
    {
        CLK = 0;
        tmp = data & 0x01;
        data = data >> 1;
        if(tmp)
            IO = 1;
        else
            IO = 0;
        CLK = 1;
        for(tmp=0; tmp<4; tmp++);  // delay
    }
    CE = 0;
}

unsigned char read1302(unsigned char addr)
{
    unsigned char i, tmp, data = 0, data1;
    CE = 0;
    CLK = 0;
    CE = 1;
    for(i=0; i<8; i++)
    {
        CLK = 0;
        tmp = addr & 0x01;
        addr = addr >> 1;
        if(tmp)
            IO = 1;
        else
            IO = 0;
        CLK = 1;
        for(tmp=0; tmp<4; tmp++);  // delay
    }

    for(i=0; i<8; i++)
    {
        data = data >> 1;
        tmp = IO;
        if(tmp)
            data |= 0x80;
        CLK = 0;
        for(tmp=0; tmp<4; tmp++);  // delay
        CLK = 1;
        for(tmp=0; tmp<4; tmp++);  // delay
    }
    CE = 0;
    data1 = data / 16;
    data = data % 16;
    return data + data1 * 10;
}

void setup()
{
    unsigned char sec;

    write1302(WRITE_PROTECT, 0x00);

    sec = read1302(READ_SECOND);
    sec &= ~(1 << 7);
    write1302(WRITE_SECOND, sec);  // disable halt mode

    write1302(WRITE_PROTECT, 0x80);
}

int main()
{
    //setup();

    while(1)
    {
        P2 = ~read1302(READ_SECOND);
    }
    return 0;
}

