#include "define.h"

int ScreenWidth=160,ScreenHeight=80; //可变的屏幕宽高
int ScreenDouble=0;


//byte ScreenBufferX[320*336];
//byte ScreenBlack[320*8];
//byte ScreenKey[6400];

/*
typedef struct {
   BITMAPINFOHEADER bmiHeader; 
   RGBQUAD          bmiColors[256]; 
} BITMAPINFOX;

static BITMAPINFOX bmi,bmik,bmis;
*/

static byte palette[256*3];

static byte syspalette[]={
	0x00,0x00,0x00,0xbf,0xbf,0xbf, //KEY调色
	0x00,0x00,0x00,0x00,0xff,0x00, //MESS调色
	0x00,0xc0,0x00,0x00,0x00,0x00, //黑绿2色
	0xff,0xff,0xff,0x00,0x00,0x00  //黑白2色
};




void calcPalette()
{
	printf("calcPalette called.\n");

	/*
	int i;

	for (i=0;i<256;i++) {
		bmi.bmiColors[i].rgbBlue=palette[i*3];
		bmi.bmiColors[i].rgbGreen=palette[i*3+1];
		bmi.bmiColors[i].rgbRed=palette[i*3+2];
		bmi.bmiColors[i].rgbReserved=0;
	}*/
}

void Palette2()
{
	if (GRAY) //黑白变换
		memcpy(palette,syspalette+18,6);
	else
		memcpy(palette,syspalette+12,6);
	calcPalette();
}

void Palette16()
{
	int i;

	for (i=0;i<16;i++) {
		palette[i*3]=palette[i*3+1]=palette[i*3+2]=(15-i)*0x11;
	}
	calcPalette();
}

void Palette256()
{
	int i;
	byte lv9[]={0,0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0xff};
	byte lv5[]={0,0x40,0x80,0xc0,0xff};

	palette[0]=palette[1]=palette[2]=0;
	palette[255*3]=palette[255*3+1]=palette[255*3+2]=255;
	for (i=0;i<225;i++) {
		palette[i*3+48]=lv5[(i/5)%5];//lv4[(i>>6)&3];
		palette[i*3+49]=lv9[i/25];//lv8[(i>>3)&7];
		palette[i*3+50]=lv5[i%5];//lv8[(i)&7];
	}
	calcPalette();
}


void SetPalette()
{
	
	palette[255*3]=palette[255*3+1]=palette[255*3+2]=0;
	if (graph_mode==4) Palette16();
	else if (graph_mode==8) Palette256();
	else Palette2();
	
}

void lav_setpalette(byte from,int num,byte *addr)
{

	int i;

	for (i=from;i<from+num;i++) {
		palette[i*3+2]=*addr++;
		palette[i*3+1]=*addr++;
		palette[i*3]=*addr++;
		addr++;
	}
	calcPalette();
	
}

void Load_Palette()
{
	memcpy(palette,task[task_lev].palette,256*3);
	calcPalette();
}

void Save_Palette()
{
	memcpy(task[task_lev].palette,palette,256*3);
}

void SetSBuffer()
{
	
	//memset(BmpData,0,320*ScreenHeight);
	//memset(ScreenBuffer,0,320*8);
	//memset(ScreenBufferX,255,320*336); //清大屏
	//memset(ScreenBlack,0,320*8);
}

void InitScreen()
{
	int i;
	
	SetSBuffer();
	SetPalette();
}


void GetBmp(char *fname)
{

}

