/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
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
 *
 *
 * File: driver/com.c
 * Description: serial port driver
 */

#include <arch/x86/io.h>
#include <driver/com.h>

/**
 * Flips DLAB bit on. Call this before doing operations with baud rates.
 * @param device device to turn DLAB bit on.
 */
static inline void serial_enable_dlab(unsigned short device)
{
	unsigned char lc_status = IO_InP(COM_REGISTER_LINE_CONTROL(device));
	IO_OutP(COM_REGISTER_LINE_CONTROL(device), lc_status | 0x80);
}

/**
 * Flips DLAB bit off. Call this after doing operations with baud rates.
 * @param device device to turn DLAB bit off.
 */
static inline void serial_disable_dlab(unsigned short device)
{
	unsigned char lc_status = IO_InP(COM_REGISTER_LINE_CONTROL(device));
	IO_OutP(COM_REGISTER_LINE_CONTROL(device), lc_status & 0x7F);
}

/**
 * Initialises (or reinitialises) a serial port. This method is required to be
 * executed before setting up some serial transmission. This method is reserved
 * for kernel purposes and should not be shared into usersepace.
 * 
 * Please note that at the moment the kernel driver is fixed to use 8 bits per
 * character, 1 stop bit and no parity. This is the most common set up and
 * even if a more complete driver could be implemented, there is no need for
 * a debugging kernel driver as this one.
 * 
 * @param device the COM device to initialize, COM_PORT_1 or COM_PORT_2. 
 * @param divisor the divisor to use when calculating internal clock speed.
 */
void serial_init(unsigned short device, unsigned short divisor)
{
	serial_enable_dlab(device);
	IO_OutP(COM_REGISTER_BAUD_LSB(device), divisor & 0x00FF);
	IO_OutP(COM_REGISTER_BAUD_MSB(device), (divisor >> 8) & 0x00FF);
	serial_disable_dlab(device);

	// Set up bits per word, parity and stop bits.
	IO_OutP(COM_REGISTER_LINE_CONTROL(device), 0x03);
}

/**
 * This function will return the current baud rate for a serial device.
 * @param device device to change baud rate.
 * @return divisor in use by the baud rate. 115200/this is the baud rate.
 */
unsigned short serial_get_baud_rate(unsigned short device)
{
	serial_enable_dlab(device);
	unsigned char lsb = IO_InP(COM_REGISTER_BAUD_LSB(device));
	unsigned char msb = IO_InP(COM_REGISTER_BAUD_MSB(device));
	serial_disable_dlab(device);
	return msb << 8 | lsb;
}

int serial_get_recv_status(unsigned short device)
{
	return IO_InP(COM_REGISTER_LINE_STATUS(device)) & COM_LINE_READY;
}

unsigned char serial_recv_byte(unsigned short device)
{
	return IO_InP(COM_REGISTER_RX_TX(device));
}

void serial_send_byte(unsigned short device, unsigned char byte)
{
	IO_OutP(COM_REGISTER_RX_TX(device), byte);
}

void serial_send_str(unsigned short device, char* string)
{
	for (char* ch = string; *ch; ch++) {
		serial_send_byte(device, *ch);
	}
}