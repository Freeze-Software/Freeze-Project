# Variables
export PROJECT_ROOT	:= $(realpath .)
BUILD_DIR			:= build
ISO_DIR				:= $(BUILD_DIR)/iso
ISO_FILE			:= $(ISO_DIR)/freeze.img
BOOT_BIN			:= $(BUILD_DIR)/boot/boot.bin
BOOT_ELF			:= $(BUILD_DIR)/boot/boot.elf
KERNEL_ELF			:= $(BUILD_DIR)/kernel/kernel.elf
LOADER_BIN			:= $(BUILD_DIR)/kernel/loader.bin
KERNEL_BIN			:= $(BUILD_DIR)/kernel/kernel.bin
MOUNT_POINT			:= /mnt

# Rules
.PHONY : all clean run debug bootloader kernel
all : clean $(ISO_FILE) run

# Make the final iso file
$(ISO_FILE): kernel
	@mkdir -p $(@D)
	dd if=/dev/zero of=$@ bs=1M count=10
	mkfs.fat -F 16 $@
	dd if=$(BOOT_BIN) of=$@ conv=notrunc

	sudo mount -o loop $@ $(MOUNT_POINT)
	sudo cp $(LOADER_BIN) $(MOUNT_POINT)
	sudo cp $(KERNEL_BIN) $(MOUNT_POINT)
	sudo umount $(MOUNT_POINT)

# Make boot & kernel
kernel:
	$(MAKE) -C boot
	$(MAKE) -C kernel

# Make slibc library
lib:
	$(MAKE) -C kernel/libs/slibc

run : 
	qemu-system-i386 -drive format=raw,file=$(ISO_FILE)

# Cleanup all build files
clean:
	rm -rf $(BUILD_DIR)/*
	clear

# Debug section
debug: clean $(ISO_FILE)
	qemu-system-i386 -s -S -kernel $(ISO_FILE)

gdb-only:
	gdb -ex "target remote localhost:1234" -ex "layout asm" -ex "br *0x7c00" -ex "c"

gdb-boot:
	gdb -ex "file $(BOOT_ELF)" -ex "target remote localhost:1234" -ex "layout asm"

gdb-kernel:
	gdb -ex "file $(KERNEL_ELF)" -ex "target remote localhost:1234" -ex "layout asm"

run: build/iso/freeze.img
	qemu-system-i386 -cdrom build/iso/freeze.img -boot d -m 512M
