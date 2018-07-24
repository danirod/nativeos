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
 * \file uart.c
 * \brief 8250 implementation for i386
 *
 * This file implements an 8250 IBM UART driver for the i386.  This driver is
 * able to talk to serial ports, such as the 9 pin RS-232 port found in old
 * i386 PCs.  While this port is not as ubiquous anymore as it used to be, it's
 * still handy to debug stuff on virtual machines using this port.
 */

#include <kernel/fs.h>

#include <kernel/devices/uart.h>
#include <kernel/cpu/io.h>
#include <kernel/cpu/spinlock.h>

// Port definition for every COM port.
#define COM_PORT_1 0x3F8
#define COM_PORT_2 0x2F8

// Port offsets
#define COM_REGISTER_RX_TX(bp) (bp)
#define COM_REGISTER_BAUD_LSB(bp) (bp)
#define COM_REGISTER_INTERRUPT(bp) (bp + 1)
#define COM_REGISTER_BAUD_MSB(bp) (bp + 1)
#define COM_REGISTER_FIFO_CONTROL(bp) (bp + 2)
#define COM_REGISTER_LINE_CONTROL(bp) (bp + 3)
#define COM_REGISTER_MODEM_CONTROL(bp) (bp + 4)
#define COM_REGISTER_LINE_STATUS(bp) (bp + 5)
#define COM_REGISTER_MODEM_STATUS(bp) (bp + 6)

// Line Status Register
#define COM_LINE_RECV_ERROR 0x80
#define COM_LINE_EMPTY_DATA 0x40
#define COM_LINE_EMPTY_TX 0x20
#define COM_LINE_BREAK 0x10
#define COM_LINE_FRAMING_ERROR 0x08
#define COM_LINE_PARITY_ERROR 0x04
#define COM_LINE_OVERRUN_ERROR 0x02
#define COM_LINE_READY 0x01

#define COM_RXTX_RETRIES 10

static struct {
	unsigned int baud_rate;
	unsigned short divisor;
} baud_rates[] = {
	{ 115200, 1},
	{ 57600, 2 },
	{ 38400, 3 },
	{ 19200, 6 },
	{ 9600, 12 },
	{ 4800, 24 },
	{ 2400, 48 },
	{ 1200, 96 },
	{ 600, 192 },
	{ 300, 384 },
	{ 220, 524 },
	{ 110, 1047 },
	{ 50, 2304 },
	{ 0, 0 }
};

/**
 * \brief Gets the current baud rate for this device.
 * \param device the device to get the baud rate for
 * \return the current baud rate for this device
 */
static unsigned int
serial_get_baud (unsigned short device)
{
	unsigned char line_control, divisor_lsb, divisor_msb;
	unsigned short divisor;
	unsigned int i = 0, rate;

	/* Switch to DLAB mode and read the divisor.  */
	line_control = IO_InP(COM_REGISTER_LINE_CONTROL(device));
	IO_OutP(COM_REGISTER_LINE_CONTROL(device), line_control | 0x80);
	divisor_lsb = IO_InP(COM_REGISTER_BAUD_LSB(device));
	divisor_msb = IO_InP(COM_REGISTER_BAUD_MSB(device));
	IO_OutP(COM_REGISTER_LINE_CONTROL(device), line_control);

	/* Cast the divisor into the baud rate.  */
	divisor = divisor_msb << 8 | divisor_lsb;
	while (baud_rates[i].divisor != 0) {
		if (baud_rates[i].divisor == divisor) {
			return baud_rates[i].baud_rate;
		}
		++i;
	}
	return -1;
}

/**
 * \brief Update the baud rate for this device.
 * \param device the device to set the baud rate
 * \param the new baud rate requested for the device
 * \return 0 if the baud rate is successfully set.
 */
static int
serial_set_baud (unsigned short device, unsigned int rate)
{
	unsigned char line_reg, divisor_lsb, divisor_msb;
	unsigned int i = 0;

	/* Get the appropiate divisor.  */
	while (baud_rates[i].divisor != 0) {
		if (baud_rates[i].baud_rate != rate) {
			++i;
			continue;
		}

		/* Take the bytes of this divisor.  */
		divisor_lsb = baud_rates[i].divisor & 0xFF;
		divisor_msb = (baud_rates[i].divisor >> 8) & 0xFF;

		/* Switch to DLAB and write the divisor.  */
		line_reg = IO_InP(COM_REGISTER_LINE_CONTROL(device));
		IO_OutP(COM_REGISTER_LINE_CONTROL(device), line_reg | 0x80);
		IO_OutP(COM_REGISTER_BAUD_LSB(device), divisor_lsb);
		IO_OutP(COM_REGISTER_BAUD_MSB(device), divisor_msb);
		IO_OutP(COM_REGISTER_LINE_CONTROL(device), line_reg);

		/* Successfully set the mode.  */
		return 0;
	}

	/* Nothing to be done.  Fail.  */
	return -1;
}

/**
 * \brief Tests if the UART is ready.
 *
 * The UART should be checked before accessing it in read mode to see if more
 * bytes are ready to be read. If this function returns 0, it means that there
 * are no bytes in the buffer for the device file to read.
 *
 * \return 1 unless the device is busy.
 */
static inline int
serial_recv_status (unsigned short device)
{
	return IO_InP(COM_REGISTER_LINE_STATUS(device)) & COM_LINE_READY;
}

/**
 * \brief Reads a byte from the given serial port.
 * \param device the device to get the data from.
 * \return the given byte as read by the UART.
 */
static inline unsigned char
serial_recv_byte (unsigned short device)
{
	return IO_InP(COM_REGISTER_RX_TX(device));
}

/**
 * \brief Wait until the device is ready for reading or fail.
 * \param device the device to test if there is data to read.
 * \param retries the amount of retries to use while it's not ready.
 * \return 1 if the device is ready, 0 if it's not.
 */
static inline int
serial_recv_ready (unsigned short device, unsigned int retries) {
	register unsigned char status;
	while (retries--) {
		status = IO_InP(COM_REGISTER_LINE_STATUS(device));
		if (status & COM_LINE_READY) {
			return 1;
		}
	}
	return 0;
}

/**
 * \brief Wait until the device is ready for writing or fail.
 * \param device the device to test if it's ready for write.
 * \param retries the amount of retries to use while it's not ready.
 * \return 1 if the device is ready, 0 if it's not.
 */
static inline int
serial_send_ready (unsigned short device, unsigned int retries) {
	register unsigned char status;
	while (retries--) {
		status = IO_InP(COM_REGISTER_LINE_STATUS(device));
		if (status & COM_LINE_EMPTY_TX) {
			return 1;
		}
	}
	return 0;
}

/**
 * \brief Sends a byte to the given UART device.
 * \param device the device to get the data from.
 * \param byte the given byte to send to the UART.
 */
static inline void
serial_send_byte (unsigned short device, unsigned char byte)
{
	IO_OutP(COM_REGISTER_RX_TX(device), byte);
}

/**
 * \brief Serial spinlock.
 *
 * This spinlock is used to avoid having two handles opened at the same time
 * targetting the same port. If a second file is opened, it should fail
 * gracefully during the vfs_open call.
 */
static spinlock_t serial_fd_spinlock;

/**
 * \brief Read operation for the serial node.
 *
 * This function implements the read operation so that the serial node is able
 * to read data from the port. By calling this function, data will be taken
 * from the serial port and written into the given buffer.
 *
 * \param node this node.
 * \param buffer the buffer where data will be copied into.
 * \param offset how many bytes to offset in the buffer.
 * \param length how many bytes to copy.
 * \return how many bytes were actually copied.
 */
static size_t
serial_fd_read (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length)
{
	int count;
	unsigned char * target;

	/* Read from the device node the given number of bytes.  */
	target = buffer + offset;
	count = 0;
	while (length--) {
		if (!serial_recv_ready(COM_PORT_1, COM_RXTX_RETRIES)) {
			break;
		}
		target[count++] = IO_InP(COM_REGISTER_RX_TX(COM_PORT_1));
	}

	return count;
}

/**
 * \brief Write operation for the serial node.
 *
 * This function implements the write operation so that the serial node is able
 * to write data into the port. By calling this function, data will be copied
 * into the serial port as read by the given buffer.
 *
 * \param node this node
 * \param bfufer the buffer where data will be taken from
 * \param offset how many bytes to offset in the buffer
 * \param length how many bytes to copy
 * \return how many bytes were actually copied.
 */
static size_t
serial_fd_write (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length)
{
	int count;
	unsigned char * target;

	/* Write into the device node the given amount of bytes.  */
	target = buffer + offset;
	count = 0;
	while (length--) {
		if (!serial_send_ready(COM_PORT_1, COM_RXTX_RETRIES)) {
			break;
		}
		IO_OutP(COM_REGISTER_RX_TX(COM_PORT_1), target[count++]);
	}

	return count;
}

/**
 * \brief Open operation for the serial node.
 *
 * This function implements the open operation so that the serial node is able
 * to be opened. By calling this function, the spinlock will be reserved if
 * possible. Otherwise, it means that another descriptor is open, so the call
 * will be blocked.
 *
 * \param node this node
 * \param argp unused, but should be NULL.
 * \return 0 if the node is opened, 2 if the descriptor is busy.
 */
static int
serial_fd_open (struct vfs_node * node, void * argp)
{
	if (spinlock_try_lock(&serial_fd_spinlock, 10)) {
		return 2;
	}
	return 0;
}

/**
 * \brief Close operation for the serial node.
 *
 * This function implements the close operation so that the serial node is able
 * to be closed. This function will free the spinlock that has been reserved
 * when calling open.
 *
 * \param node this node
 * \return 0 if the node is closed
 */
static int
serial_fd_close (struct vfs_node * node)
{
	spinlock_release(&serial_fd_spinlock);
	return 0;
}

/**
 * \brief I/O request operation for the serial node.
 *
 * This function implements the iorq operation so that this node is able to
 * respond to I/O requests.
 *
 * \param node this node
 * \param iorq the I/O request code
 * \param argp additional parameters required by some I/O request parameters
 * \return the output of the given request.
 */
static void *
serial_fd_iorq (struct vfs_node * node, unsigned int iorq, void * argp)
{
	struct serial_config * config = (struct serial_config *) argp;

	if (iorq == SERIAL_IORQ_GETRATE) {
		config->baud_rate = serial_get_baud(COM_PORT_1);
		return argp;
	}
	else if (iorq == SERIAL_IORQ_SETRATE) {
		serial_set_baud(COM_PORT_1, config->baud_rate);
		return argp;
	}
	return 0;
}

static struct vfs_node serial_fd = {
	.type = VFS_TYPE_CHARDEV,
	.open = &serial_fd_open,
	.close = &serial_fd_close,
	.read = &serial_fd_read,
	.write = &serial_fd_write,
	.iorq = &serial_fd_iorq
};
