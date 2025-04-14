.extern gdtr

.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    lgdt [gdtr]
    mov %ax, 0x10
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %ss, %ax
    ret
