#include <8051.h>
#include <stdint.h>

#define _nop_()	__asm NOP __endasm
#define IO P3_2
#define CS P3_3
#define CLK P3_4

void sendbyte(uint8_t dat)
{
    uint8_t i,temp;
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

void sendByte(uint8_t byte)
{
    uint8_t i, tmp;
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

void sendData(uint16_t data)
{
    uint8_t high, low;
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

uint16_t keyScan()
{
    // Higher bits of tmp store rows while lower store columns
    uint8_t tmp1, tmp2;

    uint16_t result = 0;
    uint8_t row, col;

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
uint8_t fromKey2Int(uint16_t key)
{
    uint8_t i;
    for(i=0; i<16; i++)
    {
        if(key&1<<i)
            return i;
    }
    return 0;
}


void init7219()
{
    uint16_t i;

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

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define STOP 4
uint8_t dir = 1 << DOWN;

uint8_t snakeBody[8][8] = {{0}};
uint8_t headX = 0, headY = 0;

void getTail(uint8_t *x, uint8_t *y)
{
    uint8_t tmp;

    *x = headX;
    *y = headY;

    if(snakeBody[headY][headX] == 0xff)
        return;

    tmp = snakeBody[headY][headX];
    while(1)
    {
        if(snakeBody[(tmp & 0xf0) >> 4][tmp & 0x0f] != 0xff)
        {
            *y = (tmp & 0xf0) >> 4;
            *x = tmp & 0x0f;
            tmp = snakeBody[(tmp & 0xf0) >> 4][tmp & 0x0f];
        }
        else
            break;
    }
}

void displayMap()
{
    unsigned int i, j, tmp;
    uint8_t tailX, tailY;
    uint8_t map[8] = {0};

    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
        {
            map[i] = map[i] << 1;
            if(snakeBody[i][j] != 0xff)
                map[i] |= 0x01;
        }
    }

    getTail(&tailX, &tailY);
    if(snakeBody[headY][headX] != 0xff)
    {
        tmp = snakeBody[tailY][tailX];
        map[(tmp & 0xf0) >> 4] |= 0x80 >> (tmp & 0x0f);
    }
    else
    {
        map[headY] |= 0x80 >> headX;
    }

    for(i = 1; i <=8; i++)
        sendData((0x0100 * i) | map[i - 1]);
}

void go()
{
    uint8_t tmp;
    uint8_t tailX, tailY;
    uint8_t nextX, nextY;

    switch(dir)
    {
        case 1 << UP:
            nextX = headX;
            if(headY == 0)
                nextY = 7;
            else
                nextY = headY - 1;
            break;
        case 1 << DOWN:
            nextX = headX;
            if(headY == 7)
                nextY = 0;
            else
                nextY = headY + 1;
            break;
        case 1 << RIGHT:
            nextY = headY;
            if(headX == 7)
                nextX = 0;
            else
                nextX = headX + 1;
            break;
        case 1 << LEFT:
            nextY = headY;
            if(headX == 0)
                nextX = 7;
            else
                nextX = headX - 1;
            break;
        case 1 << STOP:
            break;
        default:
            break;
    }

    getTail(&tailX, &tailY);

    if(snakeBody[headY][headX] != 0xff)
    {
        snakeBody[tailY][tailX] = 0xff;
        snakeBody[nextY][nextX] = (headY << 4) | headX;
    }
    headY = nextY;
    headX = nextX;

    displayMap();
}

uint8_t appleX = 0, appleY = 3;

void eat()
{
    uint8_t tailX, tailY, tmp;
    if(headX == appleX && headY == appleY)
    {
        getTail(&tailX, &tailY);
        if(snakeBody[headY][headX] != 0xff)
        {
            tmp = snakeBody[tailY][tailX];
            tailY = (tmp & 0xf0) >> 4;
            tailX = tmp & 0x0f;
        }
        else
        {
            tailY = headY;
            tailX = headX;
        }

        switch(dir)
        {
            case 1 << UP:
                if(tailY == 7)
                    snakeBody[tailY][tailX] = (0 << 4) | tailX;
                else
                    snakeBody[tailY][tailX] = ((tailY + 1) << 4) | tailX;
                break;
            case 1 << DOWN:
                if(tailY == 0)
                    snakeBody[tailY][tailX] = (7 << 4) | tailX;
                else
                    snakeBody[tailY][tailX] = ((tailY - 1) << 4) | tailX;
                break;
            case 1 << LEFT:
                if(tailX == 7)
                    snakeBody[tailY][tailX] = (tailY << 4) | 0;
                else
                    snakeBody[tailY][tailX] = (tailY << 4) | (tailX + 1);
                break;
            case 1 << RIGHT:
                if(tailX == 0)
                    snakeBody[tailY][tailX] = (tailY << 4) | 7;
                else
                    snakeBody[tailY][tailX] = (tailY << 4) | (tailX - 1);
                break;
            case 1 << STOP:
                break;
            default:
                break;
        }

        displayMap();

    }
}

void initSnakeBody()
{
    uint8_t i, j;
    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
            snakeBody[i][j] = 0xff;
}

int main()
{
    uint16_t delayCounter;
    uint16_t key;

    initDelay();
    init7219();
    initSnakeBody();
    /*
    snakeBody[0][0] = 0x01;
    snakeBody[0][1] = 0x02;
    snakeBody[0][2] = 0x03;
    snakeBody[0][3] = 0x04;
    */

    while(1)
    {
        key = keyScan();
        if(key)
        {
            switch(key)
            {
                // These should be modified depending on your keyboard
                case 1 << 10:
                    if(dir != 1 << RIGHT)
                        dir = 1 << LEFT;
                    break;
                case 1 << 13:
                    if(dir != 1 << UP)
                        dir = 1 << DOWN;
                    break;
                case 1 << 5:
                    if(dir != 1 << DOWN)
                        dir = 1 << UP;
                    break;
                case 1 << 8:
                    if(dir != 1 << LEFT)
                        dir = 1 << RIGHT;
                    break;
                //case 1 << 9:
                //    dir = 1 << STOP;
                    break;
                default:
                    break;
            }
        }
        go();
        eat();
        delayCounter = 0x7fff;
        while(delayCounter--);
    }
    return 0;
}

