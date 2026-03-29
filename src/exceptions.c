#include <stdint.h>

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
static struct idt_ptr   idtp;

extern void isr0();
extern void isr13();
extern void isr14();

void idt_set_gate(uint8_t num, uint32_t base) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = 0x08;
    idt[num].zero      = 0;
    idt[num].flags     = 0x8E;
}

void exceptions_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt[i].base_low = 0; idt[i].base_high = 0;
        idt[i].selector = 0; idt[i].zero = 0; idt[i].flags = 0;
    }

    idt_set_gate(0,  (uint32_t)isr0);
    idt_set_gate(13, (uint32_t)isr13);
    idt_set_gate(14, (uint32_t)isr14);

    __asm__ volatile ("lidt %0" :: "m"(idtp));
}
