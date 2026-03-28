__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002,
    0x00000003,
    -(0x1BADB002 + 0x00000003)
};

#include "memory.h"
#include "timer.h"
#include "input.h"
#include "vga.h"
#include "rtc.h"
#include "shell.h"
#include "fs.h"
#include <stdint.h>

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
                                 (func << 8) | (offset & 0xFC) |
                                 0x80000000);
    outl(0xCF8, address);
    return inl(0xCFC);
}

void pci_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
                                 (func << 8) | (offset & 0xFC) |
                                 0x80000000);
    outl(0xCF8, address);
    outl(0xCFC, value);
}

struct net_device {
    const char* name;
    void (*init)();
    int (*link_up)();
    void (*poll)();
};

struct net_device* active_net = 0;

void net_poll() {
    if (active_net && active_net->poll) {
        active_net->poll();
    }
}

uint8_t e1000_bus = 0xFF;
uint8_t e1000_slot = 0xFF;
volatile uint32_t* e1000_mmio = 0;

#define E1000_REG_CTRL   0x0000
#define E1000_REG_STATUS 0x0008
#define E1000_REG_RCTL   0x0100
#define E1000_REG_RDBAL  0x2800
#define E1000_REG_RDBAH  0x2804
#define E1000_REG_RDLEN  0x2808
#define E1000_REG_RDH    0x2810
#define E1000_REG_RDT    0x2818

#define E1000_NUM_RX_DESC 32
#define E1000_RX_BUFFER_SIZE 2048

struct e1000_rx_desc {
    uint64_t addr;
    uint16_t length;
    uint16_t checksum;
    uint8_t status;
    uint8_t errors;
    uint16_t special;
};

struct e1000_rx_desc rx_descs[E1000_NUM_RX_DESC];
uint8_t rx_buffers[E1000_NUM_RX_DESC][E1000_RX_BUFFER_SIZE];

int rx_index = 0;

void e1000_write(uint32_t reg, uint32_t val) {
    e1000_mmio[reg / 4] = val;
}

int e1000_link_up() {
    uint32_t status = e1000_mmio[E1000_REG_STATUS / 4];
    return (status & (1 << 1)) != 0;
}

void e1000_rx_init() {
    print("[NET] Setting up RX...\n");

    for (int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rx_descs[i].addr = (uint64_t)&rx_buffers[i];
        rx_descs[i].status = 0;
    }

    e1000_write(E1000_REG_RDBAL, (uint32_t)&rx_descs);
    e1000_write(E1000_REG_RDBAH, 0);
    e1000_write(E1000_REG_RDLEN, E1000_NUM_RX_DESC * sizeof(struct e1000_rx_desc));

    e1000_write(E1000_REG_RDH, 0);
    e1000_write(E1000_REG_RDT, E1000_NUM_RX_DESC - 1);

    uint32_t rctl = (1 << 1) | (1 << 15);
    e1000_write(E1000_REG_RCTL, rctl);

    print("[NET] RX initialized\n");
}

void e1000_poll() {
    struct e1000_rx_desc* desc = &rx_descs[rx_index];

    if (desc->status & 0x1) {
        print("[NET] Packet received! Len=");
        print_hex(desc->length);
        print("\n");

        desc->status = 0;
        e1000_write(E1000_REG_RDT, rx_index);

        rx_index = (rx_index + 1) % E1000_NUM_RX_DESC;
    }
}

void e1000_init() {
    print("\n[NET] Initializing e1000...\n");

    uint32_t cmd = pci_read(e1000_bus, e1000_slot, 0, 0x04);
    cmd |= (1 << 2) | (1 << 1);
    pci_write(e1000_bus, e1000_slot, 0, 0x04, cmd);

    uint32_t bar0 = pci_read(e1000_bus, e1000_slot, 0, 0x10);
    e1000_mmio = (volatile uint32_t*)(bar0 & 0xFFFFFFF0);

    e1000_write(E1000_REG_CTRL, (1 << 26));

    for (volatile int i = 0; i < 100000; i++);

    print("[NET] Reset done\n");

    e1000_rx_init();

    if (e1000_link_up()) {
        print("[NET] Cable connected\n");
    } else {
        print("[NET] No cable\n");
    }
}

struct net_device e1000_dev = {
    .name = "e1000",
    .init = e1000_init,
    .link_up = e1000_link_up,
    .poll = e1000_poll
};

void pci_scan() {
    print("\n[PCI] Scanning...\n");

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {

            uint32_t data = pci_read(bus, slot, 0, 0);
            uint16_t vendor = data & 0xFFFF;

            if (vendor == 0xFFFF || vendor == 0x0000)
                continue;

            uint16_t device = (data >> 16) & 0xFFFF;

            print("[PCI] ");
            print_hex(vendor);
            print(":");
            print_hex(device);
            print(" @ ");
            print_hex(bus);
            print(":");
            print_hex(slot);
            print("\n");

            uint32_t classData = pci_read(bus, slot, 0, 0x08);
            uint8_t classCode = (classData >> 24) & 0xFF;
            uint8_t subclass  = (classData >> 16) & 0xFF;

            if (classCode == 0x02) {
                print("[NET] network device detected at ");
                print_hex(bus);
                print(":");
                print_hex(slot);
                print("\n");
            }

            if (vendor == 0x8086 && device == 0x100E) {
                print("[NET] Found e1000!\n");
                e1000_bus = bus;
                e1000_slot = slot;
                active_net = &e1000_dev;
            }
        }
    }

    if (e1000_bus != 0xFF) {
        print("\n[NET] e1000 ready\n");
        e1000_init();
    }

    if (active_net) {
        print("[NET] Active driver: ");
        print(active_net->name);
        print("\n");
    }
}

void kernel_main(void){
    clear();

    pci_scan();

    fs_init();

    print("___________                                    \n");
    print("\\_   _____/______   ____   ____ ________ ____  \n");
    print(" |    __) \\_  __ \\_/ __ \\_/ __ \\\\___   // __ \\ \n");
    print(" |     \\   |  | \\/\\  ___/\\  ___/ /    /\\  ___/ \n");
    print(" \\___  /   |__|    \\___  >\\___  >_____ \\\\___  >\n");
    print("     \\/                \\/     \\/      \\/    \\/ \n");
    print("\033[92mhttps://freezeos.org/\033[0m\n");
    print("\033[93mType 'help' for help on learning commands\033[0m\n\n");
    print("\033[94m--------------------------------\033[0m\n");
    print("\033[92mCurrently: \033[93mVersion 0.64\033[0m\n");

    shell();
}
