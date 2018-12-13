#include <define.h>

#include <M5Stack.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern "C" {
u32 platGetTickCount() {
	return xTaskGetTickCount();
}

void platSleep(u32 ms) {
	vTaskDelay(ms / portTICK_PERIOD_MS);
}

u32 platGetKeyboardState() {
	return 0;
}

u32 platGetFileAttributes(char* path) {
	return 0;
}

void* platFopen(char* path, char* mode) {
	printf("fopen: %s %s\n", path, mode);
    return new File(SD.open(path,  mode));
}

int platFwrite(void* fp, u8* buf, int size) {
    return ((File*)fp) -> write(buf, size);
	//return SD.write(buf, 1, size, fp);
}

int platFread(void* fp, u8* buf, int size) {
    return ((File*)fp) -> read(buf, size);
	//return fread(buf, 1, size, fp);
}

int platFseek(void* fp, int offset, int mode) {
    return ((File*)fp)->seek(offset, (fs::SeekMode) mode);
	//return fseek(fp, offset, mode);
}

int platFtell(void* fp) {
    return ((File*)fp)->position();
}



int platFdelete(char* path) {
	return !SD.remove(path);
}

u32 platFgetlastwritetime(void* fp) {
	return 0;
}

int platFclose(void* fp) {
	((File*)fp)->close();
	return 0;
}

u32 platFgetattr(void* fp) {
	return 0;
}

int platFmkdir(char* path) {
    return !SD.mkdir(path);
}

}