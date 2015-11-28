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
	Kernel/io.o \
	Kernel/main.o \
	Kernel/printk.o \
	Kernel/console/vgacon.o

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

# Clean objective
clean:
	rm -f $(KERNEL_IMG) $(KERNEL_OBJS)
