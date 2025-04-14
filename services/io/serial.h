#ifndef __VOIKER_KERNEL_SERIAL
#define __VOIKER_KERNEL_SERIAL


#include <lib/types/bool.h>
#include <lib/types/int.h>


#define VOIKER_SERIAL_RECIEVED 1
#define VOIKER_SERIAL_EMPTY 0x20


bool serial_port_init(uint16_t com_port);

char serial_port_read(uint16_t port);
void serial_port_write(uint16_t port, char character);


#endif
