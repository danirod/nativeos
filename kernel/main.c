/*
 * This file is part of NativeOS: next-gen x86 operating system
<<<<<<< HEAD
 * Copyright (C) 2015-2017 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
=======
 * Copyright (C) 2015-2016 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
>>>>>>> 177c6d5d79d623161e676dde3a36a01dec49af9d
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

#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/paging.h>
#include <driver/com.h>
#include <driver/keyboard.h>
#include <driver/timer.h>
#include <driver/vbe.h>
#include <kernel/kernel.h>
#include <kernel/multiboot.h>

#define LOG(msg) (serial_send_str(COM_PORT_1, msg));

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
void kmain(unsigned int magic_number, multiboot_info_t *multiboot_ptr)
{
	// Serial port will act as an early logging device.
	serial_init(COM_PORT_1, 3);

	// Init hardware.
	gdt_init();
	idt_init();
	keyboard_init();
	timer_init();

	// Check the magic number is valid. On why this check is made here and not
	// at the beginning of the function: we can defer the check until we
	// actually need to use the *multiboot_ptr structure. Even if the kernel
	// wasn't loaded using a multiboot compatible bootloader, anything done
	// until now (enabling ports and setting up early hardware drivers) would
	// be valid code because it doesn't depend on the multiboot structure.
	if (magic_number != 0x2BADB002) {
		LOG("PANIC: Wrong multiboot magic number! Check your bootloader.\n");
		for(;;);
	}
  
	// Not working
	/*
	unsigned int memory_amount = count_memory(multiboot_ptr);
	// Set all unused memory to 0 (NULL)
	kzero_memory(&kernel_after, memory_amount);
	printk("Memory amount = %d\n", memory_amount);
	*/

	// Check if kfree really is freeing the memory: Working
	/* 
	printk("\n\n");
	int* _int = kmalloc(sizeof(int) * 2);
	// int* _int4 = kmalloc(sizeof(int) * 2);
	// LOGf("%d [%d], %d [%d]\n", _int4[0], &_int4[0], _int4[1], &_int4[1]);
	_int[0] = 0xAAAA;
	_int[1] = 0xBBBB;
	printk("%x [%d], %x [%d]\n", _int[0], &_int[0], _int[1], &_int[1]);
	kfree(_int);
	printk("Free ended\n");
	int* _int2 = kmalloc(sizeof(int) * 3);
	_int2[0] = 0xCCCC;
	_int2[1] = 0xDDDD;
	printk("%x [%d], %x [%d]\n", _int2[0], &_int2[0], _int2[1], &_int2[1]);
	*/

	printk("Now printk() sends output to serial port if DEBUG = 1.\n");

	vbe_init(multiboot_ptr);
	frames_init(multiboot_ptr);

	int i;
	for (i = 0; i < 16; i++)
		idt_set_handler(i, &bsod);

	printk("NativeOS has started.\n\n>");

	/* Check if kfree really is freeing the memory */
	/*
	printk("\n\n");
	int* _int = kmalloc(sizeof(int) * 2);
	// int* _int4 = kmalloc(sizeof(int) * 2);
	// printk("%d [%d], %d [%d]\n", _int4[0], &_int4[0], _int4[1], &_int4[1]);
	_int[0] = 0xAAAA;
	_int[1] = 0xBBBB;
	printk("%x [%d], %x [%d]\n", _int[0], &_int[0], _int[1], &_int[1]);
	kfree(_int);
	printk("Free ended\n");
	int* _int2 = kmalloc(sizeof(int) * 3);
	_int2[0] = 0xCCCC;
	_int2[1] = 0xDDDD;
	printk("%x [%d], %x [%d]\n", _int2[0], &_int2[0], _int2[1], &_int2[1]);
	*/
	
	
	for(;;);
}
