# This file is part of NativeOS
# Copyright (C) 2015-2018 The NativeOS contributors
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

ARCH ?= i386

# QEMU flags
GRUB_MKRESCUE = i386-elf-grub-mkrescue
QEMU = qemu-system-i386

# Tools.
CC = i386-elf-gcc
LD = i386-elf-gcc
AS = i386-elf-gcc

# Check that we have the required software.
ifeq (, $(shell which $(CC)))
    $(error "$(CC) not found. Is the toolchain compiler enabled?")
endif
ifeq (, $(shell which $(LD)))
    $(error "$(LD) not found. Is the toolchain compiler enabled?")
endif
ifeq (, $(shell which $(AS)))
    $(error "$(AS) not found. Is the toolchain compiler enabled?")
endif

QEMUARGS = -serial stdio
QEMU_DEBUGARGS = -s -S

# Global assets
KLIBC_LIBRARY = libc/libc.a
KARCH_LIBRARY = arch/$(ARCH)/lib$(ARCH).a

# Kernel compilation
KERNEL_IMAGE = nativeos.elf
LDSCRIPT = arch/$(ARCH)/kernel/linker.ld
LDFLAGS = -nostdlib

# It might not work on some platforms unless this is done.
GRUB_ROOT = $(shell dirname `which $(GRUB_MKRESCUE)`)/..

# CD-ROM
NATIVE_DISK = nativeos.iso

# Mark some targets as phony. Otherwise they might not always work.
.PHONY: qemu qemu-gdb clean

# Main build targets.
cdrom: $(NATIVE_DISK)

$(KERNEL_IMAGE): $(KARCH_LIBRARY) $(KLIBC_LIBRARY)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $@ $^

$(KLIBC_LIBRARY):
	make -C libc
$(KARCH_LIBRARY):
	make -C arch/$(ARCH)

# Builds CD-ROM.
$(NATIVE_DISK): $(KERNEL_IMAGE)
	rm -rf build
	cp -R tools/cdrom build
	cp $(KERNEL_IMAGE) build/boot/nativeos.exe
	$(GRUB_MKRESCUE) -d $(GRUB_ROOT)/lib/grub/i386-pc -o $@ build

# Create an ISO file and run it through QEMU.
qemu: $(NATIVE_DISK)
	$(QEMU) -cdrom $^ $(QEMUARGS)

# Special variant of QEMU made for debugging the kernel image.
qemu-gdb: $(NATIVE_DISK)
	$(QEMU) -cdrom $^ $(QEMUARGS) $(QEMU_DEBUGARGS)

################################################################################
# MISC RULES
################################################################################
# Clean the distribution and remove any generated file.
clean:
	rm -rf build $(KERNEL_IMAGE) $(NATIVE_DISK)
	make -C arch/$(ARCH) clean
	make -C libc clean
