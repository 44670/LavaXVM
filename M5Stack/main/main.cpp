#include "ESP8266FtpServer.h"
#include <M5Display.h>
#include <M5Stack.h>
#include <WiFi.h>

#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include <define.h>

#define TAG ("MAIN")

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE
#endif
#define KEYBOARD_I2C_ADDR 0X88
#define KEYBOARD_INT 5

// The setup routine runs once when M5Stack starts up
void setup() {

  // Initialize the M5Stack object
  M5.begin();
  Wire.begin();
  M5.Lcd.setTextSize(3);
  // LCD display
  M5.Lcd.print("Hello World\n");
}

extern "C" {
/*
// The loop routine runs over and over again forever
void loop() {

  M5.update();
}

// The arduino task
void loopTask(void *pvParameters)
{
    setup();
    for(;;) {
        micros(); //update overflow
        loop();
    }
}*/

int LoadLavOrPacFile(char *fname);
void EmuRun();

void Sleep(uint32_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }

void panic(char *msg) {
  M5.Lcd.print("ERROR!!! \n");
  M5.Lcd.print(msg);

  while (1) {
    Sleep(1000);
  }
}

char *FcPath = "/sd/";
char *BmpPath = "/sd/bmp";
char *CheatPath = "/sd/cheat";
char *RomPath = "/sd/";

char RomName[MAX_PATH];
char *pRomName;
unsigned long FREQ;
unsigned long FREQ60, FREQ256;
int TVSCAN, GRAY, ENDVERIFY, CHEATUSE;
int EmuRunning, PauseFlag, RamError;
int Frame;
int CurFrame, TotalFrame;
int stop_line = -1, stop_func, sline[256], sline_num;
unsigned long timel, times, IdleParam;
byte lav_key;
byte cur_keyb[256];
struct MESSAGE hardinput[256];   // LavaX虚拟机的硬件输入消息队列
byte hardinput_rp, hardinput_wp; //硬件输入消息队列的读写指针

void WriteScreen(int mode) {
  // printf("write start\n");
  M5Display *lcd = &(M5.lcd);
  u16 lineBuf[320 * 3];
  u16 *ptr;
  u16 pix;
  u8 *srcPtr = ScreenBuffer;
  lcd->startWrite();
  lcd->setAddrWindow(0, 0, 320 - 1, 240 - 1);
  int x, y;
  for (y = 0; y < 80; y++) {
    ptr = lineBuf;
    for (x = 0; x < 160; x++) {
      pix = (*srcPtr) ? 0x0000 : 0xffff;
      srcPtr++;
      *ptr = pix;
      ptr++;
      *ptr = pix;
      ptr++;
    }
    memcpy(lineBuf + 320, lineBuf, 320 * 2);
    memcpy(lineBuf + 640, lineBuf, 320 * 2);
    lcd->writePixels(lineBuf, 320 * 3);
  }
  lcd->endWrite();
  // printf("write end\n");
}

static void CloseRom() {
  if (EmuRunning)
    good_exit();
  EmuRunning = 0; //释放ROM所占内存
  PauseFlag = 1;
}

void RomReset() {
  RamError = 0;
  CurFrame = 0;
  TotalFrame = 0;
  // memset(BmpData,0,320*ScreenHeight); //清屏
  // memset(ScreenBuffer,0,320*8);
  lavReset();
}

const u8 *mappedResourceData;
static spi_flash_mmap_handle_t resourceDataMapHandle;

u8 *getLVMBinPointer() { return (u8 *)mappedResourceData; }
// int x = 81*94*32+0x3e+6400;

int lavaMainLoop() {
  pLAVA = (byte *)malloc(PLAVA_SIZE);
  if (!pLAVA) {
    panic("alloc LAVA ROM buffer failed.");
  }
  printf("pLAVA: %p\n", pLAVA);
  stop_func = 0;
  func_top = 0;
  task[0].pLAVA = pLAVA;
  byte *hg = getLVMBinPointer();
  ascii = hg;
  ascii8 = ascii + 1536;
  gbfont = ascii8 + 2048;
  gbfont16 = gbfont + 81 * 94 * 24;
  pinyin = gbfont16 + (81 * 94 * 32) + (0x3e) + 6400;
  // memcpy(ScreenKey,gbfont16+81*94*32+(0x3e),6400);
  IdleParam = 9;
  FREQ = PLAT_TICK_PERIOD_MS * 1000;
  FREQ60 = FREQ / 10;
  FREQ256 = FREQ / 256;
  TVSCAN = 0;

  CHEATUSE = 0;
  EmuRunning = 0;
  InitScreen();
  LoadLavOrPacFile("/sd/boot.lav");
  if (EmuRunning == 0) {
    panic("load boot.lav failed.");
  }
  while (1) {
    EmuRun();
    if (EmuRunning == 0) {
      LoadLavOrPacFile("/sd/boot.lav");
      if (EmuRunning == 0) {
        panic("reload boot.lav failed.");
      }
    }
  }
}

int LavOpen(void *fp) {
  u32 filelen;

  platFseek(fp, 0, PLAT_SEEK_END);
  filelen = platFtell(fp);
  printf("lava file len: %d\n", filelen);
  platFseek(fp, 0, PLAT_SEEK_SET);
  int bytes = platFread(fp, pLAVA, filelen);
  printf("read: %d\n", bytes);
  *(pLAVA + filelen) = 0;
  platFclose(fp);
  return 1;
}


int UnPac(char *pacname, char *zname) {
  void *fp;
  char fname[MAX_PATH];
  long SizeRead;
  byte t[32];
  int16_t total;
  char head[64];
  long filelen;
  char Path[64];
  int i, j;
  int have_lav;
  u8 workBuffer[10240];  // we have allocated 20KB for main task's stack
  char* p;

  have_lav = 0;
  if ((fp = platFopen(zname, "rb")) == NULL) {
    FATAL("无法打开PAC文件!");
    return 0;
  }
  platFseek(fp, 0, PLAT_SEEK_SET);
  SizeRead = platFread(fp, (u8 *)t, 16);
  if (SizeRead != 16 || t[0] != 'P' || t[1] != 'A' || t[2] != 'C' ||
      t[3] != ' ') {
    platFclose(fp);
    FATAL("不是有效的PAC文件!");
    return 0;
  }
  platFread(fp, (u8 *)&total, 2);
  printf("total files: %d\n", total);
  for (i = 0; i < total; i++) {
    platFread(fp, (u8 *)head, 64);
    filelen = *(long *)head;
    if (filelen) {
      strcpy(Path, head + 4);
      p = NULL;
      for (j = 0; j < 60; j++) {
        if (Path[j] == '/') {
          // Path[j]='\\';
          p = Path + j;
        }
      }
      strcpy(fname, FcPath);
      strcat(fname, Path);
      if (p) {
        for (j = 0; j < 60; j++) {
          if (!fname[j])
            break;
          if (fname[j] == '/') {
            fname[j] = 0;
            M5.Lcd.printf("mkdir: %s\n", fname);
            platFmkdir(fname);
            fname[j] = '/';
          }
        }
      }
      M5.Lcd.printf("unpack: %s\n", fname);
      void* outFp = platFopen(fname, "wb");
      if (!outFp) {
        platFclose(outFp);
        panic("fopen failed.");
      }
      while (filelen > 0) {
        int wSize = filelen;
        if (wSize > sizeof(workBuffer)) {
          wSize = sizeof(workBuffer);
        }
        printf("%d, %d\n", (int) wSize, (int) filelen);
        platFread(fp, workBuffer, wSize);
        platFwrite(outFp, workBuffer, wSize);
        filelen -= wSize;
      }
      platFclose(outFp);
    }
  }  
  platFclose(fp);
  // platFdelete(zname);
  zname[0] = 0;
  return 1;
}

int LoadLavOrPacFile(char *fname) {
  void *fp;
  long SizeRead;
  byte t[32];
  char zname[MAX_PATH];
  strcpy(zname, fname);
abc:
  if ((fp = platFopen(zname, "rb")) == NULL) {
    FATAL("无法打开LavaX文件!");
    FATAL_printf("%s\n", zname);
    return 1;
  }
  platFseek(fp, 0, PLAT_SEEK_SET);
  SizeRead = platFread(fp, (u8 *)t, 16);
  if (t[0] == 'L' && t[1] == 'A' && t[2] == 'V' && t[3] == 18 &&
      SizeRead == 16) {
    if (EmuRunning)
      CloseRom();
    // pLAVA=pAllLAVA;
    if (!LavOpen(fp))
      return 2;
  } else if (t[0] == 'P' && t[1] == 'A' && t[2] == 'C' && t[3] == ' ' &&
             SizeRead == 16) {
    platFclose(fp);
    if (!UnPac(RomName, zname))
      return 1;
    if (zname[0] == 0)
      return 1;
    goto abc;
  } else {
    FATAL("不是有效的LavaX文件!");
    platFclose(fp);
    return 1;
  }
  task_lev = 0;
  EmuRunning = 1;
  PauseFlag = 0;

  pRomName = RomName; //+ofn.nFileOffset;
  /*
  if (ofn.nFileExtension)
          *(RomName+ofn.nFileExtension-1)=0;
          */
  RomReset();
  return 0;
  // chtLoad();
}

#include "../keymap_keyboard.h"

u8 translateKey(u8 keyval) { return keyboardKeyMap[keyval]; }

void updateKey() {
  Wire.requestFrom(KEYBOARD_I2C_ADDR, 1); // request 1 byte from keyboard
  if (Wire.available()) {
    uint8_t key_val = Wire.read();
    if (key_val != 0) {
      printf("key_val: %d\n", key_val);
      u8 translatedKey = translateKey(key_val);
      if (translatedKey != 0) {
        lav_key = 0x80 | translatedKey;
      }
    }
  }
}

void EmuRun() {

  unsigned long t;
  int ReFresh, i;
  static byte direct_key[] = {23, 20, 22, 21};
  static byte num_tbl[] = "bnmghjtyu";

  if (!EmuRunning || PauseFlag) {
    Sleep(1);
    return;
  }

  t = platGetTickCount();
  if (lDelay) {
    if (t - timed >= FREQ256) {
      timed = t;
      lDelay--;
    }
  } else
    lavRun();

  t = platGetTickCount();
  if (t - timel < FREQ60)
    ReFresh = 0;
  else {
    ReFresh = 1;
    timel = t;
  }

  if (ReFresh) {
    TotalFrame++;
    CurFrame++;
    if (CurFrame >= 60) {
      CurFrame = 0;
      mesDrawTime();
    }
    WriteScreen(0);
    Sleep(1);
    updateKey();
  }

  if (!EmuRunning)
    CloseRom();
}

/*
void ftpServerMainLoop() {
  FtpServer ftpSrv;
  WiFi.softAP("lava", "hellolava");
  M5.Lcd.println("IP: ");
  M5.Lcd.println(WiFi.softAPIP());
  ftpSrv.begin("lava", "hellolava");
  while (1) {
    ftpSrv.handleFTP();
    Sleep(1);
  }
}*/


void app_main() {

  initArduino();
  setup();
  esp_err_t ret = spi_flash_mmap(0x00200000, 0x00100000, SPI_FLASH_MMAP_DATA,
                                 (const void **)&mappedResourceData,
                                 &resourceDataMapHandle);
  if (ESP_OK != ret) {
    panic("spi_flash_mmap failed");
  }
  printf("mappedResourceData: %p\n", mappedResourceData);
  if (M5.BtnA.read()) {
    //ftpServerMainLoop();
  }
  lavaMainLoop();

  // xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL,
  // ARDUINO_RUNNING_CORE);
}


}
