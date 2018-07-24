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
 * \file fs.h
 * \brief VFS public API
 *
 * This file has the public API for the VFS subsystem.  The VFS subsystem
 * provides an unified interface through hard drives and other special devices
 * using files, directories and other simple abstractions.
 *
 * TODO: Provide API comments.
 */

#pragma once

#include <stddef.h>

#define VFS_TYPE_NUL 0x00
#define VFS_TYPE_FILE 0x01
#define VFS_TYPE_DIRECTORY 0x02
#define VFS_TYPE_CHARDEV 0x03

struct vfs_node;

typedef int (vfs_open_t) (struct vfs_node * node, void * argp);
typedef int (vfs_close_t) (struct vfs_node * node);
typedef size_t (vfs_read_t) (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length);
typedef size_t (vfs_write_t) (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length);
typedef void * (vfs_iorq_t) (struct vfs_node * node, unsigned int iorq,
		void * argp);

struct vfs_node {
	unsigned int inode, length, type;
	vfs_open_t * open;
	vfs_close_t * close;
	vfs_read_t * read;
	vfs_write_t * write;
	vfs_iorq_t * iorq;
};

