#include <stdint.h>

extern volatile char* video;

void panic_fill_row(int row, char c, uint8_t color) {
    for (int col = 0; col < 80; col++) {
        int pos = row * 80 + col;
        video[pos * 2]     = c;
        video[pos * 2 + 1] = color;
    }
}

void panic_print_str(const char* s, int* col, int row, uint8_t color) {
    for (int i = 0; s[i]; i++) {
        int pos = row * 80 + *col;
        video[pos * 2]     = s[i];
        video[pos * 2 + 1] = color;
        (*col)++;
    }
}

void kernel_panic(const char* exception, uint32_t code) {
    // fill entire screen red
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2]     = ' ';
        video[i * 2 + 1] = 0x40;
    }

    // top bar
    panic_fill_row(0, ' ', 0x4F);
    int col = 2;
    panic_print_str("*** KERNEL PANIC ***", &col, 0, 0x4F);

    // exception name
    col = 4;
    panic_print_str("Exception: ", &col, 4, 0x4E);
    panic_print_str(exception,    &col, 4, 0x4F);

    // error code
    col = 4;
    panic_print_str("Error code: 0x", &col, 6, 0x4E);
    const char* hex = "0123456789ABCDEF";
    char buf[9]; buf[8] = '\0';
    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[code & 0xF];
        code >>= 4;
    }
    panic_print_str(buf, &col, 6, 0x4F);

    // message
    col = 4;
    panic_print_str("A fatal error occurred. System halted.", &col, 8, 0x4C);
    col = 4;
    panic_print_str("Restart QEMU to reboot.", &col, 9, 0x4C);

    // bottom bar
    panic_fill_row(24, ' ', 0x4F);
    col = 2;
    panic_print_str("MyKernel v2.0  |  github.com/Baranidharanv06/OS-Kernal", &col, 24, 0x4F);

    __asm__ volatile ("cli; hlt");
    while(1);
}
