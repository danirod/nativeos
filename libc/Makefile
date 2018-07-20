ARCH ?= i386

include arch/$(ARCH)/include.mk

CFLAGS=-Iinclude -g
LIBC_SOURCES = $(shell find src -name '*.c')
LIBC_OBJECTS = $(patsubst %.c, %.o, $(LIBC_SOURCES))

.PHONY = clean

libc.a: $(LIBC_OBJECTS)
	$(AR) r $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(LIBC_OBJECTS) libc.a
