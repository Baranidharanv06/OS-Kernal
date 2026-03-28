extern void keyboard_handler();

void kernel_main() {
    // Clear screen
    char* video = (char*)0xb8000;
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    // Print welcome message
    const char* msg = "Kernel ready! Press keys:";
    for (int i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2+1] = 0x07;
    }

    // Enable interrupts and wait
    __asm__ volatile ("sti");
    while(1);
}
