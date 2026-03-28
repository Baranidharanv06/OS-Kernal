[org 0x7c00]
[bits 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Load kernel from disk
    mov bx, 0x1000      ; load kernel to 0x1000
    mov dh, 15          ; read 15 sectors
    mov dl, 0x80        ; drive number
    call disk_load

    ; Switch to protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_load:
    mov ah, 0x02
    mov al, dh
    mov ch, 0
    mov cl, 2
    mov dh, 0
    int 0x13
    ret

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000

    call 0x1000         ; jump to kernel_main

    jmp $

; GDT
gdt_start:
    dq 0
gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55
