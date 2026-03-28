#include <stdint.h>
#include <stddef.h>

extern void *kmalloc(size_t size);
extern void kfree(void *ptr);

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

void print_str(const char* s) {
    for (int i = 0; s[i]; i++) {
        video[cursor*2]   = s[i];
        video[cursor*2+1] = 0x0A;
        cursor++;
    }
}

void print_hex(uint32_t val) {
    char buf[11] = "0x00000000";
    const char* hex = "0123456789ABCDEF";
    for (int i = 9; i >= 2; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }
    print_str(buf);
}

void kernel_main() {
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;

    print_str("kmalloc test: ");

    void* a = kmalloc(64);
    print_hex((uint32_t)a);
    print_str(" ");

    void* b = kmalloc(128);
    print_hex((uint32_t)b);
    print_str(" ");

    void* c = kmalloc(256);
    print_hex((uint32_t)c);

    print_str(" OK! Type: ");

    uint8_t last = 0;
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 0x01) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80) && scancode != last) {
                if (scancode < 58) {
                    char key = scancode_to_ascii[scancode];
                    if (key) {
                        video[cursor*2]   = key;
                        video[cursor*2+1] = 0x0F;
                        cursor++;
                    }
                }
            }
            last = scancode;
        }
    }
}
