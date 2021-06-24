#include <stdio.h>
#include <string.h>
#include "avr.h"
#include "lcd.h"

char buf[17];

void print_key(int key)
{
	lcd_clr();
	lcd_pos(1,0);
	sprintf(buf, "%02d", key);
	lcd_puts(buf);
}

int is_pressed(int row, int col)
{
	DDRC = 0;
	PORTC = 0;
	SET_BIT(DDRC, row);
	CLR_BIT(PORTC, row);
	SET_BIT(PORTC, col+4);
	CLR_BIT(PORTC, col+4);
	avr_wait(1);
	if (GET_BIT(PINC, col+4))
	{
		return 0;
	}
	return 1;
}

int get_press()
{
	int row, col;
	for (row = 0; row < 4; row++)
	{
		for (col = 0; col < 4; col++)
		{
			if (is_pressed(row, col))
			{
				return (row*4)+col+1;
			}
		}
	}
	return 0;
}

void send_stats(int offON, float iv, float hi, float lo, float av)
{
	lcd_clr();
	lcd_pos(0,0);
	sprintf(buf, "IV:%.2f", iv);
	lcd_puts(buf);
	
	if (offON)
	{
		lcd_pos(0,9);
		sprintf(buf, "HI:%.2f", hi);
		lcd_puts(buf);
		lcd_pos(1,0);
		sprintf(buf, "AV:%.2f", av);
		lcd_puts(buf);
		lcd_pos(1,9);
		sprintf(buf, "LO:%.2f", lo);
		lcd_puts(buf);	
	}
	else
	{
		lcd_pos(0,9);
		sprintf(buf, "HI:-.--");
		lcd_puts(buf);
		lcd_pos(1,0);
		sprintf(buf, "AV:-.--");
		lcd_puts(buf);
		lcd_pos(1,9);
		sprintf(buf, "LO:-.--");
		lcd_puts(buf);
	}

}


int get_sample()
{
	SET_BIT(ADMUX, REFS0);
	SET_BIT(ADCSRA, 7);
	SET_BIT(ADCSRA, 6);
	while(GET_BIT(ADCSRA, 6));
	return ADC;
}

int main(void)
{
	///unsigned int s, min, max, avg, count;
	///unsigned long sum;
	int offON = 0;
	float sample = 0;
	float max = 0;
	float min = 1024;
	float count = 0;
	long sum = 0;
	lcd_init();
	lcd_clr();
	
	while(1)
	{					
		int key = get_press();
		avr_wait(250);
		if (key == 12)
		{
			offON = !offON;
		}
		else if (key == 16)
		{
			offON = 0;
			max = 0;
			min = 1024;
			count = 0;
			sum = 0;
		}
		sample = (float)get_sample();
		if (offON)
		{
			sum += sample;
			count++;
			if (sample < min) {min = sample;}
			if (sample > max) {max = sample;}
		}
		send_stats(offON, ((sample / 1023.0) * 5.0),
		((max / 1023.0) * 5.0),
		((min / 1023.0) * 5.0),
		((float)((sum / 1023.0) * 5.0) / count));
		
		avr_wait(500);
	}
}