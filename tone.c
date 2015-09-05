#include <stc12.h>
#define FREQ 11529  // kHz

unsigned long hundredSeconds = 0;

void timer0() __interrupt(TF0_VECTOR)
{
    hundredSeconds++;
}

void delay(unsigned long ms)
{
    unsigned long lastTime;
    lastTime = hundredSeconds;
    while((hundredSeconds-lastTime) < ms*10);
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

void play()
{
	unsigned char i;
	for (i = 0; i < 100; i++) {
		P1_1 = 1;
		delay(3);
		P1_1 = 0;
		delay(3);
	}
	delay(100);
	for (i = 0; i < 100; i++) {
		P1_1 = 1;
		delay(2);
		P1_1 = 0;
		delay(2);
	}
	delay(100);
    for (i = 0; i < 100; i++) {
		P1_1 = 1;
		delay(2);
		P1_1 = 0;
		delay(2);
	}
	delay(100);
	for (i = 0; i < 100; i++) {
		P1_1 = 1;
		delay(3);
		P1_1 = 0;
		delay(3);
	}
}

int main()
{
    initDelay();

    P1M0 = 0x02;
    P1_0 = 0;
    while(1)
    {
        play();
        P1_0 = ~P1_0;
        delay(1000);
    }
    return 0;
}

