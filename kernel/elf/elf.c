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
 * \file elf.c
 * \brief ELF parser
 *
 * NativeOS uses the ELF binary format. This requires the kernel having a
 * parser for ELF data structures in order to load additional kernel
 * extensions, which are ELF objects as well.
 */
#include <elf/elf.h>

/**
 * \brief Tests whether NativeOS is able to understand the given ELF header.
 *
 * There are a few limitations on the support for ELF binaries on NativeOS.
 * First of all, this function will check if the given header is actually an
 * ELF header or some scrambled data.
 *
 * Even though the header may look valid, there may be cases where NativeOS is
 * not able to understand the given executable file.  For instance, NativeOS is
 * currently a 32 bit operationg system, so if the header states the file is a
 * 64 bit executable, this function will bail out.
 *
 * \param header a pointer to something that may look like an ELF header.
 * \return 0 if the header does not represent a valid ELF image, 1 otherwise.
 */
int
elf_is_valid (struct elf32_header * header)
{
	elf32_word magic_number;

	/* Test the ELF header uses a valid magic number.  */
	magic_number = *((elf32_word *) header->ident);
	if (magic_number != 0x464c457f) {
		return 0;
	}

	if (header->ident[4] != 1) {
		/* File is not 32 bit.  */
		return 0;
	}
	if (header->ident[5] != 1) {
		/* File is not little endian, so it won't work on i386.  */
		return 0;
	}
	if (header->type < 1 || header->type > 3) {
		/* Test the type is appropiate.  */
		return 0;
	}
	if (header->machine != 3) {
		/* The ELF file must be marked as "ELF32".  */
		return 0;
	}
	if (header->version != 1) {
		/* Version is not appropiate.  */
		return 0;
	}

	return 1;
}
