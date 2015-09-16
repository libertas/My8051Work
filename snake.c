#include <8051.h>
#define _nop_()	__asm NOP __endasm
#define IO P3_2
#define CS P3_3
#define CLK P3_4

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


#define FREQ 5500  // kHz
#define KEYPORT P1

unsigned long hundredMicros = 0;

void timer0() __interrupt(1)
{
    hundredMicros++;
}

void delay(unsigned long ms)
{
    unsigned long lastTime;
    lastTime = hundredMicros;
    while((hundredMicros-lastTime) < ms*10);
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
    KEYPORT = 0x0f;
    delay(1);
    tmp1 = KEYPORT;
    // Scan rows
    KEYPORT = 0xf0;
    delay(1);
    tmp1 |= KEYPORT & 0xf0;

    delay(8);  // In case of shaking

    KEYPORT = 0x0f;
    delay(1);
    tmp2 = KEYPORT;
    KEYPORT = 0xf0;
    delay(1);
    tmp2 |= KEYPORT & 0xf0;

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


void init7219()
{
    unsigned int i;

    sendData(0x0c01);
    sendData(0x0a00);
    sendData(0x0b07);
    sendData(0x0900);
    sendData(0x0f00);

    for(i=0; i<8; i++)
    {
        sendData((i + 1) << 8);
    }
}

__bit up = 0, down = 0, left = 0, right = 1, mid = 0;

unsigned char map[8] = {8};

void go()
{
    
}

int main()
{
    unsigned int key;

    initDelay();
    init7219();

    while(1)
    {
        key = keyScan();
        if(key)
        {
            sendData(0x08ff);
            switch(key)
            {
                case 1 << 6:
                    up = 1;
                    down = 0;
                    left = 0;
                    right = 0;
                    mid = 0;
                    break;
                case 1 << 9:
                    up = 0;
                    down = 0;
                    left = 1;
                    right = 0;
                    mid = 0;
                    break;
                case 1 << 1:
                    up = 0;
                    down = 0;
                    left = 0;
                    right = 1;
                    mid = 0;
                    break;
                case 1 << 4:
                    up = 0;
                    down = 1;
                    left = 0;
                    right = 0;
                    mid = 0;
                    break;
                case 1 << 5:
                    up = 0;
                    down = 0;
                    left = 0;
                    right = 0;
                    mid = 1;
                    break;
                default:
                    break;
            }
        }
        go();
    }
    return 0;
}

