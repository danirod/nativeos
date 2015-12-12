# NativeOS Makefile

# Tools
CC = gcc
AS = nasm
LD = ld

# Tool flags
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs --freestanding \
	-fno-builtin -Wall -Wextra -g -IInclude/
ASFLAGS = -f elf32
LDFLAGS = -melf_i386 -nostdlib -T linker.ld

# Kernel. Add every new unit to KERNEL_OBJS. It works. Ew, tho.
KERNEL_IMG := nativeos.elf
KERNEL_OBJS := Boot/bootstrap.o \
	Kernel/gdt.o \
	Kernel/idt.o \
	Kernel/io.o \
	Kernel/main.o \
	Kernel/lidt.o \
	Kernel/lgdt.o \
	Kernel/panic.o \
	Kernel/printk.o \
	Kernel/vga.o

# Kernel image distibution
$(KERNEL_IMG): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Object file generation
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<
%.s: %.c
	$(CC) -S $(CFLAGS) -o $@ $<

# Floppies
floppy: $(KERNEL_IMG) Tools/nativeos.img
	cd Tools; ./kerinstall

Tools/nativeos.img:
	cd Tools; ./mkfloppies

qemu: floppy
	qemu-system-i386 -fda Tools/nativeos.img -monitor stdio

grubinstall: Tools/nativeos.img
	qemu-system-i386 -fda Tools/grubdisk.img -fdb Tools/nativeos.img

# Clean objective
clean:
	rm -f $(KERNEL_IMG) $(KERNEL_OBJS) Tools/nativeos.img Tools/grubdisk.img

