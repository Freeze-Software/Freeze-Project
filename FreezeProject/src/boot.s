.section .text
.global _start

_start:

1:
    wfe
    b 1b
