# Makefile

BUILD_DIR = ./build
SRC_DIR = ./src
AS = nasm
CC = gcc
LD = x86_64-elf-ld

BOOT = $(BUILD_DIR)/boot.bin
KERNEL = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/os-image.bin

all: $(IMAGE)

$(BOOT): $(SRC_DIR)/boot.asm
	$(AS) -f bin $< -o $@

$(KERNEL): $(SRC_DIR)/kernel.c
	$(CC) -m32 -ffreestanding -c $< -o $(BUILD_DIR)/kernel.o
	$(LD) -m elf_i386 -Ttext 0x1000 --oformat binary $(BUILD_DIR)/kernel.o -o $@

$(IMAGE): $(BOOT) $(KERNEL)
	cat $(BOOT) $(KERNEL) > $@

clean:
	rm -rf $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o
