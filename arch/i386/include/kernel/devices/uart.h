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
 * \file uart.h
 * \brief 8250 public API for i386
 *
 * This file exports a public API for the 8250 controller. Since the driver is
 * exposed as an I/O device at the VFS layer, this file is used mostly for
 * exporting structs and constants that can be used in IORQ requests.
 */

#pragma once

/**
 * \brief Serial configuration.
 *
 * This struct is used by the I/O request operations GETRATE and SETRATE to
 * handle the current settings of the port. It is expected for the client to
 * provide a pointer to a struct of this type when calling the I/O request.
 */
struct serial_config {
	unsigned int baud_rate;
};

/**
 * \brief Get the baud rate for the port
 *
 * This I/O request is used to ask the driver for the current baud rate of the
 * port. The value will be returned into a wrapped serial_config struct that
 * should be given by the caller of the I/O request.
 */
#define SERIAL_IORQ_GETRATE 0

/**
 * \brief Set the baud rate for the port
 *
 * This I/O request is used to change the baud rate used by the driver. The
 * value should be given wrapped into a serial_config struct that should be
 * given by the caller of the I/O request.
 */
#define SERIAL_IORQ_SETRATE 1

int serial_install(void);
