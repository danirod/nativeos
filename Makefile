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

# Check that we have the required software.
ifeq (, $(shell which $(CC)))
    $(error "$(CC) not found. Is the toolchain compiler enabled?")
endif
ifeq (, $(shell which $(LD)))
    $(error "$(LD) not found. Is the toolchain compiler enabled?")
endif
ifeq (, $(shell which $(AS)))
    $(error "$(AS) not found. Have you installed NASM?")
endif

# Directories
KERNEL_PATH = kernel
INCLUDE_PATH = include
BUILD_PATH = out

# Tool flags
CFLAGS = -nostdlib --freestanding -fno-builtin -g -I $(INCLUDE_PATH)/
ASFLAGS = -f elf
LDFLAGS = -nostdlib
QEMUARGS = -serial stdio
QEMU_DEBUGARGS = -s -S

# Compilation units that don't depend on system architecture.
S_BASE_SOURCES = $(shell find $(KERNEL_PATH) -not -path '$(KERNEL_PATH)/arch*' -name '*.s')
C_BASE_SOURCES = $(shell find $(KERNEL_PATH) -not -path '$(KERNEL_PATH)/arch*' -name '*.c')

# Compilation units that depend on the current system architecture.
S_ARCH_SOURCES = $(shell find $(KERNEL_PATH) -path '$(KERNEL_PATH)/arch/$(ARCH)/*' -name '*.s')
C_ARCH_SOURCES = $(shell find $(KERNEL_PATH) -path '$(KERNEL_PATH)/arch/$(ARCH)/*' -name '*.c')

# All compilation units. Note S_BASE_SOURCES has priority. This is because
# we need the bootloader to be early in the compilation list in order to
# properly link the binary.
S_SOURCES = $(S_BASE_SOURCES) $(S_ARCH_SOURCES)
C_SOURCES = $(C_BASE_SOURCES) $(C_ARCH_SOURCES)
S_OBJECTS = $(patsubst %.s, $(BUILD_PATH)/%.o ,$(S_SOURCES))
C_OBJECTS = $(patsubst %.c, $(BUILD_PATH)/%.o, $(C_SOURCES))
KERNEL_OBJS = $(S_OBJECTS) $(C_OBJECTS)

KERNEL_LD = $(KERNEL_PATH)/arch/$(ARCH)/linker.ld
KERNEL_IMAGE = $(BUILD_PATH)/nativeos.elf

# These variables are used when building the distribution disk.
NATIVE_DISK = $(BUILD_PATH)/nativeos.iso
NATIVE_DISK_KERNEL = nativeos.exe

# It might not work on some platforms unless this is done.
GRUB_ROOT = $(shell dirname `which grub-mkrescue`)/..

# Mark some targets as phony. Otherwise they might not always work.
.PHONY: qemu qemu-gdb clean

# Main build targets.
kernel: $(KERNEL_IMAGE)
cdrom: $(NATIVE_DISK)

# Common build targets.
$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_PATH)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Kernel ELF binary image.
$(KERNEL_IMAGE): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -T $(KERNEL_LD) -o $@ $^

# Builds CD-ROM.
$(NATIVE_DISK): $(KERNEL_IMAGE)
	rm -rf $(BUILD_PATH)/cdrom
	cp -R tools/cdrom $(BUILD_PATH)
	cp $(KERNEL_IMAGE) $(BUILD_PATH)/cdrom/boot/$(NATIVE_DISK_KERNEL)
	grub-mkrescue -d $(GRUB_ROOT)/lib/grub/i386-pc -o $@ $(BUILD_PATH)/cdrom

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
	rm -rf $(BUILD_PATH)/cdrom
	rm -f $(KERNEL_IMAGE) $(NATIVE_DISK) $(KERNEL_OBJS)
