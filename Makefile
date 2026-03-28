BUILD_DIR = ./build
SRC_DIR = ./src
AS = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

BOOT = $(BUILD_DIR)/boot.bin
KERNEL = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/os-image.bin

all: $(IMAGE)

$(BOOT): $(SRC_DIR)/boot.asm
	$(AS) -f bin $< -o $@

$(KERNEL): $(SRC_DIR)/kernel.c $(SRC_DIR)/keyboard.c
	$(CC) -m32 -ffreestanding -nostdlib -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o
	$(CC) -m32 -ffreestanding -nostdlib -c $(SRC_DIR)/keyboard.c -o $(BUILD_DIR)/keyboard.o
	$(LD) -m elf_i386 -Ttext 0x1000 --oformat binary $(BUILD_DIR)/kernel.o $(BUILD_DIR)/keyboard.o -o $@

$(IMAGE): $(BOOT) $(KERNEL)
	cat $(BOOT) $(KERNEL) > $@

clean:
	rm -rf $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o
