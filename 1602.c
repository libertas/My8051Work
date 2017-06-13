#include <8051.h>

#define _nop_()	__asm NOP __endasm

#define V0 P1_0
#define RS P1_1
#define RW P1_2
#define EN P1_3
#define DATA P3

volatile void delay()
{
  int i;
  for(i = 0; i < 100; i++) {
    _nop_();
  }
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
  int i, j;

  for(i = 0; i < 1000; i++) {
    delay();
  }

  V0 = 0;

  write_com(0x38);

  write_com(0x0f);

  write_com(0x06);
  write_com(0x01);
}

int main()
{

    unsigned char num = 0;
    unsigned char table[]="Test 1";
    unsigned char table1[]="Test 2";

    init();

    while(1){
      write_com(0x80);

      for(num=0;num<6;num++)
      {
          write_data(table[num]);
          delay();
      }

      write_com(0x80+0x40);

      for(num=0;num<6;num++)
      {
          write_data(table1[num]);
          delay();
      }
    }

    return 0;
}
