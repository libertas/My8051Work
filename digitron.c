#include <8051.h>

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

int main()
{
    P1 = 0;
    P3 = 0;

    while(1)
    {
        writeDt(1, numbers[1]);
        writeDt(2, numbers[6] | 0x80);
        writeDt(3, numbers[8]);
        writeDt(4, numbers[9]);
    }
}

