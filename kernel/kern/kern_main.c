/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

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

// FIXME: These should not be declared here.
extern void null_install(void);
extern void vgatext_install(void);
extern void keyboard_install(void);
extern void pctimer_install(void);
extern void uart8250_install(void);

static void kernel_welcome(void);

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
	kernel_welcome();
}

static vfs_node_t *
fs_resolve_and_open(const char *path, unsigned int args)
{
	vfs_node_t *node = fs_resolve(path);
	if (node) {
		if (fs_open(node, args) != 0) {
			node = NULL;
		}
	}
	return node;
}

static unsigned int
fs_write_string(vfs_node_t *node, unsigned int offt, const char *str)
{
	return fs_write(node, offt, (unsigned char *) str, strlen(str));
}

static void
kernel_welcome(void)
{
	vfs_node_t *vtcon, *kbd, *uart;
	vtcon = fs_resolve_and_open("DEV:/vtcon", VO_FWRITE);
	uart = fs_resolve_and_open("DEV:/uart", VO_FWRITE | VO_FREAD);

	fs_write_string(vtcon, 0, "This is the NativeOS VTCON\n");
	fs_write_string(vtcon, 0, "Press any key to test the keyboard :)\n\n");

	fs_write_string(uart, 0, "This is the NativeOS UART\n");
	fs_write_string(uart, 0, "Type some characters to get echo:\n\n");

	unsigned char buf[16];
	unsigned int read;
	for (;;) {
		fs_read(vtcon, 0, buf, 16);

		read = fs_read(uart, 0, buf, 16);
		fs_write(uart, 0, buf, read);
	}
}
