#include <kernel/interupts/gdt.h>


gdt_entry_t gdt_entries[3];
gdtr_t gdtr;


void
gdt_set_gate(
    size_t index,
    uint64_t base,
    uint64_t limit,
    uint8_t access_byte,
    uint8_t granularity
) {
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_low = (base >> 24) & 0xFF;

    gdt_entries[index].limit_low = (limit & 0xFFFF);
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;

    gdt_entries[index].granularity |= granularity & 0xF0;
    gdt_entries[index].access_byte = access_byte;
}

void
gdt_install() {
    gdtr.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gdtr.address = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0x00400000, 0x003FFFFF, 0x9A, 0xCF);
    gdt_set_gate(1, 0x00800000, 0x003FFFFF, 0x92, 0xCF);
    gdt_flush();
}
