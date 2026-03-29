[bits 32]
extern kernel_panic

global isr0
global isr13
global isr14

isr0:
    pusha
    push dword 0
    push dword isr0_msg
    call kernel_panic
    popa
    iret

isr13:
    pusha
    push dword 0
    push dword isr13_msg
    call kernel_panic
    popa
    iret

isr14:
    pusha
    push dword 0
    push dword isr14_msg
    call kernel_panic
    popa
    iret

isr0_msg:  db "Division by Zero (Exception #0)", 0
isr13_msg: db "General Protection Fault (Exception #13)", 0
isr14_msg: db "Page Fault (Exception #14)", 0
