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
 * \file symtab.c
 * \brief Symbol table
 *
 * The symbol table is a required data structure by the kernel in order to
 * build an index of the different public functions available by the kernel and
 * other kernel extension modules and their respective memory address.
 *
 * The reason behind this is that new modules need to be relocated during load
 * phase, so it's required to know the exact memory address of each public
 * function a kernel module may use, in order to set the proper memory address
 * during this phase.
 *
 * TODO: Hash tables would make this faster than linked lists.  I don't want to
 * prematurely optimize, so I'll wait until I see this implementation being
 * slow or not fast enough.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <elf/symtab.h>

struct symtab_entry;

/**
 * \brief Symbol table linked list entry
 *
 * The kernel requires to know the name and the memory address of exported
 * symbols, both for the kernel image itself, and for other modules loaded into
 * the system.  External modules need to be relocated, and the location of each
 * kernel symbol (such as functions or globals), need to be known in order to
 * relocate additional loaded modules.
 */
struct symtab_entry {
	/** Name of the symbol table entry symbol. */
	char * name;

	/** Memory location of the given symbol table entry. */
	uintptr_t addr;

	/** Next pointer in the symbol table list. */
	struct symtab_entry * next;
};

/** Linked list of exported symbols. */
static struct {
	/** First item in the linked list of symbols. */
	struct symtab_entry * root;

	/** Last item added to the linked list of symbols. */
	struct symtab_entry * last;
} symtab_list;

void
symtab_new_entry (const char * name, uintptr_t addr)
{
	struct symtab_entry * symbol;
	char * sym_name;

	/* Register symbol.  */
	symbol = malloc(sizeof (struct symtab_entry));
	sym_name = malloc(strlen(name) + 1);
	strcpy(sym_name, name);
	symbol->name = sym_name;
	symbol->addr = addr;
	symbol->next = 0;

	/* Connect symbol.  */
	if (symtab_list.root) {
		symtab_list.last->next = symbol;
		symtab_list.last = symbol;
	} else {
		symtab_list.root = symbol;
		symtab_list.last = symbol;
	}
}

void
symtab_del_entry (const char * name)
{
	struct symtab_entry * entry, * prev = 0, * next;

	for (entry = symtab_list.root; entry; entry = entry->next) {
		/* Skip if this item is not the one we want to remove.  */
		if (strcmp(name, entry->name)) {
			prev = entry;
			continue;
		}

		/* Proceed to remove this item from the chain. */
		next = entry->next;
		entry->next = 0;

		if (prev) {
			/* It's not the first item.  */
			prev->next = next;
		}
		else {
			/* It's the first item.  */
			symtab_list.root = next;
		}
		if (symtab_list.last == entry) {
			/* It's also the last item.  */
			symtab_list.last = prev;
		}
		return;
	}
}

uintptr_t
symtab_get_addr (const char * name)
{
	struct symtab_entry * entry;

	for (entry = symtab_list.root; entry; entry = entry->next) {
		if (strcmp(name, entry->name)) {
			continue;
		}
		return entry->addr;
	}
	return 0;
}

void
symtab_load_elf_symbols (struct elf32_section_header * sym_hdr,
		struct elf32_section_header * str_hdr)
{
	unsigned int i, sym_count;
	struct elf32_symt_entry * symbol;

	sym_count = sym_hdr->size / sizeof (struct elf32_symt_entry);
	for (i = 0; i < sym_count; i++) {
		symbol = (struct elf32_symt_entry *) sym_hdr->
	}
}
