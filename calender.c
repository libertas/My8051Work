// Running on a STC15F204EA at 11.0529 MHz

#include <stc12.h>

// Version A only
/*
#define RXB P0_0
#define TXB P0_1
*/

// Version B only
/*
#define RXB P5_4
#define TXB P5_5
*/

#define BAUD 0xfe80


#define D_A P3_2
#define D_B P3_3
#define D_C P3_4
#define D_D P3_5
#define D_L1 P3_6
#define D_L2 P3_7

#define M_A P1_0
#define M_B P1_1
#define M_C P1_2
#define M_D P1_3
#define M_L1 P1_4

#define W_A P1_5
#define W_B P1_6
#define W_C P1_7
#define W_D P0_0

#define RST P0_1

unsigned long millis = 0;

char day = 1, month = 1, dayofweek = 1;
__code const char table[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void refresh_pins()
{
	D_A = (day % 10 & 0x01);
	D_B = (day % 10 & 0x02);
	D_C = (day % 10 & 0x04);
	D_D = (day % 10 & 0x08);
	D_L1 = (day / 10 & 0x01);
	D_L2 = (day / 10 & 0x02);
	
	M_A = (month % 10 & 0x01);
	M_B = (month % 10 & 0x02);
	M_C = (month % 10 & 0x04);
	M_D = (month % 10 & 0x08);
	M_L1 = (month / 10 & 0x01);
	
	W_A = (dayofweek & 0x01);
	W_B = (dayofweek & 0x02);
	W_C = (dayofweek & 0x04);
	W_D = (dayofweek & 0x08);
}


void uart() __interrupt(TF1_VECTOR)
{
	static unsigned int t;

	t++;

	if(t >= 20)
    {
        t = 0;
        millis += 1;
    }
}

void timer0() __interrupt(TF0_VECTOR)
{
	if(0 == RST)
	{
		day = 1;
		month = 1;
		dayofweek = 1;
		refresh_pins();
	}
}

void update()
{
	// check day
	if(day == table[month - 1])
	{
		day = 1;
		
		if(month == 12)
		{
			month = 1;
		}
		else
		{
			month++;
		}
	}
	else
	{
		day++;
	}
	
	// check day of week
	if(dayofweek == 6)
	{
		dayofweek = 8;
	}
	else if(dayofweek == 8)
	{
		dayofweek = 1;
	}
	else
	{
		dayofweek++;
	}
}

int main()
{
	unsigned long lasttime = 0;
	refresh_pins();
	
	P0M0 = 0xff;
	P1M0 = 0xff;
	P3M0 = 0xff;
	

    TMOD = 0x02;

    TR0 = 1;
    ET0 = 1;

	AUXR = 0x40;

	TL1 = BAUD;
	TH1 = BAUD >> 8;
	PT1 = 1;

    TR1 = 1;
    ET1 = 1;

	EA = 1;

    while(1)
    {
		lasttime = millis;
		while(millis - lasttime < 1000);
		if(RST)
		{
			update();
			refresh_pins();
		}
    }
    
    return 0;
}

