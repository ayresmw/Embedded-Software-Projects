
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "avr.h"
#include "lcd.h"
#include <time.h>

struct dt
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int mt;
} myTime;

int dim[13]  = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char buf[17];

char keypad[17] = 
{'1', '2', '3', 'A',
'4', '5', '6', 'B',
'7', '8', '9', 'C',
'*', '0', '#', 'D'};

void dt_init()
{
	myTime.year = 2021;
	myTime.month = 4;
	myTime.day = 30;
	myTime.hour = 4;
	myTime.minute = 20;
	myTime.second = 03;
	myTime.mt = 1;
}

void print_key(int key)
{
	lcd_clr();
	lcd_pos(1,0);
	sprintf(buf, "%02d", key);
	lcd_puts(buf);
}

void print_time()
{
	lcd_clr();
	lcd_pos(0,0);
	memset(buf, ' ', 17);
	sprintf(buf, "%02d/%02d/%04d", myTime.month, myTime.day, myTime.year);
	lcd_puts(buf);
	memset(buf, ' ', 17);
	lcd_pos(1,0);
	if (myTime.mt)
	{
		sprintf(buf, "%02d:%02d:%02d", myTime.hour, myTime.minute, myTime.second);
	}
	else
	{
		sprintf(buf, "%02d:%02d:%02d %s", myTime.hour % 12 == 0 ? 12 : myTime.hour % 12, myTime.minute, myTime.second, myTime.hour >= 12 ? "PM" : "AM");
	}
	lcd_puts(buf);

}

void pass_time()
{
	avr_wait(1000);
	myTime.second++;
	if (myTime.second >= 60)
	{
		myTime.second = 0;
		myTime.minute++;
	}
	if (myTime.minute >= 60)
	{
		myTime.minute = 0;
		myTime.hour++;
	}
	if (myTime.hour >= 24)
	{
		myTime.hour = 0;
		myTime.day++;
	}
	if (myTime.day > dim[myTime.month])
	{
		if ((myTime.month == 2) && (((myTime.year % 4 ==0) && (myTime.year != 0)) || myTime.year % 400 == 0))
		{
			if (myTime.day == 28)
			{
				return;
			}
			else if (myTime.day > 29)
			{
				myTime.day = 0;
				myTime.month++;
			}
		}
		else
		{
			myTime.day = 0;
			myTime.month++;
		}
	}
	if (myTime.month >= 13)
	{
		myTime.month = 1;
		myTime.year++;
	}
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

void get_command(int key)
{
	switch(key)
	{	
		case 15:
			myTime.mt = !myTime.mt;
			break;
		default:
			break;		
	}
}

void print_msg(const char *msg)
{
	memset(buf, ' ', 17);
	sprintf(buf, msg);
	lcd_puts(buf);
}

void edit_date()
{
	int key;
	char edit[17];
	int i = 0;
	int it[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	lcd_clr();
	lcd_pos(0,0);
	print_msg("EDIT DATE");
	avr_wait(2000);
	while(i < 8)
	{
		lcd_clr();
		lcd_pos(0,0);
		memset(edit, ' ', 17);
		sprintf(edit,"EM: %01d%01d/%01d%01d/%01d%01d%01d%01d",it[0],it[1], it[2],it[3], it[4],it[5],it[6],it[7]);
		print_msg(edit);
		key = get_press();
		avr_wait(200);
		if (key != 0)
		{
			it[i] = keypad[key-1] - '0';
			i++;
		}
		continue;
	}
	lcd_clr();
	lcd_pos(0,0);
	memset(edit, ' ', 17);
	sprintf(edit,"EM: %01d%01d/%01d%01d/%01d%01d%01d%01d",it[0],it[1], it[2],it[3], it[4],it[5],it[6],it[7]);
	print_msg(edit);
	myTime.month = it[0]*10 + it[1] > 12 ? 1 : it[0]*10 + it[1];
	myTime.day = it[2]*10 + it[3] > 31 ? 1 : it[2]*10 + it[3];
	myTime.year = it[4]*1000 + it[5]*100 + it[6]*10 + it[7];
	avr_wait(1000);
}

void edit_time()
{
	int key;
	char edit[17];
	int i = 0;
	int it[6] = {0, 0, 0, 0, 0, 0};
	lcd_clr();
	lcd_pos(0,0);
	print_msg("EDIT TIME");
	avr_wait(2000);
	while(i < 6)
	{
		lcd_clr();
		lcd_pos(1,0);
		memset(edit, ' ', 17);
		sprintf(edit,"EM: %01d%01d:%01d%01d:%01d%01d",it[0],it[1], it[2],it[3], it[4],it[5]);
		print_msg(edit);
		key = get_press();
		avr_wait(200);
		if (key != 0)
		{
			it[i] = keypad[key-1] - '0';
			i++;
		}
		continue;
	}
	lcd_clr();
	lcd_pos(1,0);
	memset(edit, ' ', 17);
	sprintf(edit,"EM: %01d%01d:%01d%01d:%01d%01d",it[0],it[1], it[2],it[3], it[4],it[5]);
	print_msg(edit);
	myTime.hour = it[0]*10 + it[1];
	myTime.minute = it[2]*10 + it[3];
	myTime.second = it[4]*10 + it[5];
	avr_wait(1000);
}

int main(void)
{
	dt_init();
	lcd_init();
	lcd_clr();
	
	while(1)
	{
		int key = get_press();	
		if (key != 0)
		{
			switch(key)
			{
				case 13:
					myTime.mt = !myTime.mt;
					break;
				case 15:
					edit_date();
					edit_time();
					break;
				default:
					break;
			}
		}
		print_time();
		pass_time();
	}
	
}