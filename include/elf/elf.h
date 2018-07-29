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
 * \file elf.h
 * \brief ELF data structures
 *
 * NativeOS uses the ELF binary format. This requires the kernel having a
 * parser for ELF data structures in order to load additional kernel
 * extensions, which are ELF objects as well.
 */
#pragma once

typedef unsigned int * elf32_addr;
typedef unsigned short int elf32_hword;
typedef unsigned int elf32_word;
typedef int elf32_sword;
typedef unsigned int elf32_offt;

#define ELF_IDENT_SIZE 16

struct elf32_header {
	unsigned char ident[ELF_IDENT_SIZE];
	elf32_hword type;
	elf32_hword machine;
	elf32_word version;
	elf32_addr entry;
	elf32_offt phoff;
	elf32_offt shoff;
	elf32_word flags;
	elf32_hword ehsize;
	elf32_hword phentsize;
	elf32_hword phnum;
	elf32_hword shentsize;
	elf32_hword shnum;
	elf32_hword shstrndx;
};

/**
 * \brief Tests whether NativeOS is able to understand the given ELF header.
 * \param header a pointer to something that may look like an ELF header.
 * \return 0 if the header does not represent a valid ELF image, 1 otherwise.
 */
int elf_is_valid (struct elf32_header * header);
