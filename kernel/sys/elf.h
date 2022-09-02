#pragma once

#include <stdint.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

// Some positions in the e_ident array, instead of hardcoding numbers
// we will use constants to make the code easier to understand by
// humans.
#define EI_NIDENT 16
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7

// The following constants exist to avoid hardcoding numbers in the
// header, to make the enum types more sound. So instead of just
// having 4, you know what 4 actually means.

// Positions EI_MAG0 up to EI_MAG3 of e_ident must have these values
// in the array, otherwise this is not a valid ELF file.
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

// Position EI_CLASS indicates whether the executable is 32 or 64 bit.
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

// Position EI_DATA indicates whether the executable uses LSB or MSB
// encoding. This is important to parse words that are more than one byte.
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// Position EI_VERSION indicates the ELF version in use.
#define EV_NONE 0
#define EV_CURRENT 1

// Type of executable file that we have. There are different kinds. Check the
// System V spec for the full reference, but here is the grasp:
// - If it is executable, this file can be executed by the operating system.
// - If it is relocatable, you can use this object code to link it.
// - If it is shared, it is a .so that you can use to build other relocatables
//   or executable files.
#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

// The machine type (processor). There are A LOT of processors, it would be
// impossible to list the list (or at least very long, so I am only adding
// those where I want NativeOS to run).
#define EM_NONE 0
#define EM_386 3
#define EM_AMD64 62
#define EM_ARM 40
#define EM_AARCH64 64

typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11

typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4

typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) &0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) &0 x f))
