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

// ENTRY 
void kernel_main(void){
    clear();
    print("\033[96m=== \033[95mFreeze Project\033[96m ===\033[0m\n");
    print("\033[92mhttps://freezeos.org/\033[0m\n");
    print("\033[93mType 'help' for help on learning commands\033[0m\n\n");
    print("\033[94m--------------------------------\033[0m\n");
    print("\033[92mCurrently: \033[93mVersion 0.62\033[0m\n");

    shell();
}
