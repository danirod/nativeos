/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

/**
 * \file  kern/hw_devices.h
 * \brief Hardware device manager
 *
 * This system module maintains a list of currently mounted devices, and
 * operations to install and remove devices from the list. While a device
 * is installed, it can be retrieved in order to use it.
 *
 * Until the VFS is implemented, some functions are added for accessing
 * the devices. These functions are device_open, device_read and device_write.
 * Note that these functions are unstable and they might be removed in the
 * future once there is a fully functional DEVFS running on top of the VFS.
 * I'm adding a warning here because, as usual, time has a different speed
 * inside this repository.
 */

#include <sys/device.h>
#include <sys/list.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

static int devfs_open(vfs_node_t *node, unsigned int flags);
static unsigned int devfs_read(vfs_node_t *, unsigned, void *, unsigned);
static unsigned int devfs_write(vfs_node_t *, unsigned, void *, unsigned);
static int devfs_close(vfs_node_t *node);
static vfs_node_t *devfs_readdir(vfs_node_t *node, unsigned int index);
static vfs_node_t *devfs_finddir(vfs_node_t *node, char *name);

static list_t *devmgr_list;
static vfs_node_t devfs_rootdir = {
    .vn_name = {0},
    .vn_flags = VN_FDIR,
    .vn_readdir = devfs_readdir,
    .vn_finddir = devfs_finddir,
};

void
device_init(void)
{
	extern char devices_start, devices_end;
	driver_t **driver_start, **driver_end, **driver;

	/* Init the data structures. */
	devmgr_list = list_alloc();
	vfs_mount(&devfs_rootdir, "DEV");

	/* Mount the devices. */
	driver_start = (driver_t **) &devices_start;
	driver_end = (driver_t **) &devices_end;
	for (driver = driver_start; driver < driver_end; driver++) {
		(*driver)->drv_init();
	}
}

int
device_install(device_t *dev, char *mtname)
{
	vfs_node_t *node;
	if ((node = devfs_finddir(0, mtname)) != 0)
		return -2; /* node name is taken. */
	if ((node = (vfs_node_t *) malloc(sizeof(vfs_node_t))) == 0)
		return -1; /* cannot allocate. */
	strncpy(node->vn_name, mtname, 64);
	node->vn_flags = VN_FCHARDEV;
	node->vn_payload = dev;
	node->vn_parent = &devfs_rootdir;
	node->vn_open = devfs_open;
	node->vn_read = devfs_read;
	node->vn_write = devfs_write;
	node->vn_close = devfs_close;
	list_append(devmgr_list, node);
	return 0;
}

void
device_remove(char *mtname)
{
	vfs_node_t *node = devfs_finddir(0, mtname);
	if (node) {
		list_delete(devmgr_list, node);
		free(node);
	}
}

static int
devfs_open(vfs_node_t *node, unsigned int flags)
{
	device_t *dev = (device_t *) node->vn_payload;
	if (dev && dev->dev_open) {
		return dev->dev_open(flags);
	}
	return -1;
}

static unsigned int
devfs_read(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	device_t *dev = (device_t *) node->vn_payload;
	unsigned char *chbuf = (unsigned char *) buf;
	unsigned int flags;

	if (dev && dev->dev_family) {
		flags = dev->dev_family->drv_flags;
		if ((flags & DV_FCHARDEV) != 0 && dev->dev_read_chr) {
			return dev->dev_read_chr(chbuf, len);
		} else if ((flags & DV_FBLCKDEV) != 0 && dev->dev_read_blk) {
			return dev->dev_read_blk(chbuf, offt, len);
		} else {
			return -1;
		}
	}

	return -1;
}

static unsigned int
devfs_write(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	device_t *dev = (device_t *) node->vn_payload;
	unsigned char *chbuf = (unsigned char *) buf;
	unsigned int flags;

	if (dev && dev->dev_family) {
		flags = dev->dev_family->drv_flags;
		if ((flags & DV_FCHARDEV) != 0 && dev->dev_write_chr) {
			return dev->dev_write_chr(chbuf, len);
		} else if ((flags & DV_FBLCKDEV) != 0 && dev->dev_write_blk) {
			return dev->dev_write_blk(chbuf, offt, len);
		} else {
			return -1;
		}
	}
	return -1;
}

static int
devfs_close(vfs_node_t *node)
{
	device_t *cdev = (device_t *) node->vn_payload;
	if (cdev && cdev->dev_close) {
		return cdev->dev_close();
	}
	return -1;
}

static vfs_node_t *
devfs_readdir(vfs_node_t *node, unsigned int index)
{
	/* TODO: Take into account node, which must be the root. */
	if (index < devmgr_list->count) {
		return (vfs_node_t *) list_at(devmgr_list, index);
	}
	return 0;
}

static vfs_node_t *
devfs_finddir(vfs_node_t *node, char *name)
{
	listnode_t *lnode;
	vfs_node_t *vnode;
	list_foreach(devmgr_list, lnode)
	{
		vnode = (vfs_node_t *) lnode->data;
		if (!strncmp(vnode->vn_name, name, 64)) {
			return vnode;
		}
	}
	return 0;
}
