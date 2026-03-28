#include <stdint.h>

volatile char* video = (volatile char*)0xb8000;
int cursor = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static const char scancode_to_ascii[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' '
};

void print_char(char c, int pos) {
    video[pos * 2]     = c;
    video[pos * 2 + 1] = 0x0A;
}

void kernel_main() {
    // Clear screen
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    // Print message
    const char* msg = "Type keys: ";
    for (int i = 0; msg[i]; i++) {
        print_char(msg[i], cursor++);
    }

    // Poll keyboard directly — no interrupts needed
    uint8_t last = 0;
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 0x01) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80) && scancode != last) {
                if (scancode < 58) {
                    char key = scancode_to_ascii[scancode];
                    if (key) print_char(key, cursor++);
                }
            }
            last = scancode;
        }
    }
}
