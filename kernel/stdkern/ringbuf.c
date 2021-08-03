/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <sys/ringbuf.h>
#include <sys/stdkern.h>

ringbuf_t *
ringbuf_alloc(unsigned int size)
{
	ringbuf_t *ringbuf = (ringbuf_t *) malloc(sizeof(ringbuf_t));
	if (ringbuf) {
		ringbuf->size = size;
		ringbuf->writeptr = 0;
		ringbuf->readptr = 0;
		ringbuf->buffer = (unsigned char *) malloc(size);
		/* TODO: Fill the buffer with zeros. */
		if (!ringbuf->buffer) {
			free(ringbuf);
			ringbuf = 0;
		}
	}
	return ringbuf;
}

void
ringbuf_write(ringbuf_t *buf, unsigned char byte)
{
	if (buf->writeptr == buf->readptr && buf->status & RINGBUF_FUL) {
		/* We are writing in a full buffer, so it's an overflow. */
		buf->status |= RINGBUF_OVF;
		if (++buf->readptr == buf->size)
			buf->readptr = 0;
	}
	buf->buffer[buf->writeptr] = byte;
	if (++buf->writeptr == buf->size)
		buf->writeptr = 0;
	if (buf->writeptr == buf->readptr)
		buf->status |= RINGBUF_FUL;
}

int
ringbuf_test_overflow(ringbuf_t *buf)
{
	return buf->status & RINGBUF_OVF;
}

int
ringbuf_test_ready(ringbuf_t *buf)
{
	return buf->writeptr != buf->readptr || buf->status & RINGBUF_FUL;
}

unsigned char
ringbuf_read(ringbuf_t *buf)
{
	unsigned char byte = buf->buffer[buf->readptr];
	if (++buf->readptr == buf->size)
		buf->readptr = 0;
	buf->status = 0; /* clear bytes */
	return byte;
}

void
ringbuf_free(ringbuf_t *buf)
{
	if (buf) {
		free(buf->buffer);
	}
	free(buf);
}