#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

extern void keyboard_isr();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++)
        idt_set_gate(i, 0, 0, 0);

    // Remap PIC
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA,    0x20);
    outb(PIC2_DATA,    0x28);
    outb(PIC1_DATA,    0x04);
    outb(PIC2_DATA,    0x02);
    outb(PIC1_DATA,    0x01);
    outb(PIC2_DATA,    0x01);
    outb(PIC1_DATA,    0x0);
    outb(PIC2_DATA,    0x0);

    // Get current code segment
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));

    // Register keyboard IRQ1 = 0x21
    idt_set_gate(0x21, (uint32_t)keyboard_isr, cs, 0x8E);

    __asm__ volatile ("lidt %0" :: "m"(idtp));
}
