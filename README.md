# MyKernel 🖥️

A bare-metal x86 operating system kernel built from scratch in C and x86 Assembly, running on QEMU. Implements core OS primitives — a Multiboot-compliant bootloader, VGA text rendering, PS/2 keyboard input, a bump memory allocator, and an interactive kernel shell — all without any standard library or operating system support.

---

## Demo

```
MyKernel Shell - type 'help' for commands
mysh> help

Available commands:
  help  - show this message
  clear - clear the screen
  mem   - show memory info
  about - about this kernel

mysh> mem

Heap start: 0x00200000
Heap size:  1MB

mysh> about

MyKernel v1.0 - built from scratch in C + Assembly
Author: Baranidharanv06

mysh> _
```

---

## Features

- **Multiboot bootloader** — GRUB-compatible entry point, switches CPU to 32-bit protected mode
- **VGA text rendering** — direct writes to the 0xb8000 memory-mapped text buffer with color support
- **PS/2 keyboard driver** — hardware polling via I/O ports 0x60/0x64 with scancode-to-ASCII translation
- **Bump memory allocator** — `kmalloc`/`kfree` with 8-byte alignment and heap bounds checking
- **Interactive kernel shell** — command input, parsing, and built-in commands (`help`, `clear`, `mem`, `about`)
- **Freestanding C kernel** — zero standard library dependencies, compiled with `-ffreestanding -nostdlib`
- **Cross-compiled on macOS** — full toolchain using `x86_64-elf-gcc` and `x86_64-elf-ld`

---

## Architecture

```
mykernel/
├── src/
│   ├── boot.asm       # Multiboot header + _start entry point
│   ├── kernel.c       # kernel_main — calls shell_main()
│   ├── shell.c        # Interactive shell — input, parsing, commands
│   ├── memory.c       # Bump allocator — kmalloc(), kfree(), reset_heap()
│   ├── idt.c          # Interrupt Descriptor Table + PIC remapping
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
4. `kernel_main()` calls `shell_main()` — the kernel becomes an interactive shell

### Keyboard driver
- Polls the **status register** at port `0x64` — if bit 0 is set, a scancode is waiting
- Reads scancode from port `0x60`
- Translates using a `scancode_to_ascii[]` lookup table
- Handles Enter (execute command), Backspace (delete char), and printable characters

### Shell
- Reads keypresses into a 256-byte buffer
- On Enter: null-terminates the buffer and calls `execute()`
- `execute()` does string comparison against known commands and runs the matching function
- Loops back to `mysh>` prompt after every command

### Memory allocator (bump allocator)
- Heap starts at `0x200000` (2MB) — safely above the kernel which loads at 1MB
- `kmalloc(size)` aligns the current pointer to 8 bytes, checks bounds, bumps forward and returns the address
- `kfree()` is a no-op — bump allocators don't reclaim individual blocks
- `reset_heap()` resets the pointer to the start for testing

### VGA text rendering
- Text mode buffer at `0xb8000` — 80×25 characters, 2 bytes per cell (char + color attribute)
- Color byte `0x0A` = green on black, `0x0F` = white on black

### Cross-compilation (macOS)
macOS ships with a Mach-O toolchain that cannot produce ELF binaries for x86. This project uses:
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
- Implementing `kmalloc`/`kfree` with a bump allocator from scratch
- Building a command parser and shell loop without any standard library
- Writing freestanding C — no `malloc`, no `printf`, no OS beneath you
- Cross-compiling for a different architecture and binary format on macOS
- GNU `make`, linker scripts, and ELF binary structure
- Debugging bare-metal code using QEMU

---

## Roadmap

- [ ] Interrupt-driven keyboard (replace polling with IDT + IRQ1)
- [ ] Paging — virtual memory with a page table
- [ ] `echo` command that uses `kmalloc` to store strings dynamically
- [ ] Process scheduler — basic round-robin multitasking
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
