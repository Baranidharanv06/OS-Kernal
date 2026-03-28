[org 0x7c00]
[bits 16]

    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    ; Jump directly to kernel right after bootloader
    jmp 0x0000:0x7e00

times 510-($-$$) db 0
dw 0xAA55
