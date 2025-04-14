#ifndef __VOIKER_KERNEL_TTY
#define __VOIKER_KERNEL_TTY


#include <kernel/hardware/vga.h>
#include <lib/types/int.h>


void tty_init();

void tty_clear();
void tty_set_color(vga_color_t new_foreground, vga_color_t new_background);
void tty_set_pointer_position(uint8_t new_x, uint8_t new_y);
void tty_put_character(char character);
void tty_scroll_up();


#endif
