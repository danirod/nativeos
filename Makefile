# This file is part of NativeOS
# Copyright (C) 2015-2016 Dani Rodríguez <danirod@outlook.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Tools. The i386-elf toolset is required to build this software
CC = i386-elf-gcc
AS = nasm
LD = i386-elf-gcc

# Tool flags
CFLAGS = -nostdlib --freestanding -fno-builtin -g -IInclude/
ASFLAGS = -f elf
LDFLAGS = -nostdlib -T linker.ld

# Kernel. Add every new unit to KERNEL_OBJS. It works. Ew, tho.
KERNEL_IMG := nativeos.elf
KERNEL_OBJS := Boot/bootstrap.o \
	Kernel/gdt.o \
	Kernel/idt.o \
	Kernel/io.o \
	Kernel/main.o \
	Kernel/memory.o \
	Kernel/lidt.o \
	Kernel/lgdt.o \
	Kernel/panic.o \
	Kernel/printk.o \
	Kernel/driver/keyboard.o \
	Kernel/driver/timer.o \
	Kernel/driver/vga.o \
    Kernel/paging.o

CDROM_ISO = nativeos.iso
GRUB_ROOT = $(shell dirname `which grub-mkrescue`)/..

# Mark a few targets as phony. Otherwise they might not always run.
.PHONY: $(CDROM_ISO) qemu qemu-gdb clean

kernel: $(KERNEL_IMG)
	
cdrom: $(CDROM_ISO)

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

# CD-ROM
$(CDROM_ISO): nativeos.elf
	rm -rf cdrom
	cp -R Tools/cdrom .
	cp nativeos.elf cdrom/boot/nativeos.exe
	grub-mkrescue -d $(GRUB_ROOT)/lib/grub/i386-pc -o $@ cdrom

# Execute the virtual ISO file through QEMU.
qemu: nativeos.iso
	qemu-system-i386 -cdrom nativeos.iso

# Special variant of QEMU designed for debugging sessions.
qemu-gdb: nativeos.iso
	qemu-system-i386 -cdrom nativeos.iso -s -S

# Clean objective
clean:
	rm -rf cdrom
	rm -f $(KERNEL_IMG) $(KERNEL_OBJS) nativeos.iso

