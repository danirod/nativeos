#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../kernel/sys/elf.h"

Elf32_Ehdr *ehdr; // The ELF file that we are processing
Elf32_Shdr *shtable; // The section table for quick acccess
Elf32_Shdr *shstr; // The symbol table for the section table
char *shstrtab; // The strings in the symbol table for section

int
test_eident()
{
	unsigned char *ident = ehdr->e_ident;

	// Testing EI_MAG
	if ((ident[EI_MAG0] != ELFMAG0) || (ident[EI_MAG1] != ELFMAG1)
	    || (ident[EI_MAG2] != ELFMAG2) || (ident[EI_MAG3] != ELFMAG3)) {
		printf("EI_MAG invalid\n");
		printf("Stopping eident checking for safety reason\n");
		return -1;
	}
	return 0;
}

#define DUMP_START(variable, bind) \
	do { \
		int bind = variable; \
		switch (variable)
#define DUMP_END() \
	} \
	while (0) \
		;
#define DUMP_UNKNOWN(value) printf("Unknown: %x", value)
#define DUMP_CASE(value, str) \
	case value: \
		printf(str); \
		break;

void
dump_ehdr()
{
	printf("ELF Header:\n");

	printf("Magic:");
	for (int i = 0; i < 16; i++)
		printf(" %02x", ehdr->e_ident[i]);
	printf("\n");

	printf("Class: ");
	DUMP_START(ehdr->e_ident[EI_CLASS], i)
	{
		DUMP_CASE(ELFCLASS32, "ELF32");
		DUMP_CASE(ELFCLASS64, "ELF64");
		DUMP_UNKNOWN(i);
	}
	DUMP_END();
	printf("\n");

	printf("Data: 2's complement, ");
	DUMP_START(ehdr->e_ident[EI_DATA], i)
	{
		DUMP_CASE(ELFDATA2LSB, "little endian");
		DUMP_CASE(ELFDATA2MSB, "big endian");
		DUMP_UNKNOWN(i);
	}
	DUMP_END();
	printf("\n");

	printf("Version: %d", ehdr->e_ident[EI_VERSION]);
	if (ehdr->e_ident[EI_VERSION] == 1) {
		printf(" (current)");
	}
	printf("\n");

	printf("OS/ABI: ");
	DUMP_START(ehdr->e_ident[EI_OSABI], i)
	{
		DUMP_CASE(ELFOSABI_NONE, "UNIX - System V");
		DUMP_CASE(ELFOSABI_NETBSD, "UNIX - NetBSD");
		DUMP_CASE(ELFOSABI_GNU, "UNIX - GNU");
		DUMP_CASE(ELFOSABI_FREEBSD, "UNIX - FreeBSD");
		DUMP_CASE(ELFOSABI_OPENBSD, "UNIX - OpenBSD");
		DUMP_UNKNOWN(i);
	}
	DUMP_END();
	printf("\n");

	printf("ABI Version: %d\n", ehdr->e_ident[EI_ABIVERSION]);

	printf("Type: ");
	DUMP_START(ehdr->e_type, i)
	{
		DUMP_CASE(ET_REL, "REL (Relocatable file)");
		DUMP_CASE(ET_EXEC, "EXEC (Executable file)");
		DUMP_CASE(ET_DYN, "DYN (Shared Object File)");
		DUMP_CASE(ET_CORE, "CORE (Core file)");
		DUMP_UNKNOWN(i);
	}
	DUMP_END();
	printf("\n");

	printf("Machine: ");
	DUMP_START(ehdr->e_machine, i)
	{
		DUMP_CASE(EM_386, "Intel 80386");
		DUMP_CASE(EM_AMD64, "Advanced Micro Devices X86-64");
		DUMP_CASE(EM_ARM, "ARM");
		DUMP_CASE(EM_AARCH64, "AArch64");
		DUMP_UNKNOWN(i);
	}
	DUMP_END();
	printf("\n");

	printf("Version: 0x%x\n", ehdr->e_version);
	printf("Entry point address: 0x%x\n", ehdr->e_entry);
	printf("Start of program headers: %d (bytes into file)\n",
	       ehdr->e_phoff);
	printf("Start of section headers: %d (bytes into file)\n",
	       ehdr->e_shoff);
	printf("Flags: 0x%x\n", ehdr->e_flags);
	printf("Size of this header: %d (bytes)\n", ehdr->e_ehsize);
	printf("Size of program headers: %d (bytes)\n", ehdr->e_phentsize);
	printf("Number of program headers: %d\n", ehdr->e_phnum);
	printf("Size of section headers: %d (bytes)\n", ehdr->e_shentsize);
	printf("Number of section headers: %d\n", ehdr->e_shnum);
	printf("Section header string table index: %d\n", ehdr->e_shstrndx);
}

#define SECTION_TYPE(value, text) \
	case value: \
		printf(text); \
		return;

void
dump_section_type(Elf32_Shdr *section)
{
	switch (section->sh_type) {
		SECTION_TYPE(SHT_NULL, "NULL");
		SECTION_TYPE(SHT_PROGBITS, "PROGBITS");
		SECTION_TYPE(SHT_SYMTAB, "SYMTAB");
		SECTION_TYPE(SHT_STRTAB, "STRTAB");
		SECTION_TYPE(SHT_RELA, "RELA");
		SECTION_TYPE(SHT_HASH, "HASH");
		SECTION_TYPE(SHT_DYNAMIC, "DYNAMIC");
		SECTION_TYPE(SHT_NOTE, "NOTE");
		SECTION_TYPE(SHT_NOBITS, "NOBITS");
		SECTION_TYPE(SHT_REL, "REL");
		SECTION_TYPE(SHT_SHLIB, "SHLIB");
		SECTION_TYPE(SHT_DYNSYM, "DYNSYM");
	}
}

#define DUMP_SECTIONFLAG(flags, value, letter) \
	if ((flags & value) != 0) \
		printf(letter);

void
print_section_flags(Elf32_Word flags)
{
	DUMP_SECTIONFLAG(flags, SHF_WRITE, "W");
	DUMP_SECTIONFLAG(flags, SHF_ALLOC, "A");
	DUMP_SECTIONFLAG(flags, SHF_EXECINSTR, "X");
	DUMP_SECTIONFLAG(flags, SHF_MERGE, "M");
	DUMP_SECTIONFLAG(flags, SHF_STRINGS, "S");
	DUMP_SECTIONFLAG(flags, SHF_INFO_LINK, "I");
	DUMP_SECTIONFLAG(flags, SHF_LINK_ORDER, "L");
	DUMP_SECTIONFLAG(flags, SHF_OS_NONCONFIRMING, "O");
	DUMP_SECTIONFLAG(flags, SHF_GROUP, "G");
	DUMP_SECTIONFLAG(flags, SHF_TLS, "T");
	DUMP_SECTIONFLAG(flags, SHF_COMPRESSED, "C");
}

void
dump_section_table_entry(int i, Elf32_Shdr *section)
{
	printf("[%d] ", i);
	printf("%s", shstrtab + section->sh_name);
	if (*(shstrtab + section->sh_name)) {
		printf(" ");
	}
	dump_section_type(section);
	printf(" %08x %06x %06x %02x",
	       section->sh_addr,
	       section->sh_offset,
	       section->sh_size,
	       section->sh_entsize);
	if (section->sh_flags) {
		printf(" ");
		print_section_flags(section->sh_flags);
	}
	printf(" %d %d %d\n",
	       section->sh_link,
	       section->sh_info,
	       section->sh_addralign);
}

void
dump_section_table()
{
	printf("Section Headers:\n");
	printf("[Nr] Name Type Addr Off Size ES Flg Lk Inf Al\n");
	for (int i = 0; i < ehdr->e_shnum; i++) {
		dump_section_table_entry(i, &shtable[i]);
	}
	printf("Key to Flags:\n");
	printf("W (write), A (alloc), X (execute), M (merge), S (strings), I "
	       "(info),\n");
	printf("L (link order), O (extra OS processing required), G (group), T "
	       "(TLS),\n");
	printf("C (compressed), x (unknown), o (OS specific), E (exclude),\n");
	printf("D (mbind), p (processor specific)\n");
}

void
breakpoint()
{
}

void
dump_symbol_table(Elf32_Shdr *section)
{
	char *name = shstrtab + section->sh_name;
	int count = section->sh_size / sizeof(Elf32_Sym);
	printf("Symbol table '%s' contains %d entries:\n", name, count);

	Elf32_Sym *symtab = (Elf32_Sym *) ((char *) ehdr + section->sh_offset);
	Elf32_Shdr *strings = &shtable[section->sh_link];
	char *stringstab = (char *) ehdr + strings->sh_offset;

	puts("Num: Value Size Type Bind Vis Ndx Name");
	for (int i = 0; i < count; i++) {
		Elf32_Sym *s = &symtab[i];
		printf("%d: %08x %d ", i, s->st_value, s->st_size);

		DUMP_START(ELF32_ST_TYPE(s->st_info), i)
		{
			DUMP_CASE(STT_NOTYPE, "NOTYPE");
			DUMP_CASE(STT_OBJECT, "OBJECT");
			DUMP_CASE(STT_FUNC, "FUNC");
			DUMP_CASE(STT_SECTION, "SECTION");
			DUMP_CASE(STT_FILE, "FILE");
			DUMP_CASE(STT_COMMON, "COMMON");
			DUMP_CASE(STT_TLS, "TLS");
			DUMP_UNKNOWN(i);
		}
		DUMP_END();

		printf(" ");

		DUMP_START(ELF32_ST_BIND(s->st_info), i)
		{
			DUMP_CASE(STB_GLOBAL, "GLOBAL");
			DUMP_CASE(STB_LOCAL, "LOCAL");
			DUMP_CASE(STB_WEAK, "WEAK");
			DUMP_UNKNOWN(i);
		}
		DUMP_END();

		printf(" ");

		DUMP_START(ELF32_ST_VISIBILITY(s->st_other), i)
		{
			DUMP_CASE(STV_HIDDEN, "HIDDEN");
			DUMP_CASE(STV_DEFAULT, "DEFAULT");
			DUMP_CASE(STV_INTERNAL, "INTERNAL");
			DUMP_UNKNOWN(i);
		}
		DUMP_END();

		printf(" ");

		if (s->st_shndx == 0) {
			printf("UND");
		} else if (ELF32_ST_TYPE(s->st_info) == STT_FILE) {
			printf("ABS");
		} else {
			printf("%d", s->st_shndx);
		}

		if (ELF32_ST_TYPE(s->st_info) == STT_SECTION) {
			Elf32_Shdr *target = &shtable[s->st_shndx];
			char *sname = shstrtab + target->sh_name;
			printf(" %s", sname);
		} else {
			printf(" %s", &stringstab[s->st_name]);
		}
		printf("\n");
	}
}

void
dump_symbol_tables()
{
	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (shtable[i].sh_type != SHT_SYMTAB) {
			continue;
		}
		if (i > 0)
			puts("");
		dump_symbol_table(&shtable[i]);
	}
}

void
dump_elf()
{
	if (test_eident() < 0) {
		puts("Stopping dump for safety reasons");
		return;
	}
	dump_ehdr();
	printf("\n");
	dump_section_table();
	dump_symbol_tables();
}

void
load_global_context(char *data)
{
	// Put the ELF file in the global context.
	char *shtable_addr;
	ehdr = (Elf32_Ehdr *) data;

	// Extract the section table into the global variable.
	shtable_addr = data + ehdr->e_shoff;
	shtable = (Elf32_Shdr *) shtable_addr;

	// Extract the symbol table for the section table.
	shstr = &shtable[ehdr->e_shstrndx];
	shstrtab = (char *) ehdr + shstr->sh_offset;
}

int
main(int argc, char **argv)
{
	int fd, error;
	struct stat fdbuf;
	char *data;

	if (argc != 2) {
		printf("Usage: %s [file]\n", argv[0]);
		exit(1);
	}
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		puts("Cannot open");
		exit(1);
	}
	if ((error = fstat(fd, &fdbuf)) < 0) {
		puts("Cannot stat");
		close(fd);
		exit(1);
	}
	data = mmap(NULL, fdbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);

	load_global_context(data);
	dump_elf();

	if ((error = munmap(data, fdbuf.st_size)) != 0) {
		printf("Cannot mmunmap, but who cares");
	}
	close(fd);
}
