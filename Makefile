BUILD_DIR = ./build
SRC_DIR = ./src
AS = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

BOOT = $(BUILD_DIR)/boot.bin
IMAGE = $(BUILD_DIR)/os-image.bin

all: $(IMAGE)

$(BOOT): $(SRC_DIR)/boot.asm
	$(AS) -f bin $< -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(SRC_DIR)/keyboard.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/idt.o: $(SRC_DIR)/idt.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/isr.o: $(SRC_DIR)/isr.asm
	$(AS) -f elf32 $< -o $@

$(IMAGE): $(BOOT) $(BUILD_DIR)/kernel.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/isr.o
	$(LD) -m elf_i386 -Ttext 0x7e00 --oformat binary $(BUILD_DIR)/kernel.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/isr.o -o $(BUILD_DIR)/kernel.bin
	cat $(BOOT) $(BUILD_DIR)/kernel.bin > $@

clean:
	rm -rf $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o
