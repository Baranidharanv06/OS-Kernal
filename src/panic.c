#include <stdint.h>

volatile char* video = (volatile char*)0xb8000;

void panic_print_str(const char* s, int* col, int row, uint8_t color) {
    for (int i = 0; s[i]; i++) {
        int pos = (row * 80 + *col);
        video[pos * 2]     = s[i];
        video[pos * 2 + 1] = color;
        (*col)++;
    }
}

void panic_fill_row(int row, char c, uint8_t color) {
    for (int col = 0; col < 80; col++) {
        int pos = row * 80 + col;
        video[pos * 2]     = c;
        video[pos * 2 + 1] = color;
    }
}

void kernel_panic(const char* exception, uint32_t code) {
    // fill entire screen dark red
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2]     = ' ';
        video[i * 2 + 1] = 0x40; // red background
    }

    // top bar
    panic_fill_row(0, ' ', 0x4F); // red bg white fg
    int col = 1;
    panic_print_str("  KERNEL PANIC  ", &col, 0, 0x4F);

    // blank row
    panic_fill_row(2, ' ', 0x40);

    // exception name
    col = 4;
    panic_print_str("Exception: ", &col, 4, 0x4E); // yellow
    panic_print_str(exception,    &col, 4, 0x4F); // white

    // error code
    col = 4;
    panic_print_str("Error code: 0x", &col, 6, 0x4E);

    // print hex error code
    const char* hex = "0123456789ABCDEF";
    char buf[9];
    buf[8] = '\0';
    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[code & 0xF];
        code >>= 4;
    }
    panic_print_str(buf, &col, 6, 0x4F);

    // message
    col = 4;
    panic_print_str("The kernel has encountered a fatal error.", &col, 8, 0x4C);
    col = 4;
    panic_print_str("System halted. Please restart QEMU.", &col, 9, 0x4C);

    // bottom bar
    panic_fill_row(24, ' ', 0x4F);
    col = 1;
    panic_print_str("  MyKernel v1.0  |  github.com/Baranidharanv06/OS-Kernal", &col, 24, 0x4F);

    // halt forever
    __asm__ volatile ("cli; hlt");
    while(1);
}
