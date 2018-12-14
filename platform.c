
#include "define.h"
#include <windows.h>
#include <stdio.h>
u32 platGetTickCount() {
	return GetTickCount();
}

void platSleep(u32 ms) {
	Sleep(ms);
}

u32 platGetKeyboardState() {
	return 0;
}

u32 platGetFileAttributes(char* path) {
	return 0;
}

void* platFopen(char* path, char* mode) {
	printf("platFopen: %s\n", path);
	return fopen(path, mode);
}

int platFwrite(void* fp, u8* buf, int size) {
	return fwrite(buf, 1, size, fp);
}

int platFread(void* fp, u8* buf, int size) {
	return fread(buf, 1, size, fp);
}

int platFseek(void* fp, int offset, int mode) {
	return fseek(fp, offset, mode);
}

int platFtell(void* fp) {
	return ftell(fp);
}

int platFgetsize(void* fp) {
	int pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return size;
}

int platFdelete(char* path) {
	return unlink(path);
}

u32 platFgetlastwritetime(void* fp) {
	return 0;
}

int platFclose(void* fp) {
	fclose(fp);
	return 0;
}

u32 platFgetattr(void* fp) {
	return 0;
}

int platFmkdir(char* path) {
	return !CreateDirectory(path, NULL);
}



int platGetDirFileCount(char* path) {
	
	return 30;
}

int platGetDirFileName(char* buf, char* path, int idx) {
	buf[0] = 0;
	sprintf(buf, "testfile%d.lav", idx);
}