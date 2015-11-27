/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	io.h - Functions for accessing input and output
*/

#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_

/* Put some data out using some port. */
void IO_OutP(unsigned short port, unsigned char data);

/* Read some data from some address. */
unsigned char IO_InP(unsigned short port);

#endif // KERNEL_IO_H_
