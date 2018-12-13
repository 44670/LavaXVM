
#include	"define.h"



#define MAX_PATH (512)

char *CommandLine;
SDL_Surface *screen;

#ifdef USE_SDL
char* FcPath = "./";
char* BmpPath = "./bmp";
char* CheatPath = "./cheat";
char* RomPath = "./";
#endif

#ifdef USE_ESP32
char* FcPath = "/sd/";
char* BmpPath = "/sd/bmp/";
char* CheatPath = "/sd/cheat";
char* RomPath = "/sd/";
#endif

char RomName[MAX_PATH];
char* pRomName;
unsigned long FREQ;
unsigned long FREQ60,FREQ256;
int TVSCAN,GRAY,ENDVERIFY,CHEATUSE;
int EmuRunning,PauseFlag,RamError;
int Frame;
int CurFrame,TotalFrame;
int stop_line=-1,stop_func,sline[256],sline_num;
unsigned long timel,times,IdleParam;
// pLAVA: pointer to current lava rom
//byte *pAllLAVA,*pLAVA,*pNextLAVA;
byte old_keyb[256],cur_keyb[256];
struct MESSAGE hardinput[256]; //LavaX虚拟机的硬件输入消息队列
byte hardinput_rp,hardinput_wp; //硬件输入消息队列的读写指针
byte lav_key;
byte KeyID;

//HWND hwnd_main, hwnd_ide;


void EmuRun();
void FileOpenX();




static CenterWindow()
{
}

void SetWindow(int width,int height)
{
	printf("SetWindow: %d %d\n", width, height);
}

static void CloseRom()
{
	if (EmuRunning) good_exit();
	EmuRunning=0; //释放ROM所占内存
	PauseFlag=1;
	mesDrawTitle();

}




void RomReset()
{
	RamError=0;
	CurFrame=0;
	TotalFrame=0;
	//memset(BmpData,0,320*ScreenHeight); //清屏
	//memset(ScreenBuffer,0,320*8);
	lavReset();
}

byte* getLVMBinPointer() {
	FILE *f = fopen("LVM.bin", "rb");
	int fileSize = 454450;
	byte* buf = malloc(fileSize);
	fread(buf, 454450 , 1, f);
	return buf;
}
#undef main
void WriteScreen() {
	u32 pitch = (screen->pitch) / 2;
	u16* dstBuf = (u16*) screen->pixels;
	u16* pix;
	u8* src = ScreenBuffer;
	int y, x;
	for (y = 0; y < LCD_HEIGHT * 2 + 16; y++) {
		pix = dstBuf + pitch * y;
		for (x = 0; x < LCD_WIDTH ; x++) {
			*pix = (*src) ? 0x0000 : 0xffff;
			pix++;
			src++;
		}
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);

}

int  main()
{
	char *p;
	int xx,yy;

	stop_func=0;
	func_top=0;
	pLAVA = malloc(128 * 1024);
	/*
	CommandLine=GetCommandLine();
	p=strstr(CommandLine,"/S");
	if (p) stop_line=getnumber(p+2);
	else stop_line=-1;
	p=strstr(CommandLine,"/H");
	if (p) hwnd_ide=(HWND)getnumber(p+2);
	else hwnd_ide=0;
	p=strstr(CommandLine,"/E");
	if (p) CommandLine=p+2;
	else CommandLine=0;
	hwnd_main=0;
	*/
	/*
	pAllLAVA = AllLAVA;
	
	if ((pAllLAVA=malloc(200 * 1024))==0) {
		FATAL("内存不足!");
		return 0;
	} //为LAVA程序申请1M内存,以后可以加大。
	*/
	task[0].pLAVA=pLAVA;


	//hInstan=hInstance;
	//GetFcePath();
	
	/*
	hr=FindResource(NULL,(char *)IDR_BIN1,"BIN");
	hg=LoadResource(NULL,hr);*/
	byte* hg = getLVMBinPointer();
	ascii=hg;
	ascii8=ascii+1536;
	gbfont=ascii8+2048;
	gbfont16=gbfont+81*94*24;
	pinyin=gbfont16+81*94*32+0x3e+6400;
	//memcpy(ScreenKey,gbfont16+81*94*32+0x3e,6400);

	/*
	strcpy(Ini,FcPath);
	strcat(Ini,"lavax.ini");
	if ((hFile=CreateFile(Ini,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_ARCHIVE,NULL))!=INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	GetPrivateProfileString("ROM","Directory",FcPath,RomPath,MAX_PATH,Ini);
	SetCurrentDirectory(RomPath);
	if ((FREQ=GetPrivateProfileInt("CPU","Frequency",0,Ini))==0) {
		char Number[8];
		FREQ=GetMHz();
		sprintf(Number,"%d",FREQ);
		WritePrivateProfileString("CPU","Frequency",Number,Ini);
	}
	if ((IdleParam=GetPrivateProfileInt("CPU","IdelParam",0,Ini))==0) {
		char Number[8];
		IdleParam=GetIdleParam();
		sprintf(Number,"%d",IdleParam);
		WritePrivateProfileString("CPU","IdelParam",Number,Ini);
	}
	strcpy(BmpPath,FcPath);
	strcat(BmpPath,"Bmp\\");
	CreateDirectory(BmpPath,NULL);
	strcpy(CheatPath,FcPath);
	strcat(CheatPath,"Cheat\\");
	CreateDirectory(CheatPath,NULL);
	*/
	IdleParam = 9;
	FREQ = PLAT_TICK_PERIOD_MS * 1000;
	FREQ60=FREQ/60;
	FREQ256=FREQ/256;
	TVSCAN=0;
	CHEATUSE=0;
	EmuRunning=0;
	InitScreen();
	LoadLavOrPacFile("boot.lav");
#ifdef USE_SDL
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_Event evt;
	while (1) {
		SDL_PollEvent(&evt);
		EmuRun();
	}
#endif

	/*
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc=(WNDPROC)WndProc;
	wc.hInstance=hInstan;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="MyLvmClass";
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hIcon=LoadIcon(hInstan,(LPCSTR)IDI_ICON1);
	RegisterClass(&wc);
	hMenu=LoadMenu(hInstance,(LPCSTR)IDR_MENU1);
	xx=GetSystemMetrics(SM_CXFIXEDFRAME)*2;
	yy=GetSystemMetrics(SM_CXFIXEDFRAME)*2+GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYCAPTION);
	hwnd_main=CreateWindow("MyLvmClass","LavaX Virtual Machine",WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX,
		0,0,xx+320,yy+344,NULL,hMenu,hInstance,NULL);
	mesDrawTitle();
	CenterWindow();
	ShowWindow(hwnd_main,nCmdShow);	
	UpdateWindow(hwnd_main);
	PauseFlag=1;
	hAccelTable=LoadAccelerators(hInstance,(LPCTSTR)IDR_ACCELERATOR1);
	
	
	if (CommandLine) FileOpenX();
	while(TRUE) {
		if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0 )) break;
			if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
			EmuRun();
	}
	GetCurrentDirectory(MAX_PATH,RomPath);
	WritePrivateProfileString("ROM","Directory",RomPath,Ini);
	if (hwnd_ide) SendMessage(hwnd_ide,WM_STAREND,0,0);
	*/
	return 0;
}

int LavOpen(FILE *fp)
{
	long filelen;
	
	fseek(fp,0,SEEK_END);
	filelen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	fread(pLAVA,1,filelen,fp);
	*(pLAVA+filelen)=0;
	//pNextLAVA=pLAVA+filelen+16;
	fclose(fp);
	return 1;
}

void FileChange(char *fname,char *ram,long len)
{
	FILE *fp;
	long filelen,i;

	fp=fopen(fname,"rb");
	if (fp!=NULL) {
		fseek(fp,0,SEEK_END);
		filelen=ftell(fp);
		if (filelen==len) {
			rewind(fp);
			for (i=0;i<len;i++) {
				if ((char)getc(fp)!=ram[i]) break;
			}
			if (i>=len) {
				fclose(fp);
				return;
			}
		}
		fclose(fp);
	}
	fp=fopen(fname,"wb");
	if (fp!=NULL) {
		fwrite(ram,1,len,fp);
		fclose(fp);
	}
}

int UnPac(char *pacname,char *zname)
{
	FILE *fp;
	char fname[MAX_PATH];
	long SizeRead;
	byte t[32];
	short total;
	char head[64];
	long filelen;
	char Path[64];
	char *p;
	int i,j;
	int have_lav;

	have_lav=0;
	if ((fp=fopen(pacname,"rb"))==NULL) {
		FATAL("无法打开PAC文件!");
		return 0;
	}
	fseek(fp,0,SEEK_SET);
	SizeRead=fread(t,1,16,fp);
	if (SizeRead!=16 || t[0]!='P' || t[1]!='A' || t[2]!='C' || t[3]!=' ') {
		fclose(fp);
		FATAL("不是有效的PAC文件!");
		return 0;
	}
	fread(&total,1,2,fp);
	for (i=0;i<total;i++) {
		fread(head,1,64,fp);
		filelen=*(long *)head;
		if (filelen) {
			strcpy(Path,head+4);
			p=NULL;
			for (j=0;j<60;j++) {
				if (Path[j]=='/') {
					//Path[j]='\\';
					p=Path+j;
				}
			}
			if (strstr(Path+strlen(Path)-4,".lav")) {
				have_lav++;
				if (have_lav==1) {
					strcpy(zname,FcPath);
					strcat(zname,Path);
				}
			}
			strcpy(fname,FcPath);
			strcat(fname,Path);
			if (p) {
				for (j=0;j<60;j++) {
					if (!fname[j]) break;
					if (fname[j]=='/') {
						fname[j]=0;
						mkdir(fname,NULL);
						fname[j]='/';
					}
				}
			}
			p=(char *)malloc(filelen);
			if (p==NULL) {
				fclose(fp);
				return 0;
			}
			fread(p,1,filelen,fp);
			FileChange(fname,p,filelen);
			free(p);
		}
	}
	fclose(fp);
	if (!have_lav) zname[0]=0;
	return 1;
}

int LoadLavOrPacFile(char* fname)
{
	FILE *fp;
	long SizeRead;
	byte t[32];
	char zname[MAX_PATH];
	strcpy(zname, fname);
abc:
	if ((fp=fopen(zname,"rb"))==NULL) {
		FATAL("无法打开LavaX文件!");
		FATAL_printf("%s\n", zname);
		return 1;
	}
	fseek(fp,0,SEEK_SET);
	SizeRead=fread(t,1,16,fp);
	if (t[0]=='L' && t[1]=='A' && t[2]=='V' &&
		t[3]==18 && SizeRead==16) {
		if (EmuRunning) CloseRom();
		//pLAVA=pAllLAVA;
		if (!LavOpen(fp)) return 2;
	} else if (t[0]=='P' && t[1]=='A' && t[2]=='C' &&
		t[3]==' ' && SizeRead==16) {
		fclose(fp);
		if (!UnPac(RomName,zname)) return 1;
		if (zname[0]==0) return 1;
		goto abc;
	} else {
		FATAL("不是有效的LavaX文件!");
		fclose(fp);
		return 1;
	}
	task_lev=0;
	EmuRunning=1;
	PauseFlag=0;

	pRomName=RomName;//+ofn.nFileOffset;
	/*
	if (ofn.nFileExtension)
		*(RomName+ofn.nFileExtension-1)=0;
		*/
	RomReset();
	return 0;
	//chtLoad();
}

/*
int TaskOpen(char *name)
{
	FILE *fp;
	long SizeRead;
	byte t[32];

	if ((fp=fopen(name,"rb"))==NULL) return 1;
	SizeRead=fread(t,1,16,fp);
	if (t[0]=='L' && t[1]=='A' && t[2]=='V' &&
		t[3]==18 && SizeRead==16) {
		pLAVA=pNextLAVA;
		task[task_lev+1].pLAVA=pLAVA;
		LavOpen(fp);
	} else {
		fclose(fp);
		return 1;
	}
	return 0;
}
*/


#if 0
void FileOpenX()
{
	FILE *fp;
	long SizeRead;
	byte t[32];
	char buf[MAX_PATH];
	char *p;
	int i;

	if ((fp=fopen(CommandLine,"rb"))==NULL) {
		FATAL("无法打开LavaX文件!");
		exit(0);
	}
	fseek(fp,0,SEEK_SET);
	SizeRead=fread(t,1,16,fp);
	if (t[0]=='L' && t[1]=='A' && t[2]=='V' &&
		t[3]==18 && SizeRead==16) {
		if (EmuRunning) CloseRom();
		pLAVA=pAllLAVA;
		if (!LavOpen(fp)) return;
	} else {
		FATAL("不是有效的LavaX文件!");
		fclose(fp);
		exit(0);
	}
	task_lev=0;
	EmuRunning=1;
	PauseFlag=0;
	RomReset();
	GetFullPathName(CommandLine,MAX_PATH,buf,&p);
	strcpy(RomName,p);
	for (i=0;;i++) {
		if (!RomName[i]) break;
		if (RomName[i]=='.') {
			RomName[i]=0;
			break;
		}
	}
	pRomName=RomName;
	chtLoad();
}
#endif

#if 0
LRESULT CALLBACK AboutProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hWnd,IDC_EDIT1,WM_SETTEXT,0,(LPARAM)("警告：本计算机程序受著作权法和国际公约的保护，未经授权擅自复制或散布本程序的部分或全部，将承受严厉的民事和刑事处罚，对已知的违反者将给予法律范围内的全面制裁。"));
		break;
	case WM_CLOSE:
		EndDialog(hWnd,0);
		break;
	case WM_COMMAND:
		if (HIWORD(wParam)==BN_CLICKED && (LOWORD(wParam))==IDOK)
			EndDialog(hWnd,0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int t,mx,my;
	char s[300],s2[300];
	static int soft_key_tbl[]={
		0,0,0,0,0,0,112,113,114,115,
		'Q','W','E','R','T','Y','U','I','O','P',
		'A','S','D','F','G','H','J','K','L',13,
		'Z','X','C','V','B','N','M',33,38,34,
		116,16,117,27,'0',190,' ',37,40,39};

	switch (message)
	{
		case WM_CREATE:
			SendMessage(hwnd_ide,WM_LAVASTAR,(WPARAM)hWnd,0);
			break;
		case WM_DESTROY:
			SendMessage(hWnd,WM_COMMAND,ID_EXIT,0);
			break;
		case WM_LBUTTONDOWN:
			mx=LOWORD(lParam);
			my=HIWORD(lParam);
			if (ScreenDouble) {
				t=(320-ScreenWidth*2)/2;
				if (my>=16 && my<ScreenHeight*2+16 && mx>=t && mx<ScreenWidth*2+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_LBUTTONDOWN;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=((mx-t)/2)|(((my-16)/2)<<16);
						hardinput_wp++;
					}
					if (have_pen && wait_key==1) {
						pen_x=(mx-t)/2;
						pen_y=(my-16)/2;
					}
				}
				t=ScreenHeight*2+24;
			} else {
				t=(320-ScreenWidth)/2;
				if (my>=16 && my<ScreenHeight+16 && mx>=t && mx<ScreenWidth+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_LBUTTONDOWN;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=(mx-t)|((my-16)<<16);
						hardinput_wp++;
					}
					if (have_pen && wait_key==1) {
						pen_x=mx-t;
						pen_y=my-16;
					}
				}
				t=ScreenHeight+24;
			}
			if (my<t || (my-t)%32==0 || mx%32==0) break;
			t=((my-t)/32)*10+(mx/32);
			KeyID=soft_key_tbl[t];
			if (KeyID) {
				keybd_event(KeyID,0,0,0);				
				KeyPress=TRUE;
			}
			break;
		case WM_LBUTTONUP:
			mx=LOWORD(lParam);
			my=HIWORD(lParam);
			if (ScreenDouble) {
				t=(320-ScreenWidth*2)/2;
				if (my>=16 && my<ScreenHeight*2+16 && mx>=t && mx<ScreenWidth*2+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_LBUTTONUP;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=((mx-t)/2)|(((my-16)/2)<<16);
						hardinput_wp++;
					}
				}
			} else {
				t=(320-ScreenWidth)/2;
				if (my>=16 && my<ScreenHeight+16 && mx>=t && mx<ScreenWidth+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_LBUTTONUP;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=(mx-t)|((my-16)<<16);
						hardinput_wp++;
					}
				}
			}
			if(KeyPress=TRUE) {	
				keybd_event(KeyID,0,KEYEVENTF_KEYUP,0);
				KeyPress=FALSE;
			}
			break;
		case WM_MOUSEMOVE:
			mx=LOWORD(lParam);
			my=HIWORD(lParam);
			if (ScreenDouble) {
				t=(320-ScreenWidth*2)/2;
				if (my>=16 && my<ScreenHeight*2+16 && mx>=t && mx<ScreenWidth*2+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_MOUSEMOVE;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=((mx-t)/2)|(((my-16)/2)<<16);
						hardinput_wp++;
					}
				}
			} else {
				t=(320-ScreenWidth)/2;
				if (my>=16 && my<ScreenHeight+16 && mx>=t && mx<ScreenWidth+t) {
					if (((hardinput_wp+1)&0xff)!=hardinput_rp) {
						hardinput[hardinput_wp].type=XM_MOUSEMOVE;
						hardinput[hardinput_wp].wParam=0;
						hardinput[hardinput_wp].lParam=(mx-t)|((my-16)<<16);
						hardinput_wp++;
					}
				}
			}
			break;
		case WM_PAINT:
			hdc=BeginPaint(hWnd,&ps);
			WriteScreen(hdc,1);
			EndPaint(hWnd,&ps);
			break;
		case WM_INITMENUPOPUP:
			if (lParam==2) {
				if (EmuRunning) {
					EnableMenuItem((HMENU)wParam,ID_VERIFY,MF_ENABLED);
					EnableMenuItem((HMENU)wParam,ID_CHECK,MF_ENABLED);
					EnableMenuItem((HMENU)wParam,ID_FINDCHEAT,MF_ENABLED);
					EnableMenuItem((HMENU)wParam,ID_CHTEDIT,MF_ENABLED);
				} else {
					EnableMenuItem((HMENU)wParam,ID_VERIFY,MF_GRAYED);
					EnableMenuItem((HMENU)wParam,ID_CHECK,MF_GRAYED);
					EnableMenuItem((HMENU)wParam,ID_FINDCHEAT,MF_GRAYED);
					EnableMenuItem((HMENU)wParam,ID_CHTEDIT,MF_GRAYED);
				}
			} else if (lParam==0) {
				if (EmuRunning) {
					EnableMenuItem(hMenu,ID_CLOSE,MF_ENABLED);
					EnableMenuItem(hMenu,ID_RESET,MF_ENABLED);
				} else {
					EnableMenuItem(hMenu,ID_CLOSE,MF_GRAYED);
					EnableMenuItem(hMenu,ID_RESET,MF_GRAYED);
				}
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_EXIT:
					if (EmuRunning) CloseRom();
					PostQuitMessage(0);
					break;
				case ID_ABOUT:
					DialogBoxParam(hInstan,MAKEINTRESOURCE(IDD_ABOUT),hWnd,(DLGPROC)AboutProc,0);
					break;
				case ID_OPEN:
					FileOpen();
					break;
				case ID_CLOSE:
					if (EmuRunning) CloseRom();
					break;
				case ID_RESET:
					good_exit();
					EmuRunning=1;
					PauseFlag=0;
					RomReset();
					break;
				case ID_TVSCAN:
					if (GetMenuState(hMenu,ID_TVSCAN,MF_BYCOMMAND)&MF_CHECKED) {
						CheckMenuItem(hMenu,ID_TVSCAN,MF_UNCHECKED);
						TVSCAN=0;
					} else {
						CheckMenuItem(hMenu,ID_TVSCAN,MF_CHECKED);
						TVSCAN=1;
						memset(ScreenBufferX,255,320*336); //清大屏
					}
					break;
				case ID_GRAY:
					if (GetMenuState(hMenu,ID_GRAY,MF_BYCOMMAND)&MF_CHECKED) {
						CheckMenuItem(hMenu,ID_GRAY,MF_UNCHECKED);
						GRAY=0;
					} else {
						CheckMenuItem(hMenu,ID_GRAY,MF_CHECKED);
						GRAY=1;
					}
					SetPalette();
					break;
				case ID_ENDVERIFY:
					if (GetMenuState(hMenu,ID_ENDVERIFY,MF_BYCOMMAND)&MF_CHECKED) {
						CheckMenuItem(hMenu,ID_ENDVERIFY,MF_UNCHECKED);
						ENDVERIFY=0;
					} else {
						CheckMenuItem(hMenu,ID_ENDVERIFY,MF_CHECKED);
						ENDVERIFY=1;
					}
					break;
				case ID_RAM0:
					if (!(GetMenuState(hMenu,ID_RAM0,MF_BYCOMMAND)&MF_CHECKED)) {
						CheckMenuItem(hMenu,ID_RAM0,MF_CHECKED);
						CheckMenuItem(hMenu,ID_RAM1,MF_UNCHECKED);
						CheckMenuItem(hMenu,ID_RAM2,MF_UNCHECKED);
						CHECKRAM=0;
					}
					break;
				case ID_RAM1:
					if (!(GetMenuState(hMenu,ID_RAM1,MF_BYCOMMAND)&MF_CHECKED)) {
						CheckMenuItem(hMenu,ID_RAM1,MF_CHECKED);
						CheckMenuItem(hMenu,ID_RAM0,MF_UNCHECKED);
						CheckMenuItem(hMenu,ID_RAM2,MF_UNCHECKED);
						CHECKRAM=1;
					} 
					break;
				case ID_RAM2:
					if (!(GetMenuState(hMenu,ID_RAM2,MF_BYCOMMAND)&MF_CHECKED)) {
						CheckMenuItem(hMenu,ID_RAM2,MF_CHECKED);
						CheckMenuItem(hMenu,ID_RAM0,MF_UNCHECKED);
						CheckMenuItem(hMenu,ID_RAM1,MF_UNCHECKED);
						CHECKRAM=2;
					}
					break;
				case ID_VERIFY:
					verify();
					break;
				case ID_CHECK:
					check();
					break;
				case ID_BMP:
					GetBmp(RomName);
					break;
				case ID_REGPAC:
					GetSystemDirectory(s,MAX_PATH);
					strcat(s,"\\lavaxpac.dll");
					sprintf(s2,"%slavaxpac.dll",FcPath);
					CopyFile(s2,s,TRUE);
					sprintf(s,"regsvr32 /s /c lavaxpac.dll");
					WinExec(s,SW_HIDE);
					break;
				case ID_UNREGPAC:
					sprintf(s,"regsvr32 /s /u lavaxpac.dll");
					WinExec(s,SW_HIDE);
					break;
				case ID_FINDCHEAT:
					DialogBoxParam(hInstan,(LPCTSTR)IDD_FINDCHEAT,hWnd,ChtFind,(LPARAM)NULL);
					break;
				case ID_CHTEDIT:
					DialogBoxParam(hInstan,(LPCTSTR)IDD_CHTEDIT,hWnd,ChtEdit,(LPARAM)NULL);
					break;
				case ID_CHTUSE:
					if (GetMenuState(hMenu,ID_CHTUSE,MF_BYCOMMAND)&MF_CHECKED) {
						CheckMenuItem(hMenu,ID_CHTUSE,MF_UNCHECKED);
						CHEATUSE=0;
					} else {
						CheckMenuItem(hMenu,ID_CHTUSE,MF_CHECKED);
						CHEATUSE=1;
					}
					break;
			}
			break;
		case WM_STOPLINE:
			stop_line=(int)wParam;
			//PauseFlag=0;
			break;
		case WM_COPYDATA:
			{
				COPYDATASTRUCT *pcds;
				pcds=(COPYDATASTRUCT *)lParam;
				sline_num=pcds->dwData;
				memcpy(sline,pcds->lpData,pcds->cbData);
				PauseFlag=0;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
#endif

#ifdef USE_SDL

#include <windows.h>
void EmuRun()
{

	unsigned long t;
	int ReFresh,i;
	byte key_bak;
	static byte direct_key[]={23,20,22,21};
	static byte num_tbl[]="bnmghjtyu";

	if (!EmuRunning || PauseFlag) {
		platSleep(1);
		return;
	}
	/*
	if (IdleParam) {
		_asm rdtsc
		_asm mov t,eax
		if (t-times>=FREQ/10000*IdleParam) {
			Sleep(1);
			_asm rdtsc
			_asm mov times,eax
			return;
		}
	}*/
	t = platGetTickCount();
	if (t-timel<FREQ60)
		ReFresh=0;
	else {
		ReFresh=1;
		timel=t;
	}
	if (lDelay) {
		if (t-timed>=FREQ256) {
			timed=t;
			lDelay--;
		}
	} else lavRun();
	if (ReFresh) {
		TotalFrame++;
		CurFrame++;
		if (CurFrame>=60) {
			CurFrame=0;
			mesDrawTime();
		}
	
		WriteScreen(0);
		memcpy(old_keyb,cur_keyb,256);
		GetKeyboardState(cur_keyb);
		if (lav_key<128) {
			key_bak=lav_key;
			for (i=0;i<256;i++) {
				if (((old_keyb[i]^cur_keyb[i])&0x80) && (cur_keyb[i]&0x80)) {
					lav_key=i;
					if (lav_key>='A' && lav_key<='Z') lav_key|=0x20;
					else if (lav_key>=112 && lav_key<=115) lav_key=lav_key-112+0x1c; //F1-F4
					else if (lav_key>=37 && lav_key<=40) lav_key=direct_key[lav_key-37];
					else if (lav_key==33) lav_key=19; //PageUp
					else if (lav_key==34) lav_key=14; //PageDown					
					else if (lav_key==190) lav_key='.';
					else if (lav_key=='0' || lav_key==' ' || lav_key==13 || lav_key==27) ;
					else if (lav_key==16) lav_key=26; //Shift
					else if (lav_key>='1' && lav_key<='9') lav_key=num_tbl[lav_key-'1'];
					else if (lav_key==116) lav_key=25; //F5作为求助
					else if (lav_key==117) lav_key=18; //F6作为输入法
					else {
						lav_key=0;
						continue;
					}
					lav_key|=0x80;
					break;
				}
			}
			if (lav_key<128) lav_key=key_bak;
		}
	}
	if (!EmuRunning) CloseRom();
}
#endif
