[bits 32]
extern keyboard_handler

global keyboard_isr
keyboard_isr:
    pusha
    call keyboard_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret
