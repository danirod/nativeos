/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2022 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

/**
 * \file
 * \brief Multiboot Data Structures
 *
 * This file defines the different structs associated with the buffers provided
 * by the Multiboot bootloader when the system starts. One of the first things
 * the kernel has to do is to preserve this pointer into a variable so that
 * we can process it later.
 */
#pragma once

/**
 * Describes a module. Multiboot bootloaders provide a way to load arbitrary
 * files into memory, known as modules. The operating system will receive a
 * list of modules.
 */
typedef struct multiboot_module {
	/** Physical memory address where the module starts. */
	unsigned int mod_start;
	/** Physical memory address where the module ends. */
	unsigned int mod_end;
	/** A string associated with the module (usually command line). */
	unsigned int string;
	unsigned int reserved;
} multiboot_module_t;

/**
 * Information about the section header table of the ELF kernel. This is the
 * executable itself as booted by the bootloader. Is useful in order to parse
 * the sections of the executable.
 */
typedef struct multiboot_elf {
	/** Number of sections in the section header table (e_shnum). */
	unsigned int num;
	/** Size of each section in the section header table (e_shentsize). */
	unsigned int size;
	/** Memory address of the section header table. */
	unsigned int addr;
	/** Index used for the string table (e_shstrndx). */
	unsigned int shndx;
} multiboot_elf_t;

/**
 * Computer memory map, as reported by the bootloader. Each memory slice
 * contains information about the type of kind of memory block this is.
 */
typedef struct multiboot_mmap {
	/** Size of this entry. */
	unsigned int size;
	/** Base address where the memory block starts. */
	unsigned long long base_addr;
	/** Length in bytes of the memory block. */
	unsigned long long length;
	/** Kind of memory block. 1 = RAM, 3 = ACPI, 5 = Defective. */
	unsigned int type;
} multiboot_mmap_t;

/* Information about the drives that are accessible on the computer. */
/**
 * A drive as reported by the bootloader. Note that the actual size of the
 * data structure is tricky to get because there is an entry per port.
 */
typedef struct multiboot_drive {
	/** Size of the structure */
	unsigned int size;
	/** Drive number as defined by the system. */
	unsigned char drive_number;
	/** Access mode used by this device. */
	unsigned char drive_mode;
	/** Number of cylinders. */
	unsigned char drive_cylinders;
	/** Number of heads. */
	unsigned short drive_heads;
	/** Number of sectors. */
	unsigned char drive_sectors;
	/** Zero-terminated array of ports for this drive. */
	unsigned short *drive_ports;
} multiboot_drive_t;

/** The device uses CHS addressing. */
#define MULTIBOOT_DRIVE_CHS 0
/** The device uses LBA addressing. */
#define MULTIBOOT_DRIVE_LBA 1

/**
 * Advanced Power Managemnt (APM) table. The spec is tricky to find because
 * Microsoft dropped the support for it long time ago, and they were the
 * creators of the spec. There is more information in the Wikipedia page:
 * https://en.wikipedia.org/wiki/Advanced_Power_Management.
 */
typedef struct multiboot_apm_table {
	/** Version number. */
	unsigned short version;
	/** 32 bit code segment. */
	unsigned short cseg;
	/** Entrypoint offset. */
	unsigned short offset;
	/** Protected mode 16 bit code segment. */
	unsigned short cseg_16;
	/** Protected mode 16 bit data segment. */
	unsigned short dseg;
	/** Flags. */
	unsigned short flags;
	/** Length of the 32 bit code segment. */
	unsigned short cseg_len;
	/** Length of the 16 bit code segment. */
	unsigned short cseg_16_len;
	/** Length of the 16 bit data segment. */
	unsigned short dseg_len;
} multiboot_apm_table_t;

/**
 * VBE info, if requested by the operating system. When VBE is requested and
 * a graphical mode is set by the bootloader, information about the graphical
 * mode set will be placed here.
 */
typedef struct multiboot_vbe_info {
	/** Memory address of the VBE control information */
	unsigned int vbe_control_info;
	/** VBE mode as returned by the system. */
	unsigned int vbe_mode_info;
	/** Video mode in the VBE 3.0 format. */
	unsigned short vbe_mode;
	/** VBE Protected Mode Interface segment. */
	unsigned short vbe_interface_segment;
	/** VBE Protected Mode Interface offset. */
	unsigned short vbe_interface_offset;
	/** VBE Protected Mode Interface length. */
	unsigned short vbe_interface_length;
} multiboot_vbe_info_t;

/**
 * Multiboot structure. A multiboot bootloader will provide during boot a
 * pointer to a memory buffer that contains a data structure using this format
 * so that the target operating system can get information about the system.
 */
typedef struct multiboot_info {
	/** Which features are supported by the OS loader (and version). */
	unsigned int flags;
	/** Lower bounds of the computer memory (flag bit 0). */
	unsigned int mem_lower;
	/** Upper bounds of the computer memory (flag bit 0). */
	unsigned int mem_upper;
	/** Attributes related to boot device (flag 1) */
	unsigned int boot_device;
	/** Attributes related to command line (flag 2) */
	unsigned int command_line;
	/** Number of modules (flag 3) */
	unsigned int mods_count;
	/** Memory address of the module data structure (flag 3). */
	unsigned int mods_addr;
	/** ELF section table information (flag 5). */
	multiboot_elf_t elf;
	/** Size of the bytearray holding the memory map info (flag 6). */
	unsigned int mmap_length;
	/** Memory address of the memory map info (flag 6). */
	unsigned int mmap_addr;
	/** Total number of drive information entries (flag 7). */
	unsigned int drives_length;
	/** Memory address of the first drive information entry (flag 7). */
	unsigned int drives_addr;
	/** Attributes related to the config table (flag 8). */
	unsigned int config_table;
	/** Attributes related to the boot laoder name (flag 9). */
	unsigned int boot_loader_name;
	/** Attributes related to the APM table (flag 10). */
	unsigned int apm_table;
	/** Attributes related to VBE data (flag 11). */
	struct multiboot_vbe_info vbe_info;
} multiboot_info_t;

/**
 * Global kernel multiboot information. On kernels built with support for
 * Multiboot, the kernel entrypoint will take this information from the
 * bootloader handoff and expose it through this variable.
 */
extern multiboot_info_t *multiboot_info;
