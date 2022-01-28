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

.PHONY: build-kernel \
	cdrom \
	check-profile \
	clean \
	qemu \
	qemu-gdb \
	usage

GRUB_MKRESCUE ?= grub-mkrescue
GRUB_ROOT = $(shell dirname `which ${GRUB_MKRESCUE}`)
QEMU = qemu-system-i386

usage:
	@echo "Targets:"
	@echo "  build-kernel     builds a kernel image"
	@echo "  cdrom            builds a CD-ROM image"
	@echo "  qemu             runs the built CD-ROM in QEMU"
	@echo "  qemu-gdb         runs the built CD-ROM in QEMU using GDB"
	@echo
	@echo "All the targets require passing a variable called PROFILE with"
	@echo "the name of a kernel profile first. For instance,"
	@echo
	@echo "    make build-kernel PROFILE=I386"
	@echo

check-profile:
ifeq ($(PROFILE),)
	$(error Please, set the PROFILE variable when calling this target.)
endif
	true

build-kernel: check-profile
	tools/kcons conf/${PROFILE}
	make -C compile/${PROFILE}

dist/ramdisk.tar: check-profile
	tar -C ramdisk -cf dist/ramdisk.tar .

cdrom: check-profile dist/nativeos-${PROFILE}.iso

dist/nativeos-${PROFILE}.iso: compile/${PROFILE}/kernel dist/ramdisk.tar
	rm -rf dist/${PROFILE}
	mkdir -p dist/${PROFILE}
	cp -R tools/cdrom/* dist/${PROFILE}
	cp compile/${PROFILE}/kernel dist/${PROFILE}/boot/nativeos.exe
	cp dist/ramdisk.tar dist/${PROFILE}/ramdisk.tar
	${GRUB_MKRESCUE} -d ${GRUB_ROOT}/../lib/grub/i386-pc -o dist/nativeos-${PROFILE}.iso dist/${PROFILE}


qemu: check-profile dist/nativeos-${PROFILE}.iso
	$(QEMU) -m 16 -cpu 486 -cdrom dist/nativeos-${PROFILE}.iso -serial stdio -display sdl

qemu-gdb: check-profile dist/nativeos-${PROFILE}.iso
	$(QEMU) -m 16 -cpu 486 -cdrom dist/nativeos-${PROFILE}.iso -serial stdio -s -S -display sdl

clean:
	rm -rf compile
