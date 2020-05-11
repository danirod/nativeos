# This file is part of NativeOS
# Copyright (C) 2015-2020 The NativeOS contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

DEBUG ?= 0

# Tools
AR = i386-elf-ar
AS = i386-elf-as
CC = i386-elf-gcc

# Tool flags
CFLAGS = -nostdlib --freestanding -fno-builtin
CFLAGS += -Iinclude
CFLAGS += -I../../libc/include
CFLAGS += -D_NTOS_VERSION_="\"$(GIT_VERSION)\""

ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0 -DHAVE_DEBUG
	ASFLAGS = -g
endif

ARCH_C_SOURCES = $(shell find kernel -name '*.c')
ARCH_S_SOURCES = $(shell find kernel -name '*.S')
ARCH_C_OBJECTS = $(patsubst %.c,%.o,$(ARCH_C_SOURCES))
ARCH_S_OBJECTS = $(patsubst %.S,%.o,$(ARCH_S_SOURCES))
ARCH_OBJECTS = $(ARCH_C_OBJECTS) $(ARCH_S_OBJECTS)

.PHONY = clean

libi386.a: $(ARCH_OBJECTS)
	$(AR) r $@ $^

clean:
	rm -f $(ARCH_OBJECTS) libi386.a
