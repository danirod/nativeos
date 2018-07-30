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
 * @file
 * @brief NativeOS main initialization code
 *
 * If you are reading this because you want to understand what happens when
 * NativeOS starts -- this is not the actual entrypoint. The main entrypoint
 * is the Bootstrap function located in boot.s. It contains some initialization
 * routines that can only be done in Assembly such as setting up the stack
 * before calling kmain. However, virtually you can consider the kmain function
 * the main entrypoint for NativeOS.
 */

#include <elf/elf.h>
#include <elf/symtab.h>
#include <module.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>
#include <kernel/mem/heap.h>

#define LOG(msg)

/**
 * \brief Parse the kernel image ELF header looking for public symbols.
 *
 * When a multiboot bootloader such as GRUB loads the kernel image, one of the
 * fields contains information about the ELF image itself, including pointers
 * to the different section headers included in the ELF image.
 *
 * This function will parse the section headers looking for the symbol table
 * and the string table.  Using this headers, it is possible to build an index
 * of global function symbols, including names and memory addresses. This index
 * is added to the kernel symbol table.
 *
 * After calling this function, the kernel symbol table will have information
 * about the public functions available in the kernel, such as kmain.  These
 * functions are required to load kernel extensions, since kernel extensions
 * are relocatable ELF objects that use functions part of the kernel, but uses
 * placeholders as memory addresses because the ELF object does not know the
 * exact location of the function in the kernel image at compile time.
 *
 * \param elf a pointer to the ELF table index provided by Multiboot.
 */
static void
read_kernel_symbol_table (struct multiboot_elf * elf)
{
	struct elf32_section_header * entry, * symtab = 0, * strtab = 0;
	unsigned int i, addr;

	for (i = 0; i < elf->size; i++) {
		addr = elf->addr + (i * elf->size);
		entry = (struct elf32_section_header *) addr;
		if (!strtab && entry->type == ELF_SHT_STRTAB) {
			strtab = entry;
		}
		if (!symtab && entry->type == ELF_SHT_SYMTAB) {
			symtab = entry;
		}
		if (symtab && strtab) {
			break;
		}
	}

	symtab_load_elf_symtab(symtab, strtab);
}

static void
load_kernel_module (struct multiboot_module * mod)
{
	struct kext_module kext;
	kext.addr = mod->mod_start;
	kext.size = mod->mod_end - mod->mod_start;
	kext_process(&kext);
}

static void
load_kernel_modules (struct multiboot_info * mboot)
{
	struct multiboot_module * base, mod;
	int i;

	base = (struct multiboot_module *) mboot->mods_addr;
	for (i = 0; i < mboot->mods_count; i++) {
		load_kernel_module(base + i);
	}
}

/**
 * @brief Main routine for the NativeOS Kernel.
 *
 * This function can be considered the main entrypoint after the bootstrap
 * function in boot.s extracts the multiboot data from the processor registers
 * and places them onto the stack in order to provide them as arguments.
 * This kernel will assume that it has been loaded by a multiboot compatible
 * bootloader such as GRUB. There is some checking to assert that but little
 * effort is made.
 *
 * According to section 3.2 of the Multiboot 0.6.96 specification, available
 * at https://www.gnu.org/software/grub/manual/multiboot/multiboot.html,
 * the magic number value must equal 0x2BADB002. The structure will contain
 * values that are set by the bootloader as described on section 3.3 of the
 * specification.
 *
 * @param magic_number the magic number provided by the bootloader.
 * @param multiboot_ptr a pointer to a multiboot structure.
 */
void kmain(multiboot_info_t *multiboot_ptr)
{
	// Init hardware.
	gdt_init();
	idt_init();
	heap_init();
	frames_init(multiboot_ptr);
	read_kernel_symbol_table(&multiboot_ptr->aout_elf.elf);
	load_kernel_modules(multiboot_ptr);
	for(;;);
}
