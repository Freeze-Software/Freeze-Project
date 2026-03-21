#include "vga.h"
void putc(char c){
    if(c=='\n'){
        row++;
        col = 0;
        /* mirror new line */
        serial_putc('\r');
        serial_putc('\n');
        return;
    }

    if(row >= 25) row = 0;

    vga[row*80 + col] = (color << 8) | c;

    col++;
    if(col >= 80){ col = 0; row++; }

    /* mirror to serial so -nographic shows output */
    serial_putc(c);
}

void print(const char* s){ for(int i=0; s[i]; i++) putc(s[i]); }

void clear(){
    for(int i=0;i<80*25;i++) vga[i] = (color<<8) | ' ';
    row = 0; col = 0;
    /* clear serial terminal */
    serial_print("\x1b[2J\x1b[H");
}
void erase_last_char(){
    if(col > 0){
        col--;
        vga[row*80 + col] = (0x07 << 8) | ' ';
        serial_putc('\b'); serial_putc(' '); serial_putc('\b');
    } else if(row > 0){
        row--; col = 79;
        vga[row*80 + col] = (0x07 << 8) | ' ';
        serial_putc('\b'); serial_putc(' '); serial_putc('\b');
    }
}
// print integer
void print_int(int v){
    char buf[16];
    int i = 0;

    if(v == 0){
        putc('0');
        return;
    }

    while(v > 0){
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }

    for(int j = i - 1; j >= 0; j--){
        putc(buf[j]);
    }
}

/* hex value */
void print_hex(unsigned int v){
    const char* hex = "0123456789ABCDEF";
    char buf[9];
    for(int i=0;i<8;i++) buf[7-i] = hex[v & 0xF], v >>= 4;
    buf[8]=0;
    print("0x");
    print(buf);
}