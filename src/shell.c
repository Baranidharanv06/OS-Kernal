#include <stdint.h>
#include <stddef.h>

#define MAX_INPUT 256

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

void print_char(char c, uint8_t color) {
    video[cursor*2]   = c;
    video[cursor*2+1] = color;
    cursor++;
}

void clear_screen() {
    for (int i = 0; i < 80*25*2; i++) video[i] = 0;
    cursor = 0;
}

int str_equal(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

void cmd_help() {
    print_str("\nAvailable commands:\n");
    print_str("  help  - show this message\n");
    print_str("  clear - clear the screen\n");
    print_str("  mem   - show memory info\n");
    print_str("  about - about this kernel\n");
}

void cmd_about() {
    print_str("\nMyKernel v1.0 - built from scratch in C + Assembly\n");
    print_str("Author: Baranidharanv06\n");
}

void cmd_mem() {
    print_str("\nHeap start: 0x00200000\n");
    print_str("Heap size:  1MB\n");
}

void execute(char* buf) {
    if (str_equal(buf, "help"))       cmd_help();
    else if (str_equal(buf, "clear")) { clear_screen(); return; }
    else if (str_equal(buf, "about")) cmd_about();
    else if (str_equal(buf, "mem"))   cmd_mem();
    else {
        print_str("\nUnknown: ");
        print_str(buf);
    }
}

void shell_main() {
    clear_screen();
    print_str("MyKernel Shell - type 'help' for commands\n");
    print_str("mysh> ");

    char buf[MAX_INPUT];
    int len = 0;
    uint8_t last = 0;

    while (1) {
        uint8_t status = inb(0x64);
        if (!(status & 0x01)) continue;

        uint8_t scancode = inb(0x60);
        if (scancode == last) continue;
        last = scancode;

        if (scancode & 0x80) continue;

        char key = (scancode < 58) ? scancode_to_ascii[scancode] : 0;
        if (!key) continue;

        if (key == '\n') {
            buf[len] = '\0';
            print_char('\n', 0x0A);
            execute(buf);
            len = 0;
            print_str("\nmysh> ");
        } else if (key == '\b') {
            if (len > 0) {
                len--;
                cursor--;
                video[cursor*2] = ' ';
            }
        } else if (len < MAX_INPUT - 1) {
            buf[len++] = key;
            print_char(key, 0x0F);
        }
    }
}
