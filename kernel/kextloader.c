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

/**
 * \brief Calculate an absolute offset of a relocatable offset.
 */
static inline uintptr_t
kext_absptr (struct elf32_header * elf, uintptr_t offset)
{
	return (uintptr_t) elf + offset;
}

/**
 * \brief Resolve and return a section header entry from a relocatable ELF.
 *
 * ELF images keep offsets relative to the start of the ELF file. If the ELF
 * image has been physically loaded into a memory address, it will be required
 * to compute the proper memory location of the data to avoid accessing
 * rubbish.
 *
 * This function will already perform offset correcting in order to return the
 * actual physical memory address of a section header part of the given ELF
 * image. This function won't bother checking whether the ID is valid, for
 * performance reasons. Such check should be made beforehand.
 *
 * \param elf the ELF image containing the section header to retrieve.
 * \param id the number of segment to retrieve from the relocatable ELF.
 * \return pointer to the requested section header.
 */
static inline struct elf32_section_header *
kext_rel_abs_shdr (struct elf32_header * elf, unsigned int id)
{
	register struct elf32_section_header * headers;
	headers = (struct elf32_section_header *) kext_absptr(elf, elf->shoff);
	return &headers[id];
}

/**
 * \brief Resolve and return a symbol table entry from a relocatable ELF.
 *
 * ELF images keep offsets relative to the start of the ELF file. If the ELF
 * file has been physically loaded into a memory address, it will be required
 * to compute the proper memory location of the data to avoid accessing
 * rubbish.
 *
 * This function will already perform offset correcting in order to return the
 * actual physical memory address of a symbol table entry part of a symbol tab
 * (that we assume is already valid), part of the given ELF image. This
 * function won't bother checking whether the ID is valid, for performance
 * reasons. Such check should be made beforehand.
 *
 * \param elf the ELF image containing the section header to retrieve.
 * \param symtab the symbol table where the symbol to retrieve is contained.
 * \param id the number of symbol table entry to return from the symbol table.
 * \return pointer to the requested symbol table entry.
 */
static inline struct elf32_symt_entry *
kext_rel_abs_symt (struct elf32_header * elf,
		struct elf32_section_header * symtab,
		unsigned int id)
{
	register struct elf32_symt_entry * entries;
	entries = (struct elf32_symt_entry *) kext_absptr(elf, symtab->offset);
	return &entries[id];
}

/**
 * \brief Load the symbols declared in a symbol table into the kernel table.
 *
 * This commit will parse the given symbol table and add global functions and
 * symbols declared in this symbol table into the kernel symbol table.  The
 * string table is required because symbol tables have pointers to strings
 * located in the string table for getting the name of the symbol itself.  See
 * the ELF spec.
 *
 * \param elf the ELF image that has the symbols to load.
 * \param symtab the symbol table having the symbols to load.
 * \param strtab the string table having the symbol names.
 */
static void
kext_rel_load_symtab (struct elf32_header * elf,
		struct elf32_section_header * symtab,
		struct elf32_section_header * strtab)
{
	unsigned int i, symbols;
	unsigned char type;
	char * strings, * sym_name;
	uintptr_t sect_addr, sym_addr;
	struct elf32_section_header * sym_section;
	struct elf32_symt_entry * entry;

	/* This string table will be used to extract symbol names.  */
	strings = (char *) kext_absptr(elf, strtab->offset);
	symbols = elf_count_symtab_symbols(symtab);

	/* symtab->info points to the first global symbol in this table.  */
	for (i = symtab->info; i < symbols; i++) {
		entry = kext_rel_abs_symt(elf, symtab, i);
		type = entry->info & 0xF;

		if (type != ELF_STT_FUNC && type != ELF_STT_OBJECT) {
			/* This is not a symbol of a type we care about.  */
			continue;
		}

		/* Get the section table where this symbol is used.  Example:
		 * the section where the instructions for the function here
		 * defined is defined.  */
		sym_section = kext_rel_abs_shdr(elf, entry->shndx);

		/* Extract the data for this symbol.  */
		sym_name = &strings[entry->name];
		sect_addr = kext_absptr(elf, sym_section->offset);
		sym_addr = sect_addr + (uintptr_t) entry->value;

		/* Put the symbol in the kernel symbol table.  */
		symtab_new_entry(sym_name, sym_addr);
	}
}

/**
 * \brief Load symbols declared in symbol tables present in an ELF image.
 *
 * This function will iterate over the symbol tables declared in the given ELF
 * image, and add the global functions and objects declared in each of these
 * symbol tables.
 *
 * Although some versions of the ELF standard declare that ELF images may have
 * up to one section of each given type, I don't know neither whether the
 * version I got from the ELF standard is outdated, nor if that day will ever
 * come.
 *
 * After calling this function, any global function or object declared in one
 * of the symbol tables of the given ELF image will be available to other
 * kernel extensions or to the kernel itself by using symbol table symbols such
 * as symtab_get_addr.
 *
 * \param elf the ELF image containing symbols to load into the table.
 */
static void
kext_rel_load_symbols (struct elf32_header * elf)
{
	unsigned int i;
	struct elf32_section_header * header, * strtab;

	for (i = 0; i < elf->shnum; i++) {
		header = kext_rel_abs_shdr(elf, i);
		if (header->type == ELF_SHT_SYMTAB) {
			/* shdr->link points to the strtab for symbols.  */
			strtab = kext_rel_abs_shdr(elf, header->link);
			kext_rel_load_symtab(elf, header, strtab);
		}
	}
}

/**
 * \brief Process a relocatable kernel extension.
 *
 * Among other things, this function will update the virtual address of this
 * relocatable image in order to match the actual memory address where the
 * kernel address is placed in memory.
 */
static int
kext_process_rel (struct elf32_header * elf)
{
	kext_rel_load_symbols(elf);
	return 0;
}

int
kext_process (struct kext_module * kext)
{
	struct elf32_header * header;

	header = (struct elf32_header *) kext->addr;
	if (!elf_is_valid(header)) {
		return 0;
	}

	switch (header->type) {
		case ELF_ET_REL:
			return kext_process_rel(header);
		default:
			return 0; /* Unsupported type.  */
	}
}
