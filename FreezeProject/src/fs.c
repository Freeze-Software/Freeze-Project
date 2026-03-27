#include "fs.h"
#include "memory.h" 
#include "vga.h"

char* strcpy(char* dest, const char* src);
void* memcpy(void* dest, const void* src, size_t n);
size_t strlen(const char* s);

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while (*src) {
        *d++ = *src++;
    }
    *d = 0;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

struct file files[MAX_FILES];

void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
        files[i].size = 0;
    }
}

int fs_create(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            strcpy(files[i].name, name);
            files[i].used = 1;
            files[i].size = 0;
            return i;
        }
    }
    return -1; // no space
}

int fs_write(int fd, const char* data, uint32_t size) {
    if (fd < 0 || fd >= MAX_FILES || !files[fd].used) return -1;
    if (size > MAX_FILE_SIZE) size = MAX_FILE_SIZE;
    memcpy(files[fd].content, data, size);
    files[fd].size = size;
    return size;
}

int fs_read(int fd, char* buffer, uint32_t size) {
    if (fd < 0 || fd >= MAX_FILES || !files[fd].used) return -1;
    if (size > files[fd].size) size = files[fd].size;
    memcpy(buffer, files[fd].content, size);
    return size;
}

int fs_find(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            const char* a = files[i].name;
            const char* b = name;
            while (*a && *b) {
                if (*a != *b) break;
                a++; b++;
            }
            if (*a == *b) return i;
        }
    }
    return -1;
}

void fs_list() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print(files[i].name);
            print("\n");
        }
    }
}