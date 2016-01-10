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
CFLAGS = -nostdlib --freestanding -fno-builtin -g -Iinclude/
ASFLAGS = -f elf
LDFLAGS = -nostdlib

# All the objects that are part of the kernel.
KERNEL_OBJS = $(patsubst %.c,%.o,$(wildcard kernel/*.c)) \
        $(patsubst %.s,%.o,$(wildcard kernel/*.s)) \
        $(patsubst %.c,%.o,$(wildcard kernel/**/*.c))

# It might not work on some platforms unless this is done.
GRUB_ROOT = $(shell dirname `which grub-mkrescue`)/..

# Mark some targets as phony. Otherwise they might not always work.
.PHONY: qemu qemu-gdb clean

################################################################################
# ALIASES
################################################################################
kernel: nativeos.elf
	
cdrom: nativeos.iso

################################################################################
# COMMON BUILD TARGETS
################################################################################
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

################################################################################
# KERNEL IMAGE
################################################################################
# Build the kernel image
nativeos.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -T kernel/linker.ld -o $@ $^

################################################################################
# CD-ROM PACKING
################################################################################
# Build the ISO image for NativeOS (requires grub)
nativeos.iso: nativeos.elf
	rm -rf cdrom
	cp -R tools/cdrom .
	cp nativeos.elf cdrom/boot/nativeos.exe
	grub-mkrescue -d $(GRUB_ROOT)/lib/grub/i386-pc -o $@ cdrom

################################################################################
# QEMU TARGETS
################################################################################
# Create an ISO file and run it through QEMU.
qemu: nativeos.iso
	qemu-system-i386 -cdrom nativeos.iso

# Special variant of QEMU made for debugging the kernel image.
qemu-gdb: nativeos.iso
	qemu-system-i386 -cdrom nativeos.iso -s -S

################################################################################
# MISC RULES
################################################################################
# Clean the distribution and remove any generated file.
clean:
	rm -rf cdrom
	rm -f nativeos.elf $(KERNEL_OBJS) nativeos.iso

