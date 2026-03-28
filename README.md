# MyKernel 🖥️

A bare-metal x86 operating system kernel built from scratch in C and x86 Assembly, running on QEMU. This project implements core OS primitives including a Multiboot-compliant bootloader, VGA text rendering, and PS/2 keyboard input via hardware polling — all without any standard library or operating system support.

---

## Demo

```
Type keys: hello world_
```

The kernel boots, clears the screen, and responds to every keypress in real time — all running on raw x86 hardware (emulated via QEMU).

---

## Features

- **Multiboot bootloader** — GRUB-compatible entry point, switches CPU to 32-bit protected mode
- **VGA text rendering** — direct writes to the 0xb8000 memory-mapped text buffer
- **PS/2 keyboard driver** — hardware polling via I/O ports 0x60 and 0x64 with scancode-to-ASCII translation
- **Freestanding C kernel** — zero standard library dependencies, compiled with `-ffreestanding -nostdlib`
- **Cross-compiled on macOS** — full toolchain using `x86_64-elf-gcc` and `x86_64-elf-ld`

---

## Architecture

```
mykernel/
├── src/
│   ├── boot.asm       # Multiboot header + _start entry point
│   ├── kernel.c       # kernel_main — screen init, keyboard poll loop
│   ├── keyboard.c     # PS/2 keyboard driver (scancode → ASCII)
│   ├── idt.c          # Interrupt Descriptor Table setup + PIC remapping
│   ├── isr.asm        # Assembly interrupt service routine stub
│   └── linker.ld      # Memory layout: kernel loads at 1MB (0x100000)
├── build/             # Compiled output (kernel.elf)
└── Makefile
```

---

## How It Works

### Boot sequence
1. GRUB reads the **Multiboot header** in `boot.asm` (magic number `0x1BADB002`)
2. CPU is placed into **32-bit protected mode** by GRUB
3. `_start` sets up the stack pointer and calls `kernel_main()`

### Kernel main
- Clears the VGA text buffer (80×25 chars at `0xb8000`)
- Calls `idt_init()` to remap the PIC and load the IDT
- Enables hardware interrupts with `sti`
- Enters a polling loop: continuously checks port `0x64` for keyboard input

### Keyboard driver
- Reads the **status register** at port `0x64` — if bit 0 is set, a scancode is ready
- Reads the **scancode** from port `0x60`
- Translates using a `scancode_to_ascii[]` lookup table
- Writes the ASCII character to video memory and advances the cursor

### Cross-compilation (macOS)
macOS ships with a Mach-O toolchain that cannot produce ELF binaries for x86. The project uses:
- `x86_64-elf-gcc` — produces ELF32 object files
- `x86_64-elf-ld` — links with a custom linker script to produce a flat ELF kernel

---

## Build & Run

### Prerequisites

```bash
brew install nasm qemu x86_64-elf-gcc x86_64-elf-binutils
```

### Build

```bash
make all
```

### Run

```bash
qemu-system-i386 -kernel build/kernel.elf
```

### Clean

```bash
make clean
```

---

## Tech Stack

| Component | Technology |
|---|---|
| Language | C (freestanding), x86 Assembly (NASM) |
| Assembler | NASM |
| Compiler | x86_64-elf-gcc (cross-compiler) |
| Linker | x86_64-elf-ld with custom linker script |
| Emulator | QEMU (qemu-system-i386) |
| Boot protocol | Multiboot (GRUB-compatible) |
| Platform | macOS (Apple Silicon, cross-compiled to x86) |

---

## What I Learned

- How a CPU boots from BIOS/GRUB into a custom kernel
- x86 protected mode and memory segmentation
- VGA text mode and direct memory-mapped I/O
- PS/2 keyboard protocol — scan codes, status registers, I/O port reads
- Interrupt Descriptor Tables (IDT) and PIC remapping
- Writing freestanding C — no `malloc`, no `printf`, no OS beneath you
- Cross-compiling for a different architecture and binary format on macOS
- GNU `make`, linker scripts, and ELF binary structure

---

## Roadmap

- [ ] Memory management — implement `kmalloc` / `kfree` with a simple bump allocator
- [ ] Interrupt-driven keyboard (replace polling with IDT + IRQ1)
- [ ] Basic shell — command input, parsing, and built-in commands
- [ ] Paging — virtual memory with a page table
- [ ] Filesystem — simple in-memory FAT-style filesystem

---

## References

- [OSDev Wiki](https://wiki.osdev.org) — the definitive OS development reference
- [James Molloy's Kernel Tutorial](http://www.jamesmolloy.co.uk/tutorial_html/) — protected mode and IDT walkthrough
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [Intel x86 Architecture Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

---

## Related Projects

- [myshell](https://github.com/Baranidharanv06/myshell) — a Unix shell built from scratch in C, implementing `fork()`, `execvp()`, `pipe()`, and `dup2()`

---

*Built on macOS (Apple Silicon) · Emulated on QEMU i386 · No standard library used*
