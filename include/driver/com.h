/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez
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

#ifndef DRIVER_COM_H_
#define DRIVER_COM_H_

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

// Use define for better performance
#define serial_recv_byte(device) (IO_InP(COM_REGISTER_RX_TX(device)))
#define serial_send_byte(device, byte) (IO_OutP(COM_REGISTER_RX_TX(device), (byte)))

void serial_init(unsigned short device, unsigned short divisor);
unsigned short serial_get_baud_rate(unsigned short device);
int serial_get_recv_status(unsigned short device);
// unsigned char serial_recv_byte(unsigned short device);
// void serial_send_byte(unsigned short device, unsigned char byte);
void serial_send_str(unsigned short device, char* string);

#endif // DRIVER_COM_H_