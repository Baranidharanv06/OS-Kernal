[bits 32]
extern kernel_panic_full

global isr0
global isr13
global isr14

%macro ISR_HANDLER 2
    pusha
    mov eax, esp        ; pointer to pushed registers
    push dword %2       ; error code
    push dword %1       ; exception name string
    push eax            ; registers pointer
    call kernel_panic_full
    popa
    iret
%endmacro

isr0:
    ISR_HANDLER isr0_msg, 0

isr13:
    ISR_HANDLER isr13_msg, 0

isr14:
    ISR_HANDLER isr14_msg, 0

isr0_msg:  db "Division by Zero (Exception #0)", 0
isr13_msg: db "General Protection Fault (Exception #13)", 0
isr14_msg: db "Page Fault (Exception #14)", 0
