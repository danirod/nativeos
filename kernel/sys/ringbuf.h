/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */
#pragma once

#define RINGBUF_FUL 1
#define RINGBUF_OVF 2

typedef struct ringbuf {
	unsigned char *buffer;
	unsigned int size;
	unsigned int writeptr;
	unsigned int readptr;

	unsigned int status;
} ringbuf_t;

/**
 * \brief Allocates a new ring buffer.
 * \param size the amount of bytes for the buffer of this ring buffer.
 * \return a pointer to the ring buffer.
 */
ringbuf_t *ringbuf_alloc(unsigned int size);

/**
 * \brief Writes a byte into the ring buffer.
 *
 * As a side effect, if writing this byte overwrites a byte still pending to
 * be read, the buffer will be marked as overflow, in order to signal that a
 * previous write operation overlapped an unread section of the buffer.
 *
 * \param buf the buffer to write into
 * \param byte the byte to place into the ring buffer.
 */
void ringbuf_write(ringbuf_t *buf, unsigned char byte);

/**
 * \brief Tests whether the buffer was overflowed.
 * \param buf the buffer to test for overflow.
 * \return a non-zero value if a previous write operation did overflow.
 */
int ringbuf_test_overflow(ringbuf_t *buf);

/**
 * \brief Tests whether the buffer contains data pending to be read.
 * \param buf the buffer to test for readiness.
 * \return a non-zero value if the buffer contains data waiting to be read.
 */
int ringbuf_test_ready(ringbuf_t *buf);

/**
 * \brief Reads a byte from the given ring buffer.
 * Note that calling this function in a buffer that is not ready to be read
 * has undefined side effects and it is possible that junk will be retrieved.
 * \param buf the buffer where the data should be read
 * \return the byte read from the ring buffer.
 */
unsigned char ringbuf_read(ringbuf_t *buf);

/**
 * \brief Deallocates a previously allocated ring buffer.
 * \param buf the ring buffer to deallocate.
 */
void ringbuf_free(ringbuf_t *buf);