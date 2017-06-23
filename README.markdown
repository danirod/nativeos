# NativeOS

NativeOS is a hobbyist kernel and operating system for the x86
platform. It is designed as a project to learn more about how the x86
processor work and how operating systems work. My intention is purely
educational. The source code is open in the hope that it helps other
people too.

## Building NativeOS

NativeOS requires the i386-elf-gcc cross-compiler installed in the
machine to compile. The build process has already been tested under
Linux, MacOS X and Windows and it works on all the platforms.

You must compile the i386-elf-binutils and i386-elf-gcc. Download
GNU Binutils and GNU GCC and compile them targetting the i386-elf
platform.

MS-Windows users should download and install a POSIX environment
such as Cygwin or MSYS2 in order to get all the tools that would
allow them to build the kernel image, such as make. Please note
that although MS-Windows is a supported host, it is not tested
so often, and a real UNIX or UNIX-like environment such as GNU/Linux
or MacOS X is recommended, even on a virtual machine.

You will also need the NASM compiler to compile the assembly source
code files. No port of the sources has been done to GAS at the moment
and I'm not actively expecting as I'm comfortable with the Intel
syntax, but I just don't care and I might just port the scripts to
GAS in the future to reduce the number of tools this project depends
on.

GNU Make is also required to run the Makefile file.

To build the kernel image just run the following command:

    $ make

This will build the kernel into "nativeos.elf" file. NativeOS kernel
uses the ELF format and it supports the Multiboot specification, so it
can be deployed in any machine that uses a Multiboot bootloader (such
as GRUB) and executed.

## Creating a CD image

You can use some of the GRUB Command Line Tools to generate ISO files.
There is already a rule in the Makefile file to generate a CD-ROM using
GRUB.

You need the GRUB Command Line Tools installed with support for the
i386-elf platform. If you are using GNU/Linux, you probably already
have support for that platform, but othwersie you might have to
add it. Instructions for building the GRUB Command Line Tools are given
below.

To create a NativeOS ISO image, run the following command:

    $ make cdrom

MacOS X and Windows users can install the GRUB Command Line tools by
following the instructions given in the following address:
<http://wiki.osdev.org/GRUB_2#Installing_GRUB2_on_Mac_OS_X>.
(Instructions are the same for Windows).

NOTE: You'll be getting the GRUB Command Line Tools here. **This is
definitely NOT the same as installing the GRUB Bootloader on your
host PC**, although the command line tools allow you to do that, so
care must be taken in order to avoid running any dangerous command.

Windows users can find a precompiled version of the GRUB Command Line
tools at <https://github.com/danirod/i386-elf-toolchain/releases>.
In any way you will need GNU xorriso to generate the ISO files.
(There is already a precompiled version of xorriso for Windows in
the link too.)

## Running NativeOS

A virtual machine is the safest way for running and testing NativeOS.
I don't recommend running this on your host PC unless you know what
you are doing and you have inspected the source code. Also, take a
look at the disclaimer below before attempting to run anything that
you could regret later.

The Makefile already has some rules to run the NativeOS ISO file with
QEMU. It is even possible to run QEMU in debug mode and to remotely
attach GDB in order to debug the kernel image.

Run the following commands:

    $ make qemu         # Executes qemu-system-i386
    $ make qemu-gdb     # Executes qemu-system-i386 in debug mode

For more information on GDB debugging with QEMU, see the QEMU
manual or the following article: <http://wiki.osdev.org/QEMU>.

## License and disclaimer

Copyright (C) 2015-2017 Dani Rodríguez

NativeOS is licensed under the terms of the GNU General Public License v3.
See the COPYING file for more information. Old NativeOS files will
probably not have the GPL header on it. I'm working on that.

NativeOS is provided AS IS with no extra support. Working with kernels
and OS and bootloaders is fun, but I'm not resposible for anything wrong
you do with it. If you break your hard drive or your computer because
of using the wrong command, that is not my fault.
