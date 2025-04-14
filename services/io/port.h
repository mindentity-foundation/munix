#ifndef __VOIKER_KERNEL_PORT
#define __VOIKER_KERNEL_PORT


#include <lib/types/int.h>


void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);


#endif
