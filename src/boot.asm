; boot.asm
[org 0x7c00]

; Print a message to the screen
mov ah, 0x0E ; Teletype output (BIOS interrupt)
mov al, 'H'
int 0x10
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10
mov al, '!'
int 0x10

; Infinite loop to prevent reboot
jmp $

; Boot loader signature
times 510-($-$$) db 0 ; Padding to fill 512 bytes
dw 0xAA55 ; Bootable drive signature