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

static list_t *device_list;

/**
 * \brief Mountpoint node structure
 *
 * This is the internal data structure used by the device list in order to
 * hold the linked list of mounted devices. The data structure contains the
 * name used to mount the device when calling device_install.
 */
typedef struct device_mountpoint {
	char mtp_name[64];
	chardev_t *mtp_chardev;
} device_mountpoint_t;

static inline device_mountpoint_t *
get_mountpoint_by_name(const char *mtname)
{
	listnode_t *node;
	device_mountpoint_t *mtpoint;
	list_foreach(device_list, node)
	{
		mtpoint = (device_mountpoint_t *) node->data;
		if (!strncmp(mtpoint->mtp_name, mtname, 64)) {
			return mtpoint;
		}
	}
	return 0;
}

/*
 * Note: this is an unstable function that will exist until NativeOS has
 * a real DEVFS mounted on top of the VFS. Eventually, to open a device you'd
 * open the proper device file rather than using this hack.
 *
 * The "file descriptor" returned by this function is the index in the device
 * list, which is then used in the read and write operations in order to
 * point to the proper device.
 *
 * DO NOT ASSUME THAT THIS FUNCTION WILL ALWAYS EXIST.
 */
int
device_open(char *mtname)
{
	device_mountpoint_t *node;
	if ((node = get_mountpoint_by_name(mtname)) != 0) {
		return list_index(device_list, node);
	}
	return -1;
}

/*
 * Note: this is an unstable function that will exist until NativeOS has
 * a real DEVFS mounted on top of the VFS. Eventually, to read data from a
 * device you'd call read on the file descriptor you get after opening the
 * device file.
 *
 * DO NOT ASSUME THAT THIS FUNCTION WILL ALWAYS EXIST.
 */
unsigned int
device_read(int fd, unsigned char *buf, unsigned int len)
{
	device_mountpoint_t *node;
	if (fd >= 0) {
		node = list_at(device_list, fd);
		if (node && node->mtp_chardev->cd_read) {
			return node->mtp_chardev->cd_read(buf, len);
		}
	}
	return (unsigned int) -1;
}

/*
 * Note: this is an unstable function that will exist until NativeOS has
 * a real DEVFS mounted on top of the VFS. Eventually, to write data into a
 * device you'd call write on the file descriptor you get after opening the
 * device file.
 *
 * DO NOT ASSUME THAT THIS FUNCTION WILL ALWAYS EXIST.
 */
unsigned int
device_write(int fd, unsigned char *buf, unsigned int len)
{
	device_mountpoint_t *node;
	if (fd >= 0) {
		node = list_at(device_list, fd);
		if (node && node->mtp_chardev->cd_write) {
			return node->mtp_chardev->cd_write(buf, len);
		}
	}
	return (unsigned int) -1;
}

int
device_install(chardev_t *dev, char *mtname)
{
	device_mountpoint_t *mtpoint;

	/* No mountpoints must share name. */
	if (get_mountpoint_by_name(mtname)) {
		return -2;
	}

	mtpoint = malloc(sizeof(device_mountpoint_t));
	if (mtpoint) {
		strncpy(mtpoint->mtp_name, mtname, 64);
		mtpoint->mtp_chardev = dev;
		list_append(device_list, mtpoint);
		return 0;
	}
	return -1;
}

void
device_remove(char *mtname)
{
	device_mountpoint_t *mtpoint;

	if ((mtpoint = get_mountpoint_by_name(mtname)) != 0) {
		list_delete(device_list, mtpoint);
		free(mtpoint);
	}
}

void
device_init(void)
{
	device_list = list_alloc();
}
