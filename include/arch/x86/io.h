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
 * File: arch/x86/io.h
 * Description: IO routines
 */

#ifndef ARCH_X86_IO_H_
#define ARCH_X86_IO_H_

/* Put some data out using some port. */
void IO_OutP(unsigned short port, unsigned char data);

/* Read some data from some address. */
unsigned char IO_InP(unsigned short port);

#endif // ARCH_X86_IO_H_
