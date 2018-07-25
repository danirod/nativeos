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
 * \file devfs.h
 * \brief public API for the device file systm
 *
 * The device file system is a special file system exposed through the VFS that
 * allows kernel code to interact with devices using standard VFS operations
 * such as read or write.  This file exposes the public API, with operations
 * for installing device nodes.
 */

#pragma once

#include <kernel/fs.h>

/**
 * \brief Installs a device into the DEVFS.
 * \param name the name of the device as exposed through the file system.
 * \param node an VFS node with operations for interacting with the device.
 * \return 0 unless there is an error.
 */
int devfs_device_install (const char * name, struct vfs_node * node);
