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
#include <stdlib.h>
#include <string.h>

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
 * \brief Compute the value for a particular symbol in a symbol table.
 *
 * Given the section table and the index of the symbol to get the data from,
 * this function will extract the symbol from the symbol table and get the
 * value for that particular symbol.
 */
static uintptr_t
kext_rel_get_symval (struct elf32_header * elf,
		struct elf32_section_header * symtab,
		unsigned int id)
{
	struct elf32_section_header * strtab, * target;
	struct elf32_symt_entry * entry;
	char * strings, * sym_name;
	uintptr_t sym_offset;

	if (id == 0) return 0; /* First element is always reserved.  */

	/* Get the string table where external symbols will be taken from.  */
	strtab = kext_rel_abs_shdr(elf, symtab->link);
	strings = (char *) kext_absptr(elf, strtab->offset);

	entry = (struct elf32_symt_entry *) kext_rel_abs_symt(elf, symtab, id);

	if (entry->shndx == ELF_SHN_UNDEF) {
		/* The symbol is external. I need the symbol name.  */
		sym_name = &strings[entry->name];
		if (sym_name && *sym_name) {
			return symtab_get_addr(sym_name);
		} else {
			/* TODO: Weak symbols are a thing.  */
			return 0;
		}
	}
	else if (entry->shndx == ELF_SHN_ABS) {
		/* Absolute symbol.  */
		return (uintptr_t) entry->value;
	} else {
		/* Internally defined symbol.  */
		target = kext_rel_abs_shdr(elf, entry->shndx);
		sym_offset = kext_absptr(elf, target->offset);
		return sym_offset + (uintptr_t) entry->value;
	}
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
 * \brief Process a particular relocation section in the ELF image.
 *
 * This function actually parses a relocation table and relocates each symbol
 * described by the relocation table.  A relocation table has some
 * dependencies, and these dependencies are provided to this function as well.
 *
 * The relocation table points to the symbols that need to be relocated and
 * indicates the type of relocation that has to be made.  This is important
 * because there are symbols that have to be relocated in a special fashion.
 *
 * The symbol table points to the actual symbol names and values.  The symbols
 * described in the relocation table are actually indices to entries in the
 * symbol table, so the symbol table is needed to know which symbol actually we
 * are trying to relocate.
 *
 * The target table points to the actual code or data where the relocations
 * will be made.  Usually it's about some values located in particular memory
 * addresses contained here.
 *
 * \param elf the ELF image that we are trying to relocate.
 * \param reltab the relocation entries table.
 * \param symtab the symbol table with symbols to be relocated.
 * \param tgttab the code or data to relocate.
 */
static void
kext_rel_do_relocate (struct elf32_header * elf,
		struct elf32_section_header * reltab,
		struct elf32_section_header * symtab,
		struct elf32_section_header * tgttab)
{
	unsigned int i, count, symidx;
	unsigned char type;
	uintptr_t symval, tgtptr;
	unsigned int * locptr;
	struct elf32_rel * rels, * rel;

	if (reltab->entsize == 0) return;
	count = reltab->size / reltab->entsize;
	rels = (struct elf32_rel *) kext_absptr(elf, reltab->offset);
	tgtptr = kext_absptr(elf, tgttab->offset);

	for (i = 0; i < count; i++) {
		rel = &rels[i];
		type = rel->info & 0xF;
		symidx = rel->info >> 8;

		/* Only ELF_R386_NONE, ELF_R386_32 and ELF_R386_PC32 are
		 * required at the moment for kernel extensions.  So we only
		 * need to know the following values: S, A and P. Where:
		 *
		 * S = the value of the symbol whose value resides in the
		 *     relocation entry. We need to access the symbol table for
		 *     this. If it's a local symbol, we can get the value by
		 *     getting the symbol's value in this very own ELF image.
		 *     Otherwise, it's external, and we must use the kernel
		 *     symbol table (call symtab_get_addr).
		 * A = the addendum. ELF-i386 doesn't use RELA entries so the
		 *     addendum is always implicit and it's the current
		 *     placeholder value of the symbol to be relocated.
		 * P = the place where we will be relocating.  (The memory
		 *     address where the relocation will have place.)
		 *
		 * Yes, there is a relationship between A and P.  P = &A.  Or
		 * either, A = *P.  Depending on the relocation type, the sum
		 * will be different.
		 */
		symval = kext_rel_get_symval(elf, symtab, symidx);
		locptr = (unsigned int *) (tgtptr + (uintptr_t) rel->offset);

		if (type == ELF_R386_NONE) {
			/* No relocation.  */
			return;
		} else if (type == ELF_R386_32) {
			/* S + A.  */
			*locptr = symval + *locptr;
		} else if (type == ELF_R386_PC32) {
			/* S + A - P.  */
			*locptr = symval + *locptr - ((uintptr_t) locptr);
		} else {
			/* TODO: Unsupported relocation error.  */
		}
	}
}

/**
 * \brief Perform relocation based in the relocation tables of the ELF image.
 *
 * This function will look for relocation sections in the ELF image, and for
 * each section found in the ELF image, the contents of such sections will be
 * read and some symbols will be relocated.
 *
 * \param elf the ELF image containing the sections and symbols to relocate.
 */
static void
kext_rel_relocate (struct elf32_header * elf)
{
	unsigned int i;
	struct elf32_section_header * header, * symtab, * tgttab;

	for (i = 0; i < elf->shnum; i++) {
		header = kext_rel_abs_shdr(elf, i);
		/* There are no ELF_SHT_RELA sections on ELF-i386.  */
		if (header->type == ELF_SHT_REL) {
			/* shdr->link points to the symbol table containing the
			 * symbols to relocate.  shdr->info points to the
			 * section containing code or data to relocate.  */
			symtab = kext_rel_abs_shdr(elf, header->link);
			tgttab = kext_rel_abs_shdr(elf, header->info);
			kext_rel_do_relocate(elf, header, symtab, tgttab);
		}
	}
}

/**
 * \brief Allocate NOBITS sections.
 *
 * In order to save space, the ELF specification supports having segments
 * stripped from the binary ELF file.  When the binary is loaded, it is
 * expected that the loader allocates a memory region of the requested size, to
 * use as that particular segment.  These segments are of type NOBITS.
 *
 * One of the most important types of NOBITS segments is BSS, which is the area
 * used by uninitialized global or static variables, or those whose initial
 * state is 0x00.  Instead of having space in the ELF binary full of zeroes,
 * the ELF image just stores the amount of bytes that has this section, and the
 * loader reserves and memsets this section to save space in the ELF image.
 *
 * This function will process the ELF image looking for segments whose type is
 * NOBITS and where the ALLOC flag is set to true, and it will allocate memory
 * regions to use as BSS segments.
 *
 * \param elf the ELF image to allocate.
 */
static void
kext_allocate_nobits (struct elf32_header * elf)
{
	unsigned int i;
	unsigned char * p, * buffer;
	struct elf32_section_header * header;

	for (i = 0; i < elf->shnum; i++) {
		header = elf_get_section_header(elf, i);

		/* Only interested about sections not allocated.  */
		if (header->type != ELF_SHT_NOBITS) continue;
		if ((header->flags & ELF_SHF_ALLOC) == 0) continue;
		if (header->size == 0) continue;

		/* Allocate a memory region for this kernel extension.  */
		buffer = malloc(header->size);
		memset(buffer, 0, header->size);

		/* Update the header to point to this buffer.  */
		header->offset = (uintptr_t) buffer - (uintptr_t) elf;
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
	/* TODO: Should be better to first load all the symbols.  */
	kext_allocate_nobits(elf);
	kext_rel_load_symbols(elf);
	kext_rel_relocate(elf);
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
