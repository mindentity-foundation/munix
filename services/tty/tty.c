#include <kernel/hardware/hardware.h>
#include <kernel/hardware/vga.h>
#include <kernel/tty.h>
#include <lib/types/int.h>

typedef struct {
    vga_color_t foreground;
    vga_color_t background;
    uint8_t x;
    uint8_t y;
} tty_state_t;

static tty_state_t tty;

void
tty_init() {
    hardware_info_t info = hardware_get_info();
    vga_init(info.vga_monochrome);
    tty.foreground = VGA_COLOR_GRAY;
    tty.background = VGA_COLOR_BLACK;
    tty.x = tty.y = 0;
}


void
tty_clear() {
    for (uint8_t row = 0; row < VGA_VIDEO_HEIGHT; row++) {
        for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
            vga_put_character(col, row, ' ',
                    vga_color_pack_pair(tty.foreground, tty.background));
        }
    }

    tty.x = tty.y = 0;
}

void
tty_set_color(
    vga_color_t new_foreground,
    vga_color_t new_background
) {
    tty.foreground = new_foreground;
    tty.background = new_background;
}

void
tty_set_pointer_position(
    uint8_t new_x,
    uint8_t new_y
) {
    tty.x = new_x;
    tty.y = new_y;
}

void
tty_put_character(
    char character
) {
    uint8_t color_pair = vga_color_pack_pair(tty.foreground, tty.background);
    
    if (character == '\n') {
        tty.x = 0;
        if (++tty.y >= VGA_VIDEO_HEIGHT) {
            tty.y--;
            tty_scroll_up();
        }
        return;
    }
    
    vga_put_character(tty.x, tty.y, character, color_pair);
    
    if (++tty.x >= VGA_VIDEO_WIDTH) {
        tty.x = 0;
        if (++tty.y >= VGA_VIDEO_HEIGHT) {
            tty.y--;
            tty_scroll_up();
        }
    }
}

void
tty_scroll_up() {
    char character;
    uint8_t color_pair;

    for (uint8_t row = 1; row < VGA_VIDEO_HEIGHT; row++) {
        for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
            character = vga_get_character(col, row, &color_pair);
            vga_put_character(col, row, ' ',
                    vga_color_pack_pair(tty.foreground, tty.background));
            vga_put_character(col, row - 1, character, color_pair);
        }
    }
    
    for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
        vga_put_character(col, VGA_VIDEO_HEIGHT - 1, ' ',
                vga_color_pack_pair(tty.foreground, tty.background));
    }
}