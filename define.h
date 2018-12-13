#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <SDL.h>
#include <SDL_video.h>
#include <time.h>
//#include <windows.h>
#define USE_SDL (1)
#define USE_WINDOWS_API (1)
#define PLAT_TICK_PERIOD_MS (1)
#else
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#define USE_ESP32 (1)
#define USE_POSIX_API (1)
#define PLAT_TICK_PERIOD_MS (portTICK_PERIOD_MS)
#endif

#define CHECKRAM (0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;


typedef uint8_t byte;

#define CheckAddr(a,b,c) 


#define PLAT_SEEK_SET (0)
#define PLAT_SEEK_CUR (1)
#define PLAT_SEEK_END (2)

u32 platGetTickCount();
void platSleep(u32 ms);
u32 platGetKeyboardState();
void* platFopen(char* path, char* mode);
int platFwrite(void* fp, u8* buf, int size);
int platFread(void* fp, u8* buf, int size);
int platFseek(void* fp, int offset, int mode);
int platFgetsize(void* fp);
u32 platFgetlastwritetime(void* fp);
int platFdelete(char* path);
int platFmkdir(char* path);
int platFclose(void* fp);
u32 platFgetattr(void* fp);
int platFtell(void* fp);

#define PLAVA_SIZE (96 * 1024)
#define LRAM_SIZE (32 * 1024)

#define MAX_PATH (256)
#define CALLBACK

//#define TextBuffer		0xf000 //故意这样，以检验是否地址相关
//#define string_stack		0xf200
//#define patbuf			0xfe00
//#define eval_stack		0xff00

#define LCD_WIDTH	160
#define LCD_HEIGHT	80
#define	SCROLL_CON	LCD_WIDTH*(LCD_HEIGHT+8)

#define FATAL(info) printf("FATAL!!!: %s\n", info);getchar();
#define FATAL_printf printf

#define WM_LAVASTAR 0xb800
#define WM_STOPLINE 0xb801
#define	WM_STAREND  0xb802
#define	WM_STARSTOP 0xb803

#define XM_LBUTTONDOWN	1
#define XM_LBUTTONUP	2
#define XM_MOUSEMOVE	3 


struct TASK
{
	int		attrib;
	char	CD[MAX_PATH];
	int		first_file;
	int		curr_file;
	int		list_set;
	int		RamBits;
	u16	graph_mode;
	u16	bgcolor;
	u16	fgcolor;
	byte	palette[256*3];
	int		ScreenWidth;
	int		ScreenHeight;
	byte	secret;
	byte	*pLAVA;
	byte	*lRam;
	byte	*lPC;
	u32		local_sp;
	u32		local_bp;
	byte	eval_top;
	u32		string_ptr;
};

struct MESSAGE
{
	u16 type;
	u32 wParam;
	u32 lParam;
};


extern int EmuRunning,PauseFlag,RamError,stop_line,stop_func,func_top;
extern int sline[256],sline_num;
extern int TVSCAN,GRAY,ENDVERIFY;
extern int ScreenWidth,ScreenHeight,ScreenDouble; //可变的屏幕宽高
extern int TotalFrame;
extern byte SaveTemp[0x80];
extern char* FcPath;
extern char* BmpPath;
extern char* CheatPath;
extern char *pRomName;
extern byte lRam[LRAM_SIZE];
extern struct TASK task[16]; //任务栈
extern int task_lev; //任务级
extern byte *ascii;//[1536];
extern byte *ascii8;//[2048];
extern byte *gbfont;//[81*94*24];
extern byte *gbfont16;//[81*94*32];
extern byte *pinyin;
extern byte ScreenBuffer[];
extern byte ScreenBufferX[];
extern byte ScreenKey[6400];
#define BmpData ((byte*)ScreenBuffer)
extern u16 graph_mode;
extern byte* pLAVA;
extern s32 lDelay;
extern u32 timed;
extern byte old_keyb[256],cur_keyb[256];
extern byte lav_key;
extern byte wait_key;
extern int have_pen,have_keypad;
extern int pen_x,pen_y;
extern byte hardinput_rp,hardinput_wp; //硬件输入消息队列的读写指针
extern struct MESSAGE hardinput[256]; //LavaX虚拟机的硬件输入消息队列

extern int LavOpen(void *fp);
extern int TaskOpen(char *name);
extern void SetWindow(int width,int height);
extern void good_exit();
extern void SetSBuffer();
extern void InitScreen();
extern void WriteScreen(int flag);
extern void GetBmp(char *fname);
extern void SetPalette();
extern void lav_setpalette(byte from,int num,byte *addr);
extern void Save_Palette();
extern void Load_Palette();
extern void mesDrawTitle();
extern void mesDrawTime();
extern void lavReset();
extern void lavRun();
extern void verify();
extern void check();
extern void chtLoad();
extern void chtExe();
extern void SelFile(char *path);
extern void file_list_key(char *path,int key);
extern void get_file_name(char *path,int index,char *name);
extern int getfilenum(char *path);
extern int getfilenum_ex(char *path,char *ext);
extern int findfile(int index,int num,char *namebuf,char *path);
extern int findfile_ex(int index,int num,char *namebuf,char *path,int namelen,char *ext);
//extern LRESULT CALLBACK ChtFind(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
//extern LRESULT CALLBACK ChtEdit(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
#ifdef __cplusplus
}
#endif
