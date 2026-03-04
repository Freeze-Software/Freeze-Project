#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

extern uint32_t total_memory;
extern uint32_t used_memory;

void* kmalloc(uint32_t size);

#endif
