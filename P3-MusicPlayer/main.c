
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "avr.h"
#include "lcd.h"
#include <time.h>

//base freqs
#define A	227
#define Bf	215
#define B	202
#define C	191
#define Cs	180
#define D	170
#define Ds	161
#define E	152
#define F	143
#define Fs	135
#define G	128
#define Af	120

//durations
#define W	1
#define H	.5
#define Q	.25
#define Ei	.125
#define S	.0625

char buf[17];

struct note 
{
	int freq;
	float duration;	
};

struct song
{
	char name[17];
	char artist[17];
	const struct note *instrumental;	
	int length;
};

const struct note maskOff[18] = { {D,Q+Ei},	{E,S}, {F,S}, {G,S}, {A,Ei}, {F,Ei}, {G,Q},
								{G,H+Q},	{G,S}, {A,S}, {G,S}, {F,S},
								{E,Q+Ei},	{D,S}, {E,S}, {D,S}, {C,Q}, {D,Q} };
									
const struct note portland[18] = { {A,Q+Ei}, {G,S}, {A,S}, {G,S}, {A,S}, {G,S}, {A,Ei}, {C,Q}, {C,Q},
								   {F,Q+Ei}, {G,S}, {A,S}, {G,S}, {A,S}, {G,S}, {A,Ei}, {C,Q}, {C,Q} };

struct song playlist[2] = { {"Mask Off", "Future", maskOff, 18}, 
							{"Portland", "Drake", portland, 18} };


void print_key(int key)
{
	lcd_clr();
	lcd_pos(1,0);
	sprintf(buf, "%02d", key);
	lcd_puts(buf);
}

void print_songTitle(const char* name, const char* artist)
{
	lcd_clr();
	lcd_pos(0,0);
	memset(buf, ' ', 17);
	sprintf(buf, "%s", name);
	lcd_puts(buf);
	memset(buf, ' ', 17);
	lcd_pos(1,0);
	sprintf(buf, "By %s", artist);
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

void print_msg(const char *msg)
{
	memset(buf, ' ', 17);
	sprintf(buf, msg);
	lcd_puts(buf);
}

void play_note(int freq, float duration)
{
	SET_BIT(DDRA, 0);
	int i;
	int k = duration*16*10000 / (freq);
	for (i=0; i<k; i++)
	{
		SET_BIT(PORTA, 0);
		avr_wait2(freq);
		CLR_BIT(PORTA, 0);
		avr_wait2(freq);
	}
}

void play_song(struct song mySong)
{
	print_songTitle(mySong.name, mySong.artist);
	int i = 0;
	int loop = 0;
	int nP = 1;
	while (i<mySong.length && loop < 2)
	{
		int key = get_press();
		if (key)
		{
			switch(key)
			{
				case 13:
					nP = 1;
					break;
				case 15:
					nP = 0;
					break;
				default:
					break;
			}	
		}
		if (nP)
		{
			play_note(mySong.instrumental[i].freq, mySong.instrumental[i].duration);
			avr_wait2(100);
			i++;
		}
		if (i >= mySong.length)
		{
			i = 0;
			loop++;
		}
	}
}

int main(void)
{
	int s = 0;
	lcd_init();
	lcd_clr();
	
	lcd_clr();
	sprintf(buf, "* to Start");
	lcd_pos(0,0);
	lcd_puts(buf);
	sprintf(buf, "# to Stop");
	lcd_pos(1,0);
	lcd_puts(buf);		
	avr_wait(5000);
	
	while(1)
	{		
		lcd_clr();
		sprintf(buf, "A : Mask Off");
		lcd_pos(0,0);
		lcd_puts(buf);
		sprintf(buf, "B : Portland");
		lcd_pos(1,0);
		lcd_puts(buf);
			
		int key = get_press();
		avr_wait(250);
		switch(key)
		{
			case 4:
				s = 0;
				play_song(playlist[s]);
				break;
			case 8:
				s = 1;
				play_song(playlist[s]);
				break;
			case 13:
				s = 0;
				play_song(playlist[s]);
				avr_wait(2000);
				s = 1;
				play_song(playlist[s]);
				break;
			default:
				break;
		}
	}
}