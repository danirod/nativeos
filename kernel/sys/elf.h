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
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9

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

// OS ABI. I do not care about this a lot, but let's keep it minimum
#define ELFOSABI_NONE 0
#define ELFOSABI_NETBSD 2
#define ELFOSABI_GNU 3
#define ELFOSABI_LINUX 3
#define ELFOSABI_FREEBSD 9
#define ELFOSABI_OPENBSD 12

// The machine type (processor). There are A LOT of processors, it would be
// impossible to list the list (or at least very long, so I am only adding
// those where I want NativeOS to run).
#define EM_NONE 0
#define EM_386 3
#define EM_AMD64 62
#define EM_ARM 40
#define EM_AARCH64 183

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
#define SHF_MERGE 0x10
#define SHF_STRINGS 0x20
#define SHF_INFO_LINK 0x40
#define SHF_LINK_ORDER 0x80
#define SHF_OS_NONCONFIRMING 0x100
#define SHF_GROUP 0x200
#define SHF_TLS 0x400
#define SHF_COMPRESSED 0x800

typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

// The _info field of the Elf32_Sym data structure is actually a bitmask for
// other two subfields that are parsed and reconstructed using these
// macros.
#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) &0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) &0 x f))

// Symbol binding.
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

// Symbol type.
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_COMMON 5
#define STT_TLS 6

// _other field has multiple purposes, so use these bitmask to pick
#define ELF32_ST_VISIBILITY(o) ((o) &0x3)

// Symbol visibility.
#define STV_DEFAULT 0
#define STV_INTERNAL 1
#define STV_HIDDEN 2
#define STV_PROTECTED 3

typedef struct {
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6fffffff
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

#define PF_X 1
#define PF_W 2
#define PF_R 4
#define PF_MASKOS 0x0ff00000
#define PF_MASKPROC 0xf0000000
#define PT_GNU_EH_FRAME (PT_LOOS + 0x474e550)
#define PT_SUNW_EH_FRAME PT_GNU_EH_FRAME
#define PT_GNU_STACK (PT_LOOS + 0x474e551)
#define PT_GNU_RELRO (PT_LOOS + 0x474e552)
#define PT_GNU_PROPERTY (PT_LOOS + 0x474e553)
