#include "disk.h"

#define IDE_DATA_PORT      0x1F0
#define IDE_ERROR_PORT     0x1F1
#define IDE_SECTOR_COUNT   0x1F2
#define IDE_SECTOR_NUM     0x1F3
#define IDE_CYLINDER_LOW   0x1F4
#define IDE_CYLINDER_HIGH  0x1F5
#define IDE_DRIVE_HEAD     0x1F6
#define IDE_STATUS_PORT    0x1F7
#define IDE_COMMAND_PORT   0x1F7

#define IDE_CMD_READ_SECTORS  0x20
#define IDE_CMD_WRITE_SECTORS 0x30

#define IDE_STATUS_BUSY      0x80
#define IDE_STATUS_READY     0x40
#define IDE_STATUS_DRQ       0x08
#define IDE_STATUS_ERROR     0x01

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int disk_wait_ready() {
    int timeout = 100000;
    while (timeout--) {
        uint8_t status = inb(IDE_STATUS_PORT);
        if (!(status & IDE_STATUS_BUSY)) {
            return 0;
        }
    }
    return -1;
}

static int disk_wait_drq() {
    int timeout = 100000;
    while (timeout--) {
        uint8_t status = inb(IDE_STATUS_PORT);
        if (status & IDE_STATUS_DRQ) {
            return 0;
        }
        if (status & IDE_STATUS_ERROR) {
            return -1;
        }
    }
    return -1;
}

void disk_init() {
    outb(0x3F6, 0x04);
    for (volatile int i = 0; i < 10000; i++);
    outb(0x3F6, 0x00);
    for (volatile int i = 0; i < 10000; i++);
}

int disk_read_sector(uint32_t sector, uint8_t* buffer) {
    if (!buffer) return -1;
    
    if (disk_wait_ready() != 0) return -1;

    uint32_t c = sector / 63 / 16;
    uint32_t h = (sector / 63) % 16;
    uint32_t s = (sector % 63) + 1;

    outb(IDE_DRIVE_HEAD, 0xE0 | (h & 0x0F));
    outb(IDE_SECTOR_COUNT, 1);
    outb(IDE_SECTOR_NUM, s);
    outb(IDE_CYLINDER_LOW, c & 0xFF);
    outb(IDE_CYLINDER_HIGH, (c >> 8) & 0xFF);
    outb(IDE_COMMAND_PORT, IDE_CMD_READ_SECTORS);

    if (disk_wait_drq() != 0) return -1;

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(IDE_DATA_PORT);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }

    return 0;
}

int disk_write_sector(uint32_t sector, const uint8_t* buffer) {
    if (!buffer) return -1;

    if (disk_wait_ready() != 0) return -1;

    uint32_t c = sector / 63 / 16;
    uint32_t h = (sector / 63) % 16;
    uint32_t s = (sector % 63) + 1;

    outb(IDE_DRIVE_HEAD, 0xE0 | (h & 0x0F));
    outb(IDE_SECTOR_COUNT, 1);
    outb(IDE_SECTOR_NUM, s);
    outb(IDE_CYLINDER_LOW, c & 0xFF);
    outb(IDE_CYLINDER_HIGH, (c >> 8) & 0xFF);
    outb(IDE_COMMAND_PORT, IDE_CMD_WRITE_SECTORS);

    if (disk_wait_drq() != 0) return -1;

    for (int i = 0; i < 256; i++) {
        uint16_t data = ((uint16_t)buffer[i * 2 + 1] << 8) | buffer[i * 2];
        outw(IDE_DATA_PORT, data);
    }

    if (disk_wait_ready() != 0) return -1;

    return 0;
}

int disk_is_ready() {
    return disk_wait_ready() == 0;
}
