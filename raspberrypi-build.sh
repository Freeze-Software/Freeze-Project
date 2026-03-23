CC = aarch64-none-elf-gcc
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

P = FreezeProject
BUILDDIR = $(P)/build
SRCDIR = $(P)/src

CFLAGS = -ffreestanding -nostdlib -nostartfiles -Wall -Wextra -I$(P)/include
LDFLAGS = -T $(SRCDIR)/linker.ld

C_SOURCES = $(wildcard $(SRCDIR)/*.c)
ASM_SOURCES = $(wildcard $(SRCDIR)/*.S)

C_OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRCDIR)/%.S,$(BUILDDIR)/%.o,$(ASM_SOURCES))

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

KERNEL = kernel8.img

all: $(KERNEL)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.S
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJECTS)
	$(OBJCOPY) $(BUILDDIR)/kernel.elf -O binary $(KERNEL)

clean:
	rm -rf $(BUILDDIR) *.img *.elf

.PHONY: all clean
