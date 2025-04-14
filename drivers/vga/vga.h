#ifndef __VOIKER_KERNEL_HARDWARE_VGA
#define __VOIKER_KERNEL_HARDWARE_VGA


#include <lib/types/bool.h>
#include <lib/types/int.h>


#define VGA_VIDEO_WIDTH  80
#define VGA_VIDEO_HEIGHT 25


typedef enum __voiker_vga_color {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_PURPLE        = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_GRAY          = 7,
    VGA_COLOR_DARK_GRAY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_PURPLE  = 13,
    VGA_COLOR_YELLOW        = 14,
    VGA_COLOR_WHITE         = 15
} vga_color_t;


void vga_init(bool monochrome);

uint8_t vga_color_pack_pair(vga_color_t foreground, vga_color_t background);
void vga_put_character(uint8_t x, uint8_t y, char character, uint8_t color_pair);

void vga_color_unpack_pair(uint8_t color, vga_color_t *foreground, vga_color_t *background);
char vga_get_character(uint8_t x, uint8_t y, uint8_t *color);


#endif
