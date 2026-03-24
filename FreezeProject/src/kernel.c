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
#include <stdint.h>

void print_hex(uint32_t val);

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
                                 ((uint32_t)0x80000000));

    outl(0xCF8, address);
    return inl(0xCFC);
}

/* Store e1000 location */
uint8_t e1000_bus = 0xFF;
uint8_t e1000_slot = 0xFF;

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

            if (vendor == 0x8086 && device == 0x100E) {
                print("[NET] Found e1000!\n");
                e1000_bus = bus;
                e1000_slot = slot;
            }
        }
    }

    if (e1000_bus == 0xFF) {
        print("\n[NET] No e1000 found.\n");
    } else {
        print("\n[NET] e1000 ready at ");
        print_hex(e1000_bus);
        print(":");
        print_hex(e1000_slot);
        print("\n");
    }
}
// Entry
void kernel_main(void){
    clear();

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

    pci_scan(); 

    shell();
}
