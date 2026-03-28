BUILD_DIR = ./build
SRC_DIR = ./src
AS = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

IMAGE = $(BUILD_DIR)/kernel.elf

all: $(IMAGE)

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.asm
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(SRC_DIR)/keyboard.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/idt.o: $(SRC_DIR)/idt.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/isr.o: $(SRC_DIR)/isr.asm
	$(AS) -f elf32 $< -o $@

$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/isr.o
	$(LD) -m elf_i386 -T $(SRC_DIR)/linker.ld -o $@ $^

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf
