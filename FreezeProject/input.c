#include "input.h"

char scancode_to_ascii(unsigned char sc){
    char map[128] = {
        0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,
        9,'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
        'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
        'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
    };
    if(sc < sizeof(map)) return map[sc];
    return 0;
}
// SERIAL input-aware reader
void get_input(char* buffer){
    int i = 0;
    while(1){
        if(serial_available()){
            char c = serial_getc();
            if(c == '\r') c = '\n';
            if(c == '\n'){ buffer[i]=0; putc('\n'); return; }
            if(c==8 || c==127){ if(i>0){ i--; erase_last_char(); } continue; }
            if(i < 127){ buffer[i++] = c; putc(c); }
            continue;
        }

        unsigned char sc = inb(0x60);
        if(sc & 0x80) continue;
        char c = scancode_to_ascii(sc);
        if(!c) continue;
        if(c=='\n'){ buffer[i]=0; putc('\n'); return; }
        if(c==8 && i>0){ i--; erase_last_char(); continue; }
        if(i < 127){ buffer[i++] = c; putc(c); }
    }
}
unsigned char inb(unsigned short port){
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outb(unsigned short port, unsigned char val){
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
