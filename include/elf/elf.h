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

#define ELF_ET_NONE 0
#define ELF_ET_REL 1
#define ELF_ET_EXEC 2
#define ELF_ET_DYN 3
#define ELF_ET_CORE 4
#define ELF_ET_LOPROC 0xFF00
#define ELF_ET_HIPROC 0xFFFF

#define ELF_SHT_NULL 0
#define ELF_SHT_PROGBITS 1
#define ELF_SHT_SYMTAB 2
#define ELF_SHT_STRTAB 3
#define ELF_SHT_RELA 4
#define ELF_SHT_HASH 5
#define ELF_SHT_DYNAMIC 6
#define ELF_SHT_NOTE 7
#define ELF_SHT_NOBITS 8
#define ELF_SHT_REL 9
#define ELF_SHT_SHLIB 10
#define ELF_SHT_DYNSYM 11

#define ELF_SHN_UNDEF 0
#define ELF_SHN_LORESERVE 0xFF00
#define ELF_SHN_LOPROC 0xFF00
#define ELF_SHN_HIPROC 0xFF1F
#define ELF_SHN_ABS 0xFFF1
#define ELF_SHN_COMMON 0xFFF2
#define ELF_SHN_HIRESERVE 0xFFFF

#define ELF_STB_LOCAL 0
#define ELF_STB_GLOBAL 1
#define ELF_STB_WEAK 2
#define ELF_STB_LOPROC 13
#define ELF_STB_HIPROC 15

#define ELF_STT_NOTYPE 0
#define ELF_STT_OBJECT 1
#define ELF_STT_FUNC 2
#define ELF_STT_SECTION 3
#define ELF_STT_FILE 4
#define ELF_STT_LOPROC 13
#define ELF_STT_HIPROC 15

#define ELF_R386_NONE 0
#define ELF_R386_32 1
#define ELF_R386_PC32 2
#define ELF_R386_GOT32 3
#define ELF_R386_PLT32 4
#define ELF_R386_COPY 5
#define ELF_R386_GLOB_DAT 6
#define ELF_R386_JMP_SLOT 7
#define ELF_R386_RELATIVE 8
#define ELF_R386_GOTOFF 9
#define ELF_R386_GOTPC 10

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

struct elf32_section_header {
	elf32_word name;
	elf32_word type;
	elf32_word flags;
	elf32_addr addr;
	elf32_offt offset;
	elf32_word size;
	elf32_word link;
	elf32_word info;
	elf32_word addralign;
	elf32_word entsize;
};

struct elf32_symt_entry {
	elf32_word name;
	elf32_addr value;
	elf32_word size;
	unsigned char info;
	unsigned char other;
	elf32_hword shndx;
};

struct elf32_rel {
	elf32_addr offset;
	elf32_word info;
};

struct elf32_rela {
	elf32_addr offset;
	elf32_word info;
	elf32_sword addend;
};

/**
 * \brief Tests whether NativeOS is able to understand the given ELF header.
 * \param header a pointer to something that may look like an ELF header.
 * \return 0 if the header does not represent a valid ELF image, 1 otherwise.
 */
int elf_is_valid (struct elf32_header * header);

/**
 * \brief Returns a pointer to some ELF section header given by its index.
 * \param header a pointer to the ELF object file header.
 * \param index the number of section to return.
 * \return a pointer to the given header or NULL if invalid index.
 */
struct elf32_section_header *
elf_get_section_header (struct elf32_header * header, unsigned int index);

/**
 * \brief Counts the number of symbols present in the given symbol table.
 * \param hdr a header pointing to a ELF section header of type SYMTAB.
 * \return the amount of symbols contained in the given symbol table header.
 */
unsigned int
elf_count_symtab_symbols (struct elf32_section_header * hdr);

/**
 * \brief Get a descriptor to a particular symbol table entry.
 * \param hdr a header to some ELF section header of type SYMTAB.
 * \param i the number of symbol table entry to retrieve.
 * \return the symbol table entry or NULL if no table entry.
 */
struct elf32_symt_entry *
elf_get_symtab_entry (struct elf32_section_header * hdr, unsigned int i);

/**
 * \brief Get a string from the string table given its index.
 * \param hdr a header to some ELF section header of type STRTAB.
 * \param i the index of the string to retrieve.
 * \return the null-terminated string contained in the given index.
 */
char *
elf_get_strtab_entry (struct elf32_section_header * hdr, unsigned int i);
