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
 * \file devfs.c
 * \brief device file system
 *
 * The device file system is a special file system exposed through the VFS that
 * allows kernel code to interact with devices using standard VFS operations
 * such as read or write.
 */

#include <kernel/fs.h>
#include <kernel/cpu/spinlock.h>
#include <kernel/mem/heap.h>

#include <string.h>

struct devfs_devnode;

/**
 * \brief Device node.
 *
 * The device node maintains the VFS node that allows code to interact with a
 * device, as well as the given name for that device.  This allows complex
 * devices that depend on the system platform to be used in an abstract
 * fashion.
 */
struct devfs_devnode {
	/** The name of the file as exposed through the VFS. */
	char name[64];

	/** The VFS node for this device. */
	struct vfs_node * node;

	/** A pointer to the next device in the linked list. */
	struct devfs_devnode * next;
};

/** The amount of devices currently in the system. */
static unsigned int devfs_devcount;

/** Pointers for the linked list of device nodes. */
static struct devfs_devnode * devfs_headdev, * devfs_taildev;

/** Locks the manipulation of the device node linked list. */
static spinlock_t devfs_spinlock;

/** VFS node for the root directory of the device file system. */
static struct vfs_node devfs_fd;

/**
 * Adds a device node into the device node linked list used to expose new
 * devices through the abstract VFS API.
 */
int
devfs_device_install (const char * name, struct vfs_node * node)
{
	struct devfs_devnode * devnode;

	/* Enter critical region.  */
	if (spinlock_try_lock(&devfs_spinlock, 10)) {
		return -1;
	}

	/* Create the new node entry.  */
	devnode = heap_alloc(sizeof (struct devfs_devnode));
	strcpy(devnode->name, name);
	devnode->node = node;

	/* Place this node in the linked list.  */
	if (!devfs_headdev) {
		devfs_headdev = devnode;
		devfs_taildev = devnode;
	} else {
		devfs_taildev->next = devnode;
		devfs_taildev = devnode;
	}

	++devfs_devcount;

	/* Clean up.  */
	spinlock_release(&devfs_spinlock);
	return 0;
}

/**
 * \brief readdir operation for the root directory of the device file system.
 * \param root the node that should represent the device file system
 * \param index the number of node to retrieve
 * \param pointer to the node to retrieve or NULL if no more nodes.
 */
static struct vfs_dirent *
devfs_fd_readdir (struct vfs_node * root, unsigned int index)
{
	struct devfs_devnode * devptr;
	struct vfs_dirent * dirent;
	char name[64];
	unsigned int inode;

	spinlock_lock(&devfs_spinlock, "readdir");

	if (index >= devfs_devcount + 2) {
		/* Invalid pointer.  */
		dirent = 0;
		goto _cleanup;
	}

	/* Easy case is if . or .. are requested. They map to the root.  */
	if (index == 0) {
		strncpy(name, ".", 64);
		inode = 2;
	}
	else if (index == 1) {
		strncpy(name, "..", 64);
		inode = 2;
	}

	/* The difficult case is to actually look for a node.  */
	else {
		index -= 2;
		devptr = devfs_headdev;
		while (index--) {
			devptr = devptr->next;
		}
		strcpy(name, devptr->name);
		inode = devptr->node->inode;
	}

	/* Copy information about the node into the structure.  */
	dirent = heap_alloc(sizeof (struct vfs_dirent));
	strcpy(dirent->name, name);
	dirent->inode = inode;
_cleanup:
	spinlock_release(&devfs_spinlock);
	return dirent;
}

/**
 * \brief finddir operation returns a node by its name.
 * \param node the node that should represent the root of the file system
 * \param name the name of the node to retrieve
 * \return pointer to the node with this file or NULL if not found.
 */
static struct vfs_node *
devfs_fd_finddir(struct vfs_node * node, const char * name)
{
	struct vfs_node * fd;
	struct devfs_devnode * devnode;

	spinlock_lock(&devfs_spinlock, "finddir");

	/* Easy case returns the devfs root.  */
	if (!strcmp(name, ".") || !strcmp(name, "..")) {
		fd = &devfs_fd;
		goto _cleanup;
	}

	/* Otherwise we have to look for a node with this name.  */
	else for (devnode = devfs_headdev; devnode; devnode = devnode->next) {
		if (!strncmp(devnode->name, name, 64)) {
			fd = devnode->node;
			goto _cleanup;
		}
	}

	/* If we got here, there is no node with such name.  */
	fd = 0;
_cleanup:
	spinlock_release(&devfs_spinlock);
	return fd;
}

static struct vfs_node devfs_fd = {
	.type = VFS_TYPE_DIRECTORY,
	.readdir = &devfs_fd_readdir,
	.finddir = &devfs_fd_finddir
};
