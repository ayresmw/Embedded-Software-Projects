#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avr.h"
#include "lcd.h"

char buf[17];

void print_key(int key)
{
	lcd_clr();
	lcd_pos(1,0);
	sprintf(buf, "%d", key);
	lcd_puts(buf);
}

void print_msg(const char *msg, int tb)
{
	lcd_pos(tb,0);
	sprintf(buf, msg);
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

void send_stats(float iv)
{
	lcd_clr();
	lcd_pos(0,0);
	sprintf(buf, "IV:%.2f", iv);
	lcd_puts(buf);
}


int get_sample()
{
	SET_BIT(ADMUX, REFS0);
	SET_BIT(ADCSRA, 7);
	SET_BIT(ADCSRA, 6);
	while(GET_BIT(ADCSRA, 6));
	return ADC;
}

void flashLED(int num, int wait)
{
	avr_wait(wait);
	SET_BIT(PORTA, num);
	avr_wait(wait);
	CLR_BIT(PORTA, num);
	
}

void flashRandom(int min, int max, int count, int *answers)
{
	for (int i = 0; i < count; i++)
	{
		int num = (rand() % (max - min + 1)) + min;
		answers[i] = num;
		flashLED(num, 500);
	}
}

int gameStart(int difficulty)
{
	int key;
	int answers[4] = {};
	int results[4] = {};
	int i = 0;
	int led = 0;
	char buf[17] = "";
	switch(difficulty)
	{
		case 4:
		print_msg("Easy Mode", 0);
		break;
		case 8:
		print_msg("Normal Mode", 0);
		break;
		case 12:
		print_msg("Hard Mode", 0);
		break;
		case 16:
		print_msg("Very Hard Mode", 0);
		break;
		default:
		print_msg("Debug Mode", 0);
		break;
	}	
	flashRandom(0,3,difficulty, answers);
	while(i < difficulty)
	{
		while((key = get_press()) == 0);
		if ((key > 0) && (key < 5))
		{
			switch(key)
			{
				case 1:
				led = 3;
				break;
				case 2:
				led = 2;
				break;
				case 3:
				led = 1;
				break;
				case 4:
				led = 0;
				break;
				default:
				break;
			}
			flashLED(led, 250);
			results[i] = led;
			i++;
		}
		while(get_press() != 0);
	}
	avr_wait(150);
	for(int j=0; j < difficulty; j++)
	{
		if (answers[j] != results[j])
		{
			return 0;
		}
	}
	return 1;
}

int main(void)
{	
	int key;
	int offON = 0;
	lcd_init();
	lcd_clr();
	SET_BIT(DDRA, 0);
	SET_BIT(DDRA, 1);
	SET_BIT(DDRA, 2);
	SET_BIT(DDRA, 3);
	int difficulty = 4;
	unsigned long seed = 0;
	while(1)
	{
		while((key = get_press()) == 0)
		{
			if (offON)
			{
				lcd_clr();
				int wl = gameStart(difficulty);
				if (wl == 1)
				{
					lcd_clr();
					print_msg("Congrats!", 0);
					print_msg("You Won!", 1);
					for (int i = 0; i < difficulty*2; i++)
					{
						int num = (rand() % (3 - 0 + 1)) + 0;
						flashLED(num, 50);
					}
					avr_wait(2000);
					lcd_clr();
				}
				else
				{
					lcd_clr();
					print_msg("You Lose!", 0);
					print_msg("Try Again?", 1);
					avr_wait(2000);
					lcd_clr();
				}
				offON = 0;
			}
			seed += 1;
			lcd_clr();
			//print_key(seed);
			print_msg("Waiting to Play..", 0);
			print_msg("A,B,C,D to Start", 1);
		}
		if (key == 4 || key == 8 || key == 12 || key == 16)
		{
			srand(seed);
			offON = !offON;
			difficulty = key;
		}
		while(get_press() != 0);
	}
}