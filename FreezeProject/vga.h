#ifndef VGA_H
#define VGA_H
#include<stdint.h>
extern volatile uint16_t* vga = (uint16_t*)0xB8000;
extern int row = 0, col = 0;
extern uint8_t color = 0x02;

void putc(char c);
void print(const char* s);
void clear();
void erase_last_char();
void print_int(int v);
void print_hex(unsigned int v);
#endif