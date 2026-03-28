extern void idt_init();

volatile char* video = (volatile char*)0xb8000;
int cursor = 8; // after "Kernel! "

void kernel_main() {
    // Clear screen
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    // Print "Kernel! "
    video[0]  = 'K'; video[1]  = 0x0A;
    video[2]  = 'e'; video[3]  = 0x0A;
    video[4]  = 'r'; video[5]  = 0x0A;
    video[6]  = 'n'; video[7]  = 0x0A;
    video[8]  = 'e'; video[9]  = 0x0A;
    video[10] = 'l'; video[11] = 0x0A;
    video[12] = '!'; video[13] = 0x0A;
    video[14] = ' '; video[15] = 0x0A;

    idt_init();
    __asm__ volatile ("sti");

    while(1) {
        __asm__ volatile ("hlt");
    }
}
