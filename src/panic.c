#include <stdint.h>

extern volatile char* video;

typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esp, ebp, esi, edi;
} registers_t;

static void put_char(int row, int col, char c, uint8_t color) {
    int pos = row * 80 + col;
    video[pos * 2]     = c;
    video[pos * 2 + 1] = color;
}

static void put_str(int row, int* col, const char* s, uint8_t color) {
    for (int i = 0; s[i]; i++) {
        put_char(row, (*col)++, s[i], color);
    }
}

static void put_hex(int row, int* col, uint32_t val, uint8_t color) {
    const char* hex = "0123456789ABCDEF";
    char buf[11] = "0x00000000";
    for (int i = 9; i >= 2; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }
    put_str(row, col, buf, color);
}

static void fill_row(int row, uint8_t color) {
    for (int i = 0; i < 80; i++) put_char(row, i, ' ', color);
}

static void draw_border_row(int row, char left, char fill, char right, uint8_t color) {
    put_char(row, 0, left, color);
    for (int i = 1; i < 79; i++) put_char(row, i, fill, color);
    put_char(row, 79, right, color);
}

static void draw_content_row(int row, uint8_t color) {
    put_char(row, 0,  '\xBA', color); // double vertical bar
    put_char(row, 79, '\xBA', color);
    for (int i = 1; i < 79; i++) put_char(row, i, ' ', color);
}

void kernel_panic_full(const char* exception, uint32_t code, registers_t* regs) {
    // fill screen red
    for (int i = 0; i < 80 * 25; i++) {
        video[i*2]     = ' ';
        video[i*2 + 1] = 0x40;
    }

    uint8_t RED_WHITE  = 0x4F;
    uint8_t RED_YELLOW = 0x4E;
    uint8_t RED_RED    = 0x4C;
    uint8_t RED_CYAN   = 0x4B;

    // row 0: top border
    draw_border_row(0, '\xC9', '\xCD', '\xBB', RED_WHITE);

    // row 1: title
    draw_content_row(1, RED_WHITE);
    int col = 1;
    put_str(1, &col, "          *** KERNEL PANIC ***   MyKernel v2.0", RED_WHITE);

    // row 2: separator
    draw_border_row(2, '\xCC', '\xCD', '\xB9', RED_WHITE);

    // row 3: exception
    draw_content_row(3, RED_WHITE);
    col = 3;
    put_str(3, &col, "Exception : ", RED_YELLOW);
    put_str(3, &col, exception, RED_WHITE);

    // row 4: error code
    draw_content_row(4, RED_WHITE);
    col = 3;
    put_str(4, &col, "Error Code: ", RED_YELLOW);
    put_hex(4, &col, code, RED_WHITE);

    // row 5: separator
    draw_border_row(5, '\xCC', '\xCD', '\xB9', RED_WHITE);

    // row 6: registers header
    draw_content_row(6, RED_WHITE);
    col = 3;
    put_str(6, &col, "REGISTERS AT TIME OF CRASH:", RED_CYAN);

    // row 7: EAX EBX
    draw_content_row(7, RED_WHITE);
    col = 3;
    put_str(7, &col, "EAX: ", RED_YELLOW);
    put_hex(7, &col, regs ? regs->eax : 0, RED_WHITE);
    col = 25;
    put_str(7, &col, "EBX: ", RED_YELLOW);
    put_hex(7, &col, regs ? regs->ebx : 0, RED_WHITE);

    // row 8: ECX EDX
    draw_content_row(8, RED_WHITE);
    col = 3;
    put_str(8, &col, "ECX: ", RED_YELLOW);
    put_hex(8, &col, regs ? regs->ecx : 0, RED_WHITE);
    col = 25;
    put_str(8, &col, "EDX: ", RED_YELLOW);
    put_hex(8, &col, regs ? regs->edx : 0, RED_WHITE);

    // row 9: ESP EBP
    draw_content_row(9, RED_WHITE);
    col = 3;
    put_str(9, &col, "ESP: ", RED_YELLOW);
    put_hex(9, &col, regs ? regs->esp : 0, RED_WHITE);
    col = 25;
    put_str(9, &col, "EBP: ", RED_YELLOW);
    put_hex(9, &col, regs ? regs->ebp : 0, RED_WHITE);

    // row 10: ESI EDI
    draw_content_row(10, RED_WHITE);
    col = 3;
    put_str(10, &col, "ESI: ", RED_YELLOW);
    put_hex(10, &col, regs ? regs->esi : 0, RED_WHITE);
    col = 25;
    put_str(10, &col, "EDI: ", RED_YELLOW);
    put_hex(10, &col, regs ? regs->edi : 0, RED_WHITE);

    // row 11: separator
    draw_border_row(11, '\xCC', '\xCD', '\xB9', RED_WHITE);

    // row 12: halted message
    draw_content_row(12, RED_WHITE);
    col = 3;
    put_str(12, &col, "System halted. Restart QEMU to reboot.", RED_RED);

    // row 13: github
    draw_content_row(13, RED_WHITE);
    col = 3;
    put_str(13, &col, "github.com/Baranidharanv06/OS-Kernal", RED_YELLOW);

    // row 14: bottom border
    draw_border_row(14, '\xC8', '\xCD', '\xBC', RED_WHITE);

    __asm__ volatile ("cli; hlt");
    while(1);
}

// simple wrapper for manual panic
void kernel_panic(const char* exception, uint32_t code) {
    kernel_panic_full(exception, code, (void*)0);
}
