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

	// TODO: These should be dynamically loaded (linker sets?)
	null_install();
	vgatext_install();

	kernel_welcome();
}

static void
kernel_welcome(void)
{
	/* FIXME: should be done using the VFS API. */
	vfs_node_t *devfs, *fb;
	devfs = vfs_get_volume("DEV");
	fb = devfs->vn_finddir(devfs, "fb");
	fb->vn_write(fb, "NativeOS\n", strlen("NativeOS\n"));
}
