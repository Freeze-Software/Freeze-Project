    .text
    .global _start
    .type _start, @function
_start:
    mov $0x200000, %esp
    mov $0x200000, %ebp
    call c_entry
hang:
    cli
    hlt
    jmp hang
