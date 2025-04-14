#include <kernel/hardware/vga.h>
#include <lib/types/defs.h>
#include <lib/types/bool.h>
#include <lib/types/int.h>
#include <lib/io.h>

extern void outb(uint16_t port, uint8_t value);

struct {
    uint8_t *buffer;
    bool colors_available;
} vga = { 0 };

void
vga_init(
    bool monochrome
) {
    vga.buffer = monochrome
        ? (uint8_t *)0xB0000
        : (uint8_t *)0xB8000;

    vga.colors_available = !monochrome;
}


uint8_t
vga_color_pack_pair(
    vga_color_t foreground,
    vga_color_t background
) {
    return (uint8_t)foreground | (uint8_t)background << 4;
}

void
vga_put_character(
    uint8_t x, uint8_t y,
    char character,
    uint8_t color_pair
) {
    uint16_t index = 0;
    if (vga.colors_available) {
        index = (y * VGA_VIDEO_WIDTH * 2) + (x * 2);
    } else {
        index = y * VGA_VIDEO_WIDTH + x;
    }

    vga.buffer[index] = character;
    if (vga.colors_available) {
        vga.buffer[index + 1] = color_pair;
    }
}


void
vga_color_unpack_pair(
    uint8_t color,
    vga_color_t *foreground,
    vga_color_t *background
) {
    if (NULL != foreground) {
        *foreground = color & 0x0F;
    }

    if (NULL != background) {
        *background = color & 0xF0;
    }
}

char
vga_get_character(
    uint8_t x, uint8_t y,
    uint8_t *color
) {
    uint16_t index = 0;
    if (vga.colors_available) {
        index = (y * VGA_VIDEO_WIDTH * 2) + (x * 2);
    } else {
        index = y * VGA_VIDEO_WIDTH + x;
    }

    char character = vga.buffer[index];
    if (vga.colors_available && NULL != color) {
        *color = vga.buffer[index + 1];
    }

    return character;
}

void vga_set_cursor_position(uint8_t x, uint8_t y) {
    uint16_t pos = y * VGA_VIDEO_WIDTH + x;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}
