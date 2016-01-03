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
	Kernel/driver/keyboard.o \
	Kernel/driver/timer.o \
	Kernel/driver/vga.o

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

