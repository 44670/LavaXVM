#include "define.h"

const byte mesCHAR[][8]={
	{0x38,0x4c,0xc6,0xc6,0xc6,0x64,0x38,0x00}, //0
	{0x18,0x38,0x18,0x18,0x18,0x18,0x7e,0x00}, //1
	{0x7c,0xc6,0x0e,0x3c,0x78,0xe0,0xfe,0x00}, //2
	{0x7e,0x0c,0x18,0x3c,0x06,0xc6,0x7c,0x00}, //3
	{0x1c,0x3c,0x6c,0xcc,0xfe,0x0c,0x0c,0x00}, //4
	{0xfc,0xc0,0xfc,0x06,0x06,0xc6,0x7c,0x00}, //5
	{0x3c,0x60,0xc0,0xfc,0xc6,0xc6,0x7c,0x00}, //6
	{0xfe,0xc6,0x0c,0x18,0x30,0x30,0x30,0x00}, //7
	{0x7c,0xc6,0xc6,0x7c,0xc6,0xc6,0x7c,0x00}, //8
	{0x7c,0xc6,0xc6,0x7e,0x06,0x0c,0x78,0x00}, //9
	{0x00,0x30,0x30,0x00,0x30,0x30,0x00,0}, //:
	{0x00,0x00,0x00,0x00,0x30,0x30,0x00,0}, //.
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0}, //
	{0x00,0x00,0x00,0xff,0x00,0x00,0x00,0}, //-
	{0x00,0x00,0x3c,0x3c,0x3c,0x3c,0x00,0},
	{0x0c,0x0c,0x18,0x10,0x00,0x60,0x60,0}, //!
	{0x3c,0x42,0x99,0xa1,0xa1,0x99,0x42,0x3c},
	{0x38,0x6c,0xc6,0xc6,0xfe,0xc6,0xc6,0x00}, //A
	{0xfc,0xc6,0xc6,0xfc,0xc6,0xc6,0xfc,0x00}, //B
	{0x3c,0x66,0xc0,0xc0,0xc0,0x66,0x3c,0x00}, //C
	{0xf8,0xcc,0xc6,0xc6,0xc6,0xcc,0xf8,0x00}, //D
	{0xfe,0xc0,0xc0,0xfc,0xc0,0xc0,0xfe,0x00}, //E
	{0xfe,0xc0,0xc0,0xfc,0xc0,0xc0,0xc0,0x00}, //F
	{0x3e,0x60,0xc0,0xce,0xc6,0x66,0x3e,0x00}, //G
	{0xc6,0xc6,0xc6,0xfe,0xc6,0xc6,0xc6,0x00}, //H
	{0x7e,0x18,0x18,0x18,0x18,0x18,0x7e,0x00}, //I
	{0x1e,0x06,0x06,0x06,0xc6,0xc6,0x7c,0x00}, //J
	{0xc6,0xcc,0xd8,0xf0,0xf8,0xdc,0xce,0x00}, //K
	{0x60,0x60,0x60,0x60,0x60,0x60,0x7e,0x00}, //L
	{0xc6,0xee,0xfe,0xfe,0xd6,0xc6,0xc6,0x00}, //M
	{0xc6,0xe6,0xf6,0xfe,0xde,0xce,0xc6,0x00}, //N
	{0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00}, //O
	{0xfc,0xc6,0xc6,0xc6,0xfc,0xc0,0xc0,0x00}, //P
	{0x7c,0xc6,0xc6,0xc6,0xde,0xcc,0x7a,0x00}, //Q
	{0xfc,0xc6,0xc6,0xce,0xf8,0xdc,0xce,0x00}, //R
	{0x78,0xcc,0xc0,0x7c,0x06,0xc6,0x7c,0x00}, //S
	{0x7e,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, //T
	{0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00}, //U
	{0xc6,0xc6,0xc6,0xee,0x7c,0x38,0x10,0x00}, //V
	{0xc6,0xc6,0xd6,0xfe,0xfe,0xee,0xc6,0x00}, //W
	{0xc6,0xee,0x7c,0x38,0x7c,0xee,0xc6,0x00}, //X
	{0x66,0x66,0x66,0x3c,0x18,0x18,0x18,0x00}, //Y
	{0xfe,0x0e,0x1c,0x38,0x70,0xe0,0xfe,0x00}}; //Z
	
void mesDrawChar(int c,int x,int y,int color,int bkcolor)
{
	return;

	int i,j,mask;
	char *p;

	switch (c) {
		case '.':c=0x3b;break;
		case ' ':c=0x3c;break;	
		case '-':c=0x3d;break;
		case '!':c=0x3f;break;
	}
	c-='0';
	
	for (i=0;i<8;i++) {
		p = ScreenBuffer + (y * 8 + i)*LCD_WIDTH + x * 8;
		mask=0x80;
		for (j=0;j<8;j++) {
			if (mesCHAR[c][i]&mask)
				*p=color;
			else
				*p=bkcolor;
			p++;
			mask>>=1;
		}
		
	}
}

void mesDrawString(char *s,int x,int y,int color,int bkcolor)
{
	while (*s) {
		mesDrawChar(*s,x,y,color,bkcolor);
		s++;
		x++;
	}
}

void mesDrawTitle()
{
	mesDrawString("      LVM V3.5      ",0,0,1,0);
}

void mesDrawTime()
{
	extern u32 ramuses,ramusee;
	int hour,min,sec;
	char s[20],*p;
	hour=TotalFrame/(3600*60);
	min=(TotalFrame/3600)%60;
	sec=(TotalFrame/60)%60;
	sprintf(s,"%3d:%2d:%2d",hour,min,sec);
	p=s+2;
	while (*p) {
		if (*p==0x20) *p='0';
		p++;
	}
	mesDrawString(s,11,0,1,0);
	sprintf(s,"%4x-%4x",ramuses,ramusee);
	p=s;
	while (*p) {
		if (*p>='a' && *p<='z') *p-=32;
		p++;
	}
	mesDrawString(s,0,0,1,0);
}
