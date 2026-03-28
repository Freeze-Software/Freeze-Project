AS = gcc
CC = gcc
LD = ld

P = FreezeProject
BUILDDIR = $(P)/build
SRCDIR = $(P)/src

CFLAGS = -ffreestanding -m32 -Wall -Wextra -I$(P)/include -I$(P)/src
LDFLAGS = -m elf_i386

C_SOURCES = $(wildcard $(SRCDIR)/*.c)
ASM_SOURCES = $(wildcard $(SRCDIR)/*.S)

C_OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRCDIR)/%.S,$(BUILDDIR)/%.o,$(ASM_SOURCES))

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

all: run

# compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.S
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# kernel
$(BUILDDIR)/kernel.bin: $(OBJECTS) $(SRCDIR)/linker.ld
	$(LD) $(LDFLAGS) -T $(SRCDIR)/linker.ld -o $@ $(OBJECTS)

# grub
iso/boot/grub/grub.cfg: $(P)/grub/grub.cfg
	mkdir -p iso/boot/grub
	cp $< $@

# build iso
freeze.iso: $(BUILDDIR)/kernel.bin iso/boot/grub/grub.cfg
	rm -rf iso/boot/kernel.bin
	mkdir -p iso/boot/grub
	cp $(BUILDDIR)/kernel.bin iso/boot/
	grub-mkrescue -o $@ iso

# disk image for persistent file system
freeze.img:
	@if [ ! -f freeze.img ]; then \
		dd if=/dev/zero of=freeze.img bs=1M count=10 2>/dev/null || true; \
	fi

# QEMU
run: freeze.iso freeze.img
	qemu-system-i386 -cdrom freeze.iso -drive file=freeze.img,format=raw,media=disk -nographic

# clean
clean:
	rm -rf $(BUILDDIR) freeze.iso iso

.PHONY: all clean run
