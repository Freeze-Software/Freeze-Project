#ifndef DISK_H
#define DISK_H

#include <stdint.h>

#define DISK_SECTOR_SIZE 512
#define FS_HEADER_SECTOR 0
#define FS_METADATA_START 1
#define FS_DATA_START 33

void disk_init();

int disk_read_sector(uint32_t sector, uint8_t* buffer);

int disk_write_sector(uint32_t sector, const uint8_t* buffer);

int disk_is_ready();

#endif
