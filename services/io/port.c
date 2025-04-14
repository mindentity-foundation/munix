#include <kernel/io/port.h>
#include <lib/types/int.h>


inline void
outb(
    uint16_t port,
    uint8_t value
) {
    __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

inline uint8_t
inb(
    uint16_t port
) {
    uint8_t result;
    __asm__ volatile("inb %w1, %b0" : "=a"(result) : "Nd"(port) : "memory");
    return result;
}
