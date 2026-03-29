# MyKernel 🖥️

A bare-metal x86 operating system kernel built from scratch in C and x86 Assembly, running on QEMU. Implements core OS primitives — a Multiboot-compliant bootloader, VGA text rendering, PS/2 keyboard input, a bump memory allocator, an interactive kernel shell, and a professional kernel panic handler with CPU register dump — all without any standard library or operating system support.

---

## Demo

### Shell
```
MyKernel Shell v2.0 - type 'help' for commands
mysh> help

Available commands:
  help      - show this message
  clear     - clear the screen
  mem       - show memory info
  about     - about this kernel
  panic     - trigger kernel panic (test)
  divzero   - trigger division by zero

mysh> mem

Heap start: 0x00200000
Heap size:  1MB

mysh> about

MyKernel v2.0 - built from scratch in C + Assembly
Author: Baranidharanv06

mysh> panic
```

### Kernel Panic Screen
```
╔══════════════════════════════════════════════════════════════════════════════╗
║  ██  *** KERNEL PANIC ***  ██    MyKernel v2.0                              ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  Exception : Division by Zero (Exception #0)                                ║
║  Error Code: 0x00000000                                                     ║
║  Details   : A program attempted to divide a number by zero.                ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  REGISTERS AT TIME OF CRASH:                                                ║
║  EAX: 0x00000000        EBX: 0x00000000                                     ║
║  ECX: 0x00000001        EDX: 0x00000000                                     ║
║  ESP: 0x00090000        EBP: 0x00090000                                     ║
║  ESI: 0x00000000        EDI: 0x00000000                                     ║
╠══════════════════════════════════════════════════════════════════════════════╣
║  System halted. Restart QEMU to reboot.                                     ║
║  github.com/Baranidharanv06/OS-Kernal                                       ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

---

## Features

### v1.0
- **Multiboot bootloader** — GRUB-compatible entry point, switches CPU to 32-bit protected mode
- **VGA text rendering** — direct writes to the 0xb8000 memory-mapped text buffer with color support
- **PS/2 keyboard driver** — hardware polling via I/O ports 0x60/0x64 with scancode-to-ASCII translation
- **Bump memory allocator** — `kmalloc`/`kfree` with 8-byte alignment and heap bounds checking
- **Interactive kernel shell** — command input, parsing, and built-in commands (`help`, `clear`, `mem`, `about`)
- **Freestanding C kernel** — zero standard library dependencies, compiled with `-ffreestanding -nostdlib`
- **Cross-compiled on macOS** — full toolchain using `x86_64-elf-gcc` and `x86_64-elf-ld`

### v2.0
- **Kernel panic handler** — catches CPU exceptions (divide by zero, page fault, general protection fault) instead of crashing silently
- **CPU register dump** — displays all 8 registers (EAX, EBX, ECX, EDX, ESP, EBP, ESI, EDI) at exact crash time
- **Per-exception descriptions** — each exception type shows a different human-readable explanation
- **IDT (Interrupt Descriptor Table)** — 32-bit IDT setup with ISR stubs for exceptions 0, 13, and 14
- **Box-bordered panic UI** — double-line ASCII box borders, color-coded sections (yellow labels, white values, red messages, cyan headers)
- **Shell panic commands** — `panic` (manual trigger with `0xDEADBEEF`) and `divzero` (real CPU exception) for live testing

---

## Architecture

```
mykernel/
├── src/
│   ├── boot.asm          # Multiboot header + _start entry point
│   ├── kernel.c          # kernel_main — initialises exceptions then calls shell
│   ├── shell.c           # Interactive shell — input, parsing, commands
│   ├── memory.c          # Bump allocator — kmalloc(), kfree(), reset_heap()
│   ├── panic.c           # Kernel panic screen — register dump, borders, descriptions
│   ├── exceptions.c      # IDT setup + gate registration for CPU exceptions
│   ├── exceptions.asm    # Assembly ISR stubs — save registers, call kernel_panic_full
│   └── linker.ld         # Memory layout: kernel loads at 1MB (0x100000)
├── build/                # Compiled output (kernel.elf)
└── Makefile
```

---

## How It Works

### Boot sequence
1. GRUB reads the **Multiboot header** in `boot.asm` (magic number `0x1BADB002`)
2. CPU is placed into **32-bit protected mode** by GRUB
3. `_start` sets up the stack pointer and calls `kernel_main()`
4. `kernel_main()` calls `exceptions_init()` then `shell_main()`

### Kernel panic handler (v2.0)
- `exceptions_init()` sets up the IDT — a 256-entry table mapping each CPU exception number to a handler function
- When a CPU exception fires (e.g. divide by zero = exception 0), the CPU looks up entry 0 in the IDT and jumps to our ISR stub in `exceptions.asm`
- The ISR stub saves all registers with `pusha`, then calls `kernel_panic_full()` with the exception name, error code, and register pointer
- `kernel_panic_full()` draws the full panic screen, dumps all 8 registers, then halts with `cli; hlt`

### Per-exception descriptions
- Division by Zero → "A program attempted to divide a number by zero"
- General Protection Fault → "CPU protection violation — illegal instruction or access"
- Page Fault → "Invalid memory address was accessed (null pointer?)"

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
- Color byte `0x0A` = green on black, `0x0F` = white on black, `0x4F` = white on red (panic)

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

### Trigger a kernel panic

```
mysh> panic      # manual panic with 0xDEADBEEF error code
mysh> divzero    # real CPU division by zero exception
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
- **IDT (Interrupt Descriptor Table)** — mapping CPU exception numbers to handler functions
- **ISR stubs in Assembly** — saving CPU state with `pusha` before calling C handlers
- **CPU register dump** — reading register values at crash time via stack pointer
- Writing freestanding C — no `malloc`, no `printf`, no OS beneath you
- Cross-compiling for a different architecture and binary format on macOS
- GNU `make`, linker scripts, and ELF binary structure
- Debugging bare-metal code using QEMU

---

## Roadmap

- [x] Multiboot bootloader
- [x] VGA text rendering
- [x] PS/2 keyboard driver (polling)
- [x] Bump memory allocator (kmalloc/kfree)
- [x] Interactive kernel shell
- [x] Kernel panic handler with CPU register dump and per-exception descriptions
- [ ] Cooperative multitasking — multiple tasks sharing the CPU
- [ ] Interrupt-driven keyboard (replace polling with IDT + IRQ1)
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
