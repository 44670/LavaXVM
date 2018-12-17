#include <define.h>

#include <M5Stack.h>

#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

extern "C" {
u32 platGetTickCount() { return xTaskGetTickCount(); }

void platSleep(u32 ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }

u32 platGetKeyboardState() { return 0; }

u32 platGetFileAttributes(char *path) { return 0; }

void *platFopen(char *path, char *mode) {
  printf("fopen: %s %s\n", path, mode);
  return fopen(path, mode);
}

int platFwrite(void *fp, u8 *buf, int size) {
  return fwrite(buf, 1, size, (FILE *)fp);
}

int platFread(void *fp, u8 *buf, int size) {
  return fread(buf, 1, size, (FILE *)fp);
}

int platFseek(void *fp, int offset, int mode) {
  return fseek((FILE *)fp, offset, mode);
}

int platFtell(void *fp) { return ftell((FILE *)fp); }

int platFdelete(char *path) {
  return unlink(path);
  // return !SD.remove(path);
}

u32 platFgetlastwritetime(void *fp) { return 0; }

int platFclose(void *fp) {

  fclose((FILE *)fp);
  return 0;
}

u32 platFgetattr(void *fp) { return 0; }

int platFmkdir(char *path) {
  printf("mkdir: %s\n", path);
  return mkdir(path, 0755);
}

static char *formatDirPath(char *path) {
  static char buf[MAX_PATH];
  strcpy(buf, path);
  int len = strlen(buf);
  if (len > 0) {
    if (buf[len - 1] == '/') {
      buf[len - 1] = 0;
    }
  }
  return buf;
}
int platGetDirFileCount(char *path) {
  DIR *dir;
  struct dirent *entry;
  int cnt = 0;

  path = formatDirPath(path);
  if ((dir = opendir(path)) == NULL) {
    return 0;
  }
  while ((entry = readdir(dir)) != NULL) {
    cnt++;
  }
  closedir(dir);
  return cnt;
}

int platGetDirFileName(char *buf, char *path, int idx) {
  buf[0] = 0;
  DIR *dir;
  struct dirent *entry;
  int cnt = 0;

  path = formatDirPath(path);
  if ((dir = opendir(path)) == NULL) {
    return 0;
  }
  while ((entry = readdir(dir)) != NULL) {
    if (cnt == idx) {
      strcpy(buf, entry->d_name);
      closedir(dir);
      return 0;
    }
    cnt++;
  }
  closedir(dir);
  return 0;
}
}