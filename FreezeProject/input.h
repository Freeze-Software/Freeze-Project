#ifndef INPUT_H
#define INPUT_H

unsigned char inb(unsigned short port);
static inline void outb(unsigned short port, unsigned char val);
void get_input(char* buffer);
char scancode_to_ascii(unsigned char sc);
#endif
