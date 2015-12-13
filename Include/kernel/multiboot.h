#ifndef KERNEL_MULTIBOOT_H_
#define KERNEL_MULTIBOOT_H_

/* This structure is used for getting information about a module. */
typedef struct multiboot_module {
	/* Mod starting and ending address. */
	unsigned int mod_start;
	unsigned int mod_end;
	
	/* String (usually command line). */
	unsigned int string;
	
	/* Reserved (must be zero ATM) */
	unsigned int reserved;
} __attribute__((packed)) multiboot_module_t;

/*
 * This structure has information about the symbol table on A.OUT kernels.
 * Probably won't ever be supported since NativeOS is an ELF kernel, but
 * it's good to know.
 */
typedef struct multiboot_aout {
	unsigned int tabsize; /* Size of the nlist array table. */
	unsigned int strsize; /* Size of the string array table. */
	unsigned int addr; /* Physical address of the A.OUT symbol table. */
	unsigned int reserved; /* Must be zeros. */
} __attribute__((packed)) multiboot_aout_t;

/*
 * This structure has information aobut the section header on ELF kernels.
 * Again, I don't expect to have to traverse this structure, but it's good
 * to have.
 */
typedef struct multiboot_elf {
	unsigned int num; /* How many entries are there in the header. */
	unsigned int size; /* The size of every entry in the header. */
	unsigned int addr; /* Where is the section header structure. */
	unsigned int shndx; /* String table used as the index of names. */
} __attribute__((packed))  multiboot_elf_t;

/*
 * Information about the computer memory map. The actual memory map could be
 * split in multiple blocks (maybe an empty slot in the computer or some
 * memory reserved for any other thing inside the computer...)
 */
typedef struct multiboot_mmap {
	unsigned int size; /* Size of the structure. */
	unsigned long long base_addr; /* Starting address of the memory block. */
	unsigned long long length; /* Size of the memory block in bytes. */
	unsigned int type; /* Which kind of memory block is this. (1 == RAM) */
} __attribute__((packed)) multiboot_mmap_t;

/* Information about the drives that are accessible on the computer. */
typedef struct multiboot_drive {
	unsigned int size; /* Size of the structure (might vary!) */
	unsigned char drive_number; /* Drive number as declared by BIOS. */
	unsigned char drive_mode; /* What kind of access mode the device has. */
	unsigned char drive_cylinders; /* How many cylinders the device has. */
	unsigned short drive_heads; /* How many heads the device has. */
	unsigned char drive_sectors; /* How many sectores the device has. */
	unsigned short *drive_ports; /* Drive ports array (ends with 00) */
} __attribute__((packed)) multiboot_drive_t;

#define MULTIBOOT_DRIVE_CHS 0 /* The device uses CHS addressing. */
#define MULTIBOOT_DRIVE_LBA 1 /* The device uses LBA addressing. */

/*
 * Advanced Power Management (APM) table. See the specs at the Microsoft site
 * or maybe at https://en.wikipedia.org/wiki/Advanced_Power_Management
 */
typedef struct multiboot_apm_table {
	unsigned short version; /* Version number. */
	unsigned short cseg; /* Protected mode 32 bit code segment. */
	unsigned short offset; /* Offset of entry point. */
	unsigned short cseg_16; /* Protected mode 16 bit code segment. */
	unsigned short dseg; /* Protected mode 16 bit data segment. */
	unsigned short flags; /* Flags. */
	unsigned short cseg_len; /* Length of protected mode 32 bit CS. */
	unsigned short cseg_16_len; /* Length of protected mode 16 bit CS. */
	unsigned short dseg_len; /* Length of protected mode 16 bit DS. */
} __attribute__((packed)) multiboot_apm_table_t;

/*
 * This is the actual multiboot structure that the boot loader gives my OS
 * when it loads and gives control to my kernel code. I can use this structure
 * to get information about the loader or the computer.
 */
typedef struct multiboot_info {
	/* Which features are supported by the OS loader (and version). */
	unsigned int flags;
	
	/* Attributes related to memory lower and upper bounds (flag 0) */
	unsigned int mem_lower;
	unsigned int mem_upper;
	
	/* Attributes related to boot device (flag 1) */
	unsigned int boot_device;
	
	/* Attributes related to command line (flag 2) */
	unsigned int command_line;
	
	/* Attributes related to modules (flag 3) */
	unsigned int mods_count;
	unsigned int mods_addr;
	
	/* Attributes related to the symbols (flags 4 or 5, but not both). */
	union {
		/* If flag 4 is ON, it is an A.OUT kernel. */
		multiboot_aout_t aout;
		
		/* Buf if flag 5 is ON, it is an ELF kernel. */
		multiboot_elf_t elf;
	} aout_elf;
	
	/* Attributes related to memory map (flag 6). */
	unsigned int mmap_length;
	unsigned int mmap_addr;
	
	/* Attirbutes related to the drives (flag 7). */
	unsigned int drives_length;
	unsigned int drives_addr;
	
	/* Attributes related to the config table (flag 8). */
	unsigned int config_table;
	
	/* Attributes related to the boot laoder name (flag 9). */
	unsigned int boot_loader_name;
	
	/* Attributes related to the APM table (flag 10). */
	unsigned int apm_table;
	
	/* Attributes related to VBE data (if requested by the OS, flag 11). */
	unsigned int vbe_control_info;
	unsigned int vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_segment;
	unsigned short vbe_interface_offset;
	unsigned short vbe_interface_length;
} __attribute__((packed)) multiboot_info_t;

#endif // KERNEL_MULTIBOOT_H_