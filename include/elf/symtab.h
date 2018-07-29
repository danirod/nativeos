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
 * \file symtab.h
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
 */
#pragma once

#include <stdint.h>

/**
 * \brief Register a symbol in the symbol tab.
 * \param name the name of this symbol.
 * \param addr the memory address of the symbol.
 */
void symtab_new_entry (const char * name, uintptr_t addr);

/**
 * \brief Remove a symbol from the symbol tab.
 * \param name the name of the symbol.
 */
void symtab_del_entry (const char * name);

/**
 * \brief Get the memory address of a symbol.
 * \param name the name of the symbol whose memory address to return.
 * \return the address of the given symbol or NULL if not found.
 */
uintptr_t symtab_get_addr (const char * name);

/**
 * \brief Load symbols in an ELF symbol table into the symbol table.
 * \param symtab the symbol table of an ELF binary object.
 * \param strtab the string table of an ELF binary object.
 */
void
symtab_load_elf_symtab(struct elf32_section_header * symtab,
		struct elf32_section_header *strtab);
