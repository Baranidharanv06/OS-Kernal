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

$(BUILD_DIR)/shell.o: $(SRC_DIR)/shell.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/panic.o: $(SRC_DIR)/panic.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/exceptions.o: $(SRC_DIR)/exceptions.c
	$(CC) -m32 -ffreestanding -nostdlib -c $< -o $@

$(BUILD_DIR)/exceptions_asm.o: $(SRC_DIR)/exceptions.asm
	$(AS) -f elf32 $< -o $@

$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/shell.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/panic.o $(BUILD_DIR)/exceptions.o $(BUILD_DIR)/exceptions_asm.o
	$(LD) -m elf_i386 -T $(SRC_DIR)/linker.ld -o $@ $^

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf
