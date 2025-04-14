#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/* Basic types for kernel */
typedef unsigned char uint8_t;

#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Function declarations
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_get_scancode(void);
uint8_t keyboard_read_status(void);
uint8_t keyboard_read_data(void);

#endif
