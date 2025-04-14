#include <kernel/drivers/keyboard.h>
#include <kernel/io/port.h>
#include <kernel/io/io.h>
#include <kernel/misc.h>

#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

#define KEYBOARD_ACK 0xFA
#define KEYBOARD_RESEND 0xFE

#define KEY_PRESSED 0
#define KEY_RELEASED 0x80

static unsigned char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char shift_keyboard_map[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define KEY_LSHIFT 42
#define KEY_RSHIFT 54
#define KEY_CAPS 58

static int shift_pressed = 0;
static int capslock_enabled = 0;
static int keyboard_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;

void keyboard_wait(void) {
    int timeout = 100000;
    while (timeout-- && (keyboard_read_status() & 2)) {
        io_wait();
    }
}

uint8_t keyboard_read_status(void) {
    return inb(KEYBOARD_STATUS_PORT);
}

uint8_t keyboard_read_data(void) {
    return inb(KEYBOARD_DATA_PORT);
}

int keyboard_send_command(uint8_t command) {
    keyboard_wait();
    outb(KEYBOARD_COMMAND_PORT, command);
    
    keyboard_wait();
    uint8_t response = keyboard_read_data();
    
    switch (response) {
        case KEYBOARD_ACK:
            return 0;
            
        case KEYBOARD_RESEND:
            return keyboard_send_command(command);
            
        default:
            return -1;
    }
}

int keyboard_send_data(uint8_t data) {
    keyboard_wait();
    outb(KEYBOARD_DATA_PORT, data);
    
    keyboard_wait();
    uint8_t response = keyboard_read_data();
    
    switch (response) {
        case KEYBOARD_ACK:
            return 0;
            
        case KEYBOARD_RESEND:
            return keyboard_send_data(data);
            
        default:
            return -1;
    }
}

void keyboard_init(void) {
    keyboard_wait();
    outb(KEYBOARD_COMMAND_PORT, 0xAE);
    
    keyboard_send_command(0xF3);
    keyboard_send_data(0x00);
    
    keyboard_send_command(0xF4);
    
    buffer_head = 0;
    buffer_tail = 0;
    shift_pressed = 0;
    capslock_enabled = 0;
}

void keyboard_buffer_push(char c) {
    if (c == 0) return;
    
    int next_head = (buffer_head + 1) % 256;
    if (next_head != buffer_tail) {
        keyboard_buffer[buffer_head] = c;
        buffer_head = next_head;
    }
}

char keyboard_buffer_pop(void) {
    if (buffer_head == buffer_tail) {
        return 0;
    }
    
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return c;
}

char keyboard_get_char(uint8_t scancode) {
    switch (scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift_pressed = 1;
            return 0;
        
        case (KEY_LSHIFT | KEY_RELEASED):
        case (KEY_RSHIFT | KEY_RELEASED):
            shift_pressed = 0;
            return 0;
            
        case KEY_CAPS:
            capslock_enabled = !capslock_enabled;
            return 0;
            
        default:
            if (scancode & KEY_RELEASED) {
                return 0;
            }
            
            int uppercase = shift_pressed;
            
            if ((scancode >= 16 && scancode <= 25) || 
                (scancode >= 30 && scancode <= 38) || 
                (scancode >= 44 && scancode <= 50)) {
                if (capslock_enabled) {
                    uppercase = !uppercase;
                }
            }
            
            return uppercase ? shift_keyboard_map[scancode] : keyboard_map[scancode];
    }
}

void keyboard_handler(void) {
    uint8_t scancode = keyboard_read_data();
    char c = keyboard_get_char(scancode);
    
    switch (c) {
        case 0:
            break;
            
        default:
            keyboard_buffer_push(c);
            klogf("%c", c);
            break;
    }
}

char keyboard_getc(void) {
    char c = 0;
    while (c == 0) {
        c = keyboard_buffer_pop();
        switch (c) {
            case 0:
                __asm__ volatile("hlt");
                break;
                
            default:
                return c;
        }
    }
    return c;
}

int keyboard_getc_nonblock(void) {
    return keyboard_buffer_pop();
}