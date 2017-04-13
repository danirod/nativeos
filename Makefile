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

# Build flags
ARCH = x86

# Tools.
CC = i386-elf-gcc
LD = i386-elf-gcc
AS = nasm
QEMU = qemu-system-i386

ifneq (, $(wildcard tools/toolchain/.))
    # tools/toolchain is enabled. Use it as compiler.
    CC = tools/toolchain/bin/i386-elf-gcc
    LD = tools/toolchain/bin/i386-elf-gcc
endif

# Check that we have the required software.
ifeq (, $(shell which $(CC)))
    $(error "No $(CC) compiler in PATH. Is the toolchain compiler enabled?")
endif
ifeq (, $(shell which $(AS)))
    $(error "No $(AS) compiler in PATH. Have you already installed NASM?")
endif
ifeq (, $(shell which $(LD)))
    $(error "No $(LD) compiler in PATH. Is the toolchain compiler enabled?")
endif

# Tool flags
CFLAGS = -nostdlib --freestanding -fno-builtin -g -Iinclude/
ASFLAGS = -f elf
LDFLAGS = -nostdlib
QEMUARGS = -serial stdio
QEMU_DEBUGARGS = -s -S

# Compilation units that don't depend on system architecture.
S_BASE_SOURCES = $(shell find kernel -not -path 'kernel/arch*' -name '*.s')
C_BASE_SOURCES = $(shell find kernel -not -path 'kernel/arch*' -name '*.c')

# Compilation units that depend on the current system architecture.
S_ARCH_SOURCES = $(shell find kernel -path 'kernel/arch/$(ARCH)/*' -name '*.s')
C_ARCH_SOURCES = $(shell find kernel -path 'kernel/arch/$(ARCH)/*' -name '*.c')

# All compilation units. Note S_BASE_SOURCES has priority. This is because
# we need the bootloader to be early in the compilation list in order to
# properly link the binary.
S_SOURCES = $(S_BASE_SOURCES) $(S_ARCH_SOURCES)
C_SOURCES = $(C_BASE_SOURCES) $(C_ARCH_SOURCES)
S_OBJECTS = $(patsubst %.s,%.o,$(S_SOURCES))
C_OBJECTS = $(patsubst %.c,%.o,$(C_SOURCES))
KERNEL_OBJS = $(S_OBJECTS) $(C_OBJECTS)
KERNEL_LD = kernel/arch/$(ARCH)/linker.ld

# It might not work on some platforms unless this is done.
GRUB_ROOT = $(shell dirname `which grub-mkrescue`)/..

# Mark some targets as phony. Otherwise they might not always work.
.PHONY: qemu qemu-gdb clean

# Main build targets.
kernel: nativeos.elf
cdrom: nativeos.iso

# Common build targets.
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

# Kernel ELF binary image.
nativeos.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -T $(KERNEL_LD) -o $@ $^

# Builds CD-ROM.
nativeos.iso: nativeos.elf
	rm -rf cdrom
	cp -R tools/cdrom .
	cp nativeos.elf cdrom/boot/nativeos.exe
	grub-mkrescue -d $(GRUB_ROOT)/lib/grub/i386-pc -o $@ cdrom

# Create an ISO file and run it through QEMU.
qemu: nativeos.iso
	$(QEMU) -cdrom $^ $(QEMUARGS)

# Special variant of QEMU made for debugging the kernel image.
qemu-gdb: nativeos.iso
	$(QEMU) -cdrom $^ $(QEMUARGS) $(QEMU_DEBUGARGS)

################################################################################
# MISC RULES
################################################################################
# Clean the distribution and remove any generated file.
clean:
	rm -rf cdrom
	rm -f nativeos.elf $(KERNEL_OBJS) nativeos.iso
