#include <8051.h>

#define WRITE_SECOND 0x80 
#define WRITE_MINUTE 0x82 
#define WRITE_HOUR 0x84 
#define READ_SECOND 0x81 
#define READ_MINUTE 0x83 
#define READ_HOUR 0x85 
#define WRITE_PROTECT 0x8E 

#define CE P3_2
#define IO P3_1
#define CLK P3_0


#define D1 P1_1
#define D2 P3_6
#define D3 P3_5
#define D4 P1_7
#define DA P1_0
#define DB P3_4
#define DC P1_5
#define DD P1_3
#define DE P1_2
#define DF P3_7
#define DG P1_6
#define DP P1_4

unsigned char numbers[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void writeSeg(unsigned char data)
{
    D1 = !(data & 0x1);
    D2 = !(data & 0x2);
    D3 = !(data & 0x4);
    D4 = !(data & 0x8);
}

void writeBit(unsigned char data)
{
    DA = data & 0x01;
    DB = data & 0x02;
    DC = data & 0x04;
    DD = data & 0x08;
    DE = data & 0x10;
    DF = data & 0x20;
    DG = data & 0x40;
    DP = data & 0x80;
}

void writeDt(unsigned char seg, unsigned char data)
{
    D1 = 1;
    D2 = 1;
    D3 = 1;
    D4 = 1;
    DA = 0;
    DB = 0;
    DC = 0;
    DD = 0;
    DE = 0;
    DF = 0;
    DG = 0;
    DP = 0;
    writeSeg(1 << (seg - 1));
    writeBit(data);
}

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
    unsigned char i, tmp, data = 0;
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

    // ignoring the first zero bit
    CLK = 0;
    for(tmp=0; tmp<4; tmp++);  // delay
    CLK = 1;

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
    return data;
}

unsigned char getHour(const unsigned char value) {
  unsigned char adj;
  if (value & 128)  // 12-hour mode
    adj = 12 * ((value & 32) >> 5);
  else           // 24-hour mode
    adj = 10 * ((value & (32 + 16)) >> 4);
  return (value & 15) + adj;
}

unsigned char hour, minute;

void timer0() __interrupt(TF0_VECTOR)
{
    static unsigned char t;
    switch(t)
    {
        case 0:
            writeDt(1, numbers[hour / 10]);
            break;
        case 1:
            writeDt(2, numbers[hour % 10] | 0x80);
            break;
        case 2:
            writeDt(3, numbers[(minute & 0xf0) >> 4]);
            break;
        case 3:
            writeDt(4, numbers[minute & 0x0f]);
            break;
        default:
            t = -1;
    }
    t++;
}

int main()
{
    TMOD = 0x02;
    TR0 = 1;
    ET0 = 1;
    EA = 1;

    while(1)
    {
        hour = getHour(read1302(READ_HOUR));
        minute = read1302(READ_MINUTE) & 0x7f;
    }
    return 0;
}

