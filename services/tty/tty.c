#include <kernel/hardware/hardware.h>
#include <kernel/hardware/vga.h>
#include <kernel/tty.h>
#include <lib/types/int.h>
#include "lib/string.h"

#define TAB_SIZE 4

typedef struct {
    vga_color_t foreground;
    vga_color_t background;
    uint8_t x;
    uint8_t y;
    bool cursor_enabled;
} tty_state_t;

static tty_state_t tty;

void tty_update_cursor(void);

void tty_init() {
    hardware_info_t info = hardware_get_info();
    vga_init(info.vga_monochrome);
    tty.foreground = VGA_COLOR_WHITE;
    tty.background = VGA_COLOR_BLACK;
    tty.x = tty.y = 0;
    tty.cursor_enabled = true;
    tty_clear();
    tty_update_cursor();
}

void tty_clear() {
    uint8_t color_pair = vga_color_pack_pair(tty.foreground, tty.background);
    
    for (uint8_t row = 0; row < VGA_VIDEO_HEIGHT; row++) {
        for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
            vga_put_character(col, row, ' ', color_pair);
        }
    }

    tty.x = tty.y = 0;
    tty_update_cursor();
}

void tty_set_color(vga_color_t new_foreground, vga_color_t new_background) {
    tty.foreground = new_foreground;
    tty.background = new_background;
}

void tty_set_pointer_position(uint8_t new_x, uint8_t new_y) {
    if (new_x >= VGA_VIDEO_WIDTH || new_y >= VGA_VIDEO_HEIGHT) {
        return;
    }
    
    tty.x = new_x;
    tty.y = new_y;
    tty_update_cursor();
}

void tty_enable_cursor(bool enable) {
    tty.cursor_enabled = enable;
    
    if (enable) {
        tty_update_cursor();
    } else {
        vga_disable_cursor();
    }
}

void tty_update_cursor() {
    if (tty.cursor_enabled) {
        vga_set_cursor_position(tty.x, tty.y);
    }
}

void tty_handle_special_char(char c) {
    switch (c) {
        case '\n':
            tty.x = 0;
            if (++tty.y >= VGA_VIDEO_HEIGHT) {
                tty.y--;
                tty_scroll_up();
            }
            break;
            
        case '\r':
            tty.x = 0;
            break;
            
        case '\t':
            for (size_t i = 0; i < (size_t)(TAB_SIZE - (tty.x % TAB_SIZE)); i++) {
                tty_put_character(' ');
            }
            break;
            
        case '\b':
            if (tty.x > 0) {
                tty.x--;
                vga_put_character(tty.x, tty.y, ' ', 
                    vga_color_pack_pair(tty.foreground, tty.background));
            } else if (tty.y > 0) {
                tty.y--;
                tty.x = VGA_VIDEO_WIDTH - 1;
                vga_put_character(tty.x, tty.y, ' ', 
                    vga_color_pack_pair(tty.foreground, tty.background));
            }
            break;
    }
}

void tty_put_character(char character) {
    uint8_t color_pair = vga_color_pack_pair(tty.foreground, tty.background);
    
    if (character == '\n' || character == '\r' || character == '\t' || character == '\b') {
        tty_handle_special_char(character);
        tty_update_cursor();
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
    
    tty_update_cursor();
}

void tty_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        tty_put_character(data[i]);
    }
}

void tty_write_string(const char* string) {
    while (*string) {
        tty_put_character(*string++);
    }
}

void tty_scroll_up() {
    char character;
    uint8_t color_pair;
    uint8_t blank_color = vga_color_pack_pair(tty.foreground, tty.background);

    for (uint8_t row = 1; row < VGA_VIDEO_HEIGHT; row++) {
        for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
            character = vga_get_character(col, row, &color_pair);
            
            vga_put_character(col, row - 1, character, color_pair);
        }
    }
    
    for (uint8_t col = 0; col < VGA_VIDEO_WIDTH; col++) {
        vga_put_character(col, VGA_VIDEO_HEIGHT - 1, ' ', blank_color);
    }
    
    tty_update_cursor();
}