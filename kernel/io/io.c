#include <kernel/io/io.h>
#include <kernel/io/port.h>

void io_wait(void) {
    // Port 0x80 is used for POST codes during BIOS initialization
    // Writing to it causes a small delay (~1 microsecond)
    outb(0x80, 0);
}
