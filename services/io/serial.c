#include <kernel/io/serial.h>
#include <kernel/io/port.h>
#include <lib/types/defs.h>
#include <lib/types/bool.h>
#include <lib/types/int.h>
#include <lib/string.h>


bool
serial_port_init(
    uint16_t com_port
) {
    outb(com_port + 1,  0x00); // disable interupts
    outb(com_port + 3,  0x80); // enable dlab
    outb(com_port,      0x03); // settings divisor to 3 (lo)
    outb(com_port + 1,  0x00); //                       (hi)
    outb(com_port + 3,  0x03); // 8 bits 
    outb(com_port + 2,  0xC7); // enable fifo
    outb(com_port + 4,  0x0B); // irqs enabled, rts/dsr set
    outb(com_port + 4,  0x1E); // set in loopback mode
    outb(com_port,      0xAE); // test serial chip

    if (inb(com_port) != 0xAE) {
        return false;
    }

    outb(com_port + 4, 0x0F);
    return true;
}


char
serial_port_read(
    uint16_t port
) {
    while ((inb(port + 5) & VOIKER_SERIAL_RECIEVED) == false);

    return inb(port);
}

void
serial_port_write(
    uint16_t port,
    char character
) {
    while ((inb(port + 5) & VOIKER_SERIAL_EMPTY) == false);

    outb(port, character);
}

