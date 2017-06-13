#include <8051.h>

#define V0 P1_0
#define RS P1_1
#define RW P1_2
#define EN P1_3
#define DATA P0

volatile void delay()
{
  int i;
  for(i = 0; i < 10; i++);
}

void write(unsigned char c)
{
  RW = 0;
  EN = 0;
  DATA = c;
  delay();
  EN = 1;
  delay();
  EN = 0;
}

void write_com(unsigned char com)
{
  RS = 0;
  write(com);
}

void write_data(unsigned char data)
{
  RS = 1;
  write(data);
}

void init()
{
  V0 = 0;
}

int main()
{


    init();

    while(1)
    {
    }
    return 0;
}
