#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60

static const char scancode_to_ascii[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' '
};

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Video memory pointer
static char* video = (char*)0xb8000;
static int cursor = 0;

void print_char(char c) {
    video[cursor * 2] = c;
    video[cursor * 2 + 1] = 0x07;
    cursor++;
}

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (!(scancode & 0x80)) {
        char key = scancode_to_ascii[scancode];
        if (key) print_char(key);
    }
    outb(0x20, 0x20); // Send EOI to PIC
}
