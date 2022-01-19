/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <fs/tarfs/tar.h>
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

// FIXME: These should not be declared here.
extern void null_install(void);
extern void vgatext_install(void);
extern void keyboard_install(void);
extern void pctimer_install(void);
extern void uart8250_install(void);

static void ramdisk_init(void);
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
	ramdisk_init();
	kernel_welcome();
}

static void
ramdisk_init(void)
{
	multiboot_module_t *multiboot_mods;
	unsigned char *tar_buffer;

	multiboot_mods = (multiboot_module_t *) multiboot_info->mods_addr;
	tar_buffer = (unsigned char *) multiboot_mods[0].mod_start;
	vfs_mount("tarfs", "INITRD", tar_buffer);
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

static void dump_path(vfs_node_t *stdout, char *path);
static void dump_dir(vfs_node_t *stdout, char *path, vfs_node_t *dir);

static void
dump_dir(vfs_node_t *stdout, char *path, vfs_node_t *dir)
{
	unsigned int i = 0;
	vfs_node_t *node;
	size_t child_len;
	char *child_path;

	while ((node = fs_readdir(dir, i++))) {
		child_len = strlen(path) + strlen(node->vn_name) + 2;
		child_path = (char *) malloc(child_len);
		if (child_path) {
			strcpy(child_path, path);
			/* If the folder I'm in is not root. */
			if (node->vn_parent->vn_parent) {
				strcat(child_path, "/");
			}
			strcat(child_path, node->vn_name);
			dump_path(stdout, child_path);
			free(child_path);
		}
	}
}

static void
dump_file(vfs_node_t *stdout, vfs_node_t *node)
{
	unsigned int read = 0;
	char buf[128];
	unsigned int offset = 0;

	fs_write_string(stdout, 0, "\n");
	while ((read = fs_read(node, offset, buf, 128)) > 0) {
		fs_write(stdout, 0, buf, read);
		offset += read;
	}
	fs_write_string(stdout, 0, "\n");
}

static void
dump_path(vfs_node_t *stdout, char *path)
{
	vfs_node_t *node = fs_resolve(path);

	if (node) {
		switch (node->vn_flags) {
		case VN_FREGFILE:
			fs_write_string(stdout, 0, "[FIL]");
			break;
		case VN_FDIR:
			fs_write_string(stdout, 0, "[DIR]");
			break;
		case VN_FCHARDEV:
			fs_write_string(stdout, 0, "[CHD]");
			break;
		case VN_FBLOCKDEV:
			fs_write_string(stdout, 0, "[BLD]");
			break;
		}
		fs_write_string(stdout, 0, " ");
		fs_write_string(stdout, 0, path);
		fs_write_string(stdout, 0, " (");
		fs_write_string(stdout, 0, node->vn_name);
		fs_write_string(stdout, 0, ")\n");
		if (node->vn_flags == VN_FDIR) {
			dump_dir(stdout, path, node);
		} else if (node->vn_flags == VN_FREGFILE) {
			dump_file(stdout, node);
		}
	}
}

static void
dump_mountpoint(vfs_node_t *stdout, char *name)
{
	char *path = malloc(strlen(name) + 3);
	strcpy(path, name);
	strcat(path, ":/");
	dump_path(stdout, path);
	fs_write_string(stdout, 0, "\n");
}

static void
dump_mountpoints(vfs_node_t *stdout)
{
	vfs_node_t *rootfs, *mountpoint;
	unsigned int i = 0;

	rootfs = fs_resolve("ROOT:/");
	if (rootfs) {
		while ((mountpoint = fs_readdir(rootfs, i++))) {
			/* Make sure ROOT is not dumped, infinite loop! */
			if (strcmp(mountpoint->vn_name, "ROOT")) {
				dump_mountpoint(stdout, mountpoint->vn_name);
			}
		}
	}
}

static void
print_mountpoints(vfs_node_t *stdout)
{
	vfs_node_t *rootfs, *mountpoint;
	unsigned int i = 0;

	rootfs = fs_resolve("ROOT:/");
	if (rootfs) {
		fs_write_string(stdout, 0, "Mounted file systems:\n");
		while ((mountpoint = fs_readdir(rootfs, i++))) {
			fs_write_string(stdout, 0, mountpoint->vn_name);
			fs_write_string(stdout, 0, "\n");
		}
		fs_write_string(stdout, 0, "\n");
	} else {
		fs_write_string(stdout, 0, "Error! No rootfs is present\n\n");
	}
}

static unsigned int
octal2int(char *octstring)
{
	unsigned int acc = 0;
	while (*octstring) {
		acc = acc * 8 + (*octstring - '0');
		octstring++;
	}
	return acc;
}

static void
kernel_welcome(void)
{
	vfs_node_t *vtcon, *kbd, *uart;
	vtcon = fs_resolve_and_open("DEV:/vtcon", VO_FWRITE);
	uart = fs_resolve_and_open("DEV:/uart", VO_FWRITE | VO_FREAD);

	print_mountpoints(vtcon);
	dump_mountpoints(vtcon);

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
