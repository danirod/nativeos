/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <i386/include/paging.h>
#include <machine/multiboot.h>
#include <sys/device.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

/**
 * \file kern/kern_main.c
 * \brief Kernel main entrypoint
 *
 * This function contains code executed right after the platform loader has
 * finished with the platform-dependent stuff. Code in this module should
 * finish with the system initialisation and delegate to the init process
 * to have a proper userland switch. (Or until this is programmed, at least
 * do something useful).
 */

static void ramdisk_init(void);
static void kernel_welcome(void);

extern void syscall_init(void);
extern int syscall_open(const char *path, unsigned int attr);
extern int syscall_close(int fd);
extern int syscall_read(int fd, void *buf, unsigned int len);
extern int syscall_write(int fd, void *buf, unsigned int len);

/**
 * \brief Kernel main
 *
 * Virtually the main function, although you should know that the platform
 * loader may have done additional stuff before reaching here. Check out
 * code preceding a call to kernel_main in the loader code for each platform.
 */
void
kernel_main(void)
{
	vfs_init();
	device_init();
	ramdisk_init();
	syscall_init();
	enable_paging();
	kernel_welcome();
}

static void
ramdisk_init(void)
{
	unsigned int i;
	multiboot_module_t *multiboot_mods;
	unsigned char *tar;

	multiboot_mods = (multiboot_module_t *) multiboot_info->mods_addr;
	for (i = 0; i < multiboot_info->mods_count; i++) {
		/* We can afford to strcmp because "ramdisk" is static. */
		if (!strcmp("ramdisk", (char *) multiboot_mods[0].string)) {
			/* We found the ramdisk. */
			tar = (unsigned char *) multiboot_mods[i].mod_start;
			vfs_mount("tarfs", "INITRD", tar);
			return;
		}
	}
}

static void
kernel_welcome(void)
{
	int vtcon, motd, read;
	char buffer[64];

	vtcon = syscall_open("DEV:/vtcon", VO_FWRITE);
	motd = syscall_open("INITRD:/SYSTEM/MOTD.TXT", VO_FREAD);
	if (motd) {
		while ((read = syscall_read(motd, buffer, 64)) > 0) {
			syscall_write(vtcon, buffer, read);
		}
		syscall_write(vtcon, "\n", 1);
		syscall_close(motd);
	}
	for (;;) {
		syscall_read(vtcon, buffer, 64);
	}
}
