extern void idt_init();

void kernel_main() {
    volatile char* video = (volatile char*)0xb8000;

    // Clear screen
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    // Print message
    const char* msg = "Kernel running! Type keys: ";
    for (int i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2+1] = 0x0A; // green
    }

    idt_init();
    __asm__ volatile ("sti");

    while(1) {
        __asm__ volatile ("hlt");
    }
}
