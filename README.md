# NativeOS

NativeOS is a hobbyist kernel and operating system for the x86
platform. It is designed as a project to learn more about how the x86
processor work and how operating systems work. My intention is purely
educational. The source code is open in the hope that it helps other
people too.

## Roadmap and contributing to the project

**This project has no public roadmap**. This means that I'm not disclosing
what I'm working on when I'm working on this project. This is intended at
this moment for this project because the project is not mature or stable
enough and is still under development.

My issue and pull request policy is clarified on [CONTRIBUTING.md][1]. I'm not
accepting pull requests at this moment. See the document to read why.
Issues are still open and covered by the Code of Conduct. See
[CODE\_OF\_CONDUCT.md][2] for details.

## Building the NativeOS Kernel

To build the kernel image (in other words, the program that will run
when the computer starts up), you will need to install the LLVM compiler
if you don't have it already. NativeOS switched to LLVM because it has
support for foreign ABIs (such as i386-elf in an x86-64 macOS) out of
the box without requiring to setup a cross-compiler.

GNU Make is also required to run the Makefile file.

Python 3 is also required to run the kcons tool, which is used to
generate the Makefiles.

Windows NT is not a supported platform and there may be bugs. Users
of Windows NT are encouraged to setup a POSIX system such as MSYS or
Cygwin, or use the Windows Subsystem for Linux if available.

### Picking a profile

Profiles are kernel compilation settings. A profile causes a specific
subset of source code files to be compiled into the kernel image, and
some specific optional settings to be enabled.

At the moment, the repository provides the I386 profile, to build a
basic 32 bit kernel image for the i386 architecture. The settings file
for this profile is at conf/I386.

### Building a profile

To build a profile, you can use the kcons script. kcons will parse the
profile and the associated files and generate a Makefile that can be
used to build the kernel. Call `tools/kcons` providing the path to a
profile file as a parameter.

    $ tools/kcons conf/I386

This will generate a subdirectory named after the profile name inside
the `compile/` directory. In this directory, a Makefile will be present.
Use it to build the kernel image:

    $ cd compile/I386
    $ make

The NativeOS kernel file is a binary in the ELF format. It supports the
Multiboot specification, so it can be deployed in any machine using a
bootloader capable of loading Multiboot binaries, such as GNU GRUB,
SYSLINUX or Limine.

### Shortcut

It is also possible to configure and build a kernel image profile using
the root Makefile. Provide the profile name as the `PROFILE` parameter
and it will run all the commands above.

    $ make build-kernel PROFILE=I386

## Creating a CD image

You can use some of the GRUB Command Line Tools to generate ISO files.
There is already a rule in the Makefile file to generate a CD-ROM using
GRUB.

You need the GRUB Command Line Tools installed with support for the
i386-elf platform. If you are using GNU/Linux, you probably already have
support for that platform, but otherwise you might have to install it to
your system. Take protections to prevent GRUB to running on your host
system if that is not practical for you. The `grub-mkrescue` program
should be able to run in your command line.

GNU xorriso is additionally required to generate the ISO file.

To create a NativeOS ISO image, run the following command, specifying
the name of a **previously built** profile as the `PROFILE` variable:

    $ make cdrom PROFILE=I386

Windows NT, macOS and BSD users can install the GRUB Command Line tools
by following the instructions given in the following address:
<http://wiki.osdev.org/GRUB_2#Installing_GRUB2_on_Mac_OS_X>.
(Instructions are the same for Windows).

NOTE: You'll be getting the GRUB Command Line Tools here. **This is
definitely NOT the same as installing the GRUB Bootloader on your host
PC**, although the command line tools allow you to do that, so care must
be taken in order to avoid running any dangerous command.

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

    $ make qemu PROFILE=I386      # Run qemu-system-i386
    $ make qemu-gdb PROFILE=I386  # Run qemu-system-i386 in debug mode

For more information on GDB debugging with QEMU, see the QEMU
manual or the following article: <http://wiki.osdev.org/QEMU>.

## License and disclaimer

Copyright (C) 2015-2021 Dani Rodríguez

NativeOS is licensed under the terms of the GNU General Public License v3.
See the COPYING file for more information. Old NativeOS files will
probably not have the GPL header on it. I'm working on that.

NativeOS is provided AS IS with no extra support. Working with kernels
and OS and bootloaders is fun, but I'm not resposible for anything wrong
you do with it. If you break your hard drive or your computer because
of using the wrong command, that is not my fault.

[1]: https://github.com/danirod/nativeos/blob/master/CONTRIBUTING.md
[2]: https://github.com/danirod/nativeos/blob/master/CODE_OF_CONDUCT.md
