#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_FILES 32
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 1024

struct file {
    char name[MAX_FILENAME];
    char content[MAX_FILE_SIZE];
    uint32_t size;
    int used;
};

extern struct file files[MAX_FILES];

void fs_init();
int fs_create(const char* name);
int fs_write(int fd, const char* data, uint32_t size);
int fs_read(int fd, char* buffer, uint32_t size);
int fs_find(const char* name);
void fs_list();

#endif