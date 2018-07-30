/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file kextloader.c
 * \brief Kernel Extension Loader
 *
 * Kernel Extensions are relocatable ELF objects present in the boot media
 * where NativeOS boots from that contain additional modules with kernel code.
 * The NativeOS Loader will load this extensions by processing their ELF
 * tables, relocating the extensions into their memory addresses, and importing
 * symbols into the kernel symbol table.
 */
#include <elf/elf.h>
#include <elf/symtab.h>
#include <module.h>

int
kext_process (struct kext_module * kext)
{
	struct elf32_header * header;

	header = (struct elf32_header *) kext->addr;
	if (!elf_is_valid(header)) {
		return 0;
	}

	switch (header->type) {
		default:
			return 0; /* Unsupported type.  */
	}
}
