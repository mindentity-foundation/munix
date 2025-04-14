#ifndef __VOIKER_KERNEL_GDT
#define __VOIKER_KERNEL_GDT


#include <lib/types/int.h>
#include <lib/types/defs.h>


typedef struct __voiker_gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access_byte;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;


typedef struct __voiker_gdtr {
    uint16_t limit;
    uint32_t address;
} __attribute__((packed)) gdtr_t;


extern void gdt_flush();
void gdt_set_gate(size_t index, uint64_t base, uint64_t limit, uint8_t access_byte, uint8_t granularity);
void gdt_install();


#endif
