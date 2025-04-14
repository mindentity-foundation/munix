#include <kernel/io/serial.h>
#include <kernel/tty.h>
#include <lib/types/vargs.h>
#include <lib/types/defs.h>
#include <lib/string.h>
#include <lib/io.h>



static void
formated_print(
    const char *string,
    va_list list,
    void *context,
    void (*put_string)(void *, const char *),
    void (*put_char)(void *, char)
) {
    char arg_character;
    char *arg_buffer;
    int arg_number;
    char arg_number_str[12] = { 0 };

    while (*string) {
        char character = *string++;
        if (character == '%') {
            character = *string++;
            switch (character) {
                case 's':
                    arg_buffer = va_arg(list, char*);
                    put_string(context, arg_buffer);
                    break;
                case 'd':
                    arg_number = va_arg(list, int);
                    string_int_to_string(arg_number, arg_number_str, 10);
                    put_string(context, arg_number_str);
                    break;
                case 'x':
                    arg_number = va_arg(list, int);
                    string_int_to_string(arg_number, arg_number_str, 16);
                    put_string(context, arg_number_str);
                    break;
                case 'c':
                    arg_character = va_arg(list, int);
                    put_char(context, arg_character);
                    break;
                case '%':
                    put_char(context, '%');
                    break;
                default:
                    put_char(context, '%');
                    put_char(context, character);
                    break;
            }
        } else {
            put_char(context, character);
        }
    }
}



static void 
tty_put_string(
    void *context,
    const char *string
) {
    (void)context;
    for (size_t index = 0; index < string_length(string); index++) {
        tty_put_character(string[index]);
    }
}

static void
tty_put_char_wrapper(
    void *context,
    char character
) {
    (void)context;
    tty_put_character(character);
}


static void
serial_put_string(
    void *context,
    const char *string
) {
    uint16_t com_port = *(uint16_t*)context;
    for (size_t index = 0; index < string_length(string); index++) {
        if (string[index] == '\n') {
            serial_port_write(com_port, '\r');
        }
        serial_port_write(com_port, string[index]);
    }
}

static void
serial_put_char(
    void *context,
    char character
) {
    uint16_t com_port = *(uint16_t*)context;
    if (character == '\n') {
        serial_port_write(com_port, '\r');
    }
    serial_port_write(com_port, character);
}



void
kput(
    const char *string
) {
    tty_put_string(NULL, string);
}

void
kputf(
    const char *string,
    ...
) {
    va_list list;
    va_start(list, string);
    formated_print(string, list, NULL, tty_put_string, tty_put_char_wrapper);
    va_end(list);
}


void
sput(
    uint16_t com_port,
    const char *string
) {
    serial_put_string(&com_port, string);
}

void
sputf(
    uint16_t com_port,
    const char *string,
    ...
) {
    va_list list;
    va_start(list, string);
    formated_print(string, list, &com_port, serial_put_string, serial_put_char);
    va_end(list);
}
