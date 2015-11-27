CC = gcc
CFLAGS = -m32 -nostdlib -nostartfiles -nodefaultlibs -nostdinc --freestanding \
		-fno-builtin -Wall -Wextra -g

AS = nasm
ASFLAGS = -f elf32

LD = ld
LDFLAGS = -melf_i386 -nostdlib


