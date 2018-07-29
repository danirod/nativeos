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

# Build flags
ARCH = i386
GIT_VERSION = $(shell git describe --always)
DEBUG ?= 0

# Tools.
CC = i386-elf-gcc
LD = i386-elf-gcc
AS = i386-elf-gcc
QEMU = qemu-system-i386

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

# Directories
KERNEL_PATH = kernel
INCLUDE_PATH = include
KEXT_PATH = ext
BUILD_PATH = out

# Tool flags
CFLAGS = -nostdlib --freestanding -fno-builtin -g -I$(INCLUDE_PATH)/
CFLAGS += -Iarch/$(ARCH)/$(INCLUDE_PATH)/
CFLAGS += -Ilibc/include/
CFLAGS += -D_NTOS_VERSION_="\"$(GIT_VERSION)\""
ASFLAGS = -g
LDFLAGS = -nostdlib
QEMUARGS = -serial stdio
QEMU_DEBUGARGS = -s -S

ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0 -DHAVE_DEBUG
endif

# Compilation units that don't depend on system architecture.
S_BASE_SOURCES = $(shell find $(KERNEL_PATH) -name '*.S')
C_BASE_SOURCES = $(shell find $(KERNEL_PATH) -name '*.c')

# Compilation units that depend on the current system architecture.
S_ARCH_SOURCES = $(shell find 'arch/$(ARCH)/$(KERNEL_PATH)' -name '*.S')
C_ARCH_SOURCES = $(shell find 'arch/$(ARCH)/$(KERNEL_PATH)' -name '*.c')

# The klibc is also required to compile the kernel.
KLIBC_LIBRARY = libc/libc.a
KLIBC_SOURCES = $(shell find 'libc/src/' -name '*.c')

# All compilation units. Note S_BASE_SOURCES has priority. This is because
# we need the bootloader to be early in the compilation list in order to
# properly link the binary.
S_SOURCES = $(S_BASE_SOURCES) $(S_ARCH_SOURCES)
C_SOURCES = $(C_BASE_SOURCES) $(C_ARCH_SOURCES)
S_OBJECTS = $(patsubst %.s, $(BUILD_PATH)/%.o ,$(S_SOURCES))
C_OBJECTS = $(patsubst %.c, $(BUILD_PATH)/%.o, $(C_SOURCES))
KERNEL_OBJS = $(S_OBJECTS) $(C_OBJECTS) $(KLIBC_LIBRARY)

KERNEL_LD = arch/$(ARCH)/kernel/linker.ld
KERNEL_IMAGE = $(BUILD_PATH)/nativeos.elf

KEXT_MODS = $(shell find $(KEXT_PATH) -type d -mindepth 1 -maxdepth 1 -exec basename {} \;)
KEXT_OBJS = $(patsubst %, $(BUILD_PATH)/$(KEXT_PATH)/%.kxt, $(KEXT_MODS))
KEXT_LD = arch/$(ARCH)/ext/linker.ld

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

$(BUILD_PATH)/ext/%.kxt: ext/%/*.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -r -T $(KEXT_LD) -o $@ $^

# Kernel ELF binary image.
$(KERNEL_IMAGE): $(KERNEL_OBJS) $(KEXT_OBJS)
	$(LD) $(LDFLAGS) -T $(KERNEL_LD) -o $@ $^

# Build the static library.
$(KLIBC_LIBRARY): $(KLIBC_SOURCES)
	make -C libc

# Builds CD-ROM.
$(NATIVE_DISK): $(KERNEL_IMAGE)
	rm -rf $(BUILD_PATH)/cdrom
	cp -R tools/cdrom $(BUILD_PATH)
	mkdir -p $(BUILD_PATH)/cdrom/nativeos
	cp $(KERNEL_IMAGE) $(BUILD_PATH)/cdrom/boot/$(NATIVE_DISK_KERNEL)
	cp $(BUILD_PATH)/ext/*.kxt $(BUILD_PATH)/cdrom/nativeos
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
	rm -rvf $(BUILD_PATH)
	rm -vf $(NATIVE_DISK) $(KERNEL_IMAGE)
	make -C libc clean
