extern void idt_init();

void kernel_main() {
    char* video = (char*)0xb8000;
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    const char* msg = "Kernel ready! Press keys:";
    for (int i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2+1] = 0x07;
    }

    idt_init();

    __asm__ volatile ("sti");
    while(1);
}
