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

static char* video = (char*)0xb8000;
static int cursor = 26; // start after welcome message

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (!(scancode & 0x80)) {
        if (scancode < sizeof(scancode_to_ascii)) {
            char key = scancode_to_ascii[scancode];
            if (key) {
                video[cursor * 2] = key;
                video[cursor * 2 + 1] = 0x0F; // bright white
                cursor++;
            }
        }
    }
}
