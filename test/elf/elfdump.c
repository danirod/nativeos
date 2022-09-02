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

	printf("e_ident test is starting\n");

	// Testing EI_MAG
	if ((ident[EI_MAG0] != ELFMAG0) || (ident[EI_MAG1] != ELFMAG1)
	    || (ident[EI_MAG2] != ELFMAG2) || (ident[EI_MAG3] != ELFMAG3)) {
		printf("[fail] EI_MAG invalid\n");
		printf("       Stopping eident checking for safety reason\n");
		return -1;
	}
	printf("[ ok ] EI_MAG = <0x7f> E L F\n");

	if (ident[EI_CLASS] == ELFCLASS32)
		printf("[ ok ] EI_CLASS is 32 bit\n");
	else if (ident[EI_CLASS] == ELFCLASS64)
		printf("[ ok ] EI_CLASS is 64 bit\n");
	else
		printf("[fail] EI_CLASS is invalid: %d\n", ident[EI_CLASS]);

	if (ident[EI_DATA] == ELFDATA2LSB)
		printf("[ ok ] EI_DATA is ELFDATA2LSB\n");
	else if (ident[EI_DATA] == ELFDATA2MSB)
		printf("[ ok ] EI_DATA is ELFDATA2MSB\n");
	else
		printf("[fail] EI_DATA is invalid: %d\n", ident[EI_DATA]);

	if (ident[EI_VERSION] == EV_CURRENT)
		printf("[ ok ] EI_VERSION is EV_CURRENT\n");
	else
		printf("[fail] EI_VERSION is invalid: %d\n", ident[EI_VERSION]);

	printf("e_ident test is done\n");
	return 0;
}

void
test_ehdr()
{
	printf("ehdr test is starting\n");

	if (ehdr->e_type == ET_REL)
		printf("[ ok ] e_type is ET_REL\n");
	else if (ehdr->e_type == ET_EXEC)
		printf("[ ok ] e_type is ET_EXEC\n");
	else if (ehdr->e_type == ET_DYN)
		printf("[ ok ] e_type is ET_DYN\n");
	else if (ehdr->e_type == ET_CORE)
		printf("[ ok ] e_type is ET_CORE\n");
	else
		printf("[fail] e_type is unknown: %d\n", ehdr->e_type);

	if (ehdr->e_machine == EM_NONE)
		printf("[fail] e_machine is NONE\n");
	else if (ehdr->e_machine == EM_386)
		printf("[ ok ] e_machine is 386\n");
	else if (ehdr->e_machine == EM_AMD64)
		printf("[ ok ] e_machine is AMD64\n");
	else if (ehdr->e_machine == EM_ARM)
		printf("[ ok ] e_machine is ARM\n");
	else if (ehdr->e_machine == EM_AARCH64)
		printf("[ ok ] e_machine is AARCH64\n");
	else
		printf("[fail] e_machine is not registered: %d",
		       ehdr->e_machine);

	if (ehdr->e_version == EV_CURRENT)
		printf("[ ok ] EI_VERSION is EV_CURRENT\n");
	else
		printf("[fail] EI_VERSION is invalid: %d\n", ehdr->e_version);

	printf("[info] e_entry is %x \n", ehdr->e_entry);
	printf("[info] e_phoff is %x \n", ehdr->e_phoff);
	printf("[info] e_shoff is %x \n", ehdr->e_shoff);
	printf("[info] e_flags is %x \n", ehdr->e_flags);
	printf("[info] e_ehsize is %d \n", ehdr->e_ehsize);
	printf("[info] e_phentsize is %d \n", ehdr->e_phentsize);
	printf("[info] e_phnum is %d \n", ehdr->e_phnum);
	printf("[info] e_shentsize is %d \n", ehdr->e_shentsize);
	printf("[info] e_shnum is %d \n", ehdr->e_shnum);
	printf("[info] e_shstrndx is %d \n", ehdr->e_shstrndx);

	printf("ehdr test is done\n");
}

#define SECTION_TYPE(value) \
	case value: \
		printf(#value); \
		return;

void
dump_section_type(Elf32_Shdr *section)
{
	switch (section->sh_type) {
		SECTION_TYPE(SHT_NULL);
		SECTION_TYPE(SHT_PROGBITS);
		SECTION_TYPE(SHT_SYMTAB);
		SECTION_TYPE(SHT_STRTAB);
		SECTION_TYPE(SHT_RELA);
		SECTION_TYPE(SHT_HASH);
		SECTION_TYPE(SHT_DYNAMIC);
		SECTION_TYPE(SHT_NOTE);
		SECTION_TYPE(SHT_NOBITS);
		SECTION_TYPE(SHT_REL);
		SECTION_TYPE(SHT_SHLIB);
		SECTION_TYPE(SHT_DYNSYM);
	}
}

void
dump_section_table_entry(Elf32_Shdr *section)
{
	printf("section offset = %x, size = %x, type = %x \n",
	       section->sh_offset,
	       section->sh_size,
	       section->sh_type);

	printf("  Name: %s\n", shstrtab + section->sh_name);
	printf("  Type: ");
	dump_section_type(section);
	printf("\n");

	printf("  Flags: ");
	if (section->sh_flags & SHF_WRITE)
		printf("SHF_WRITE ");
	if (section->sh_flags & SHF_ALLOC)
		printf("SHF_ALLOC ");
	if (section->sh_flags & SHF_EXECINSTR)
		printf("SHF_EXECINSTR ");
	printf("\n");
	puts("===");
}

void
dump_section_table()
{
	puts("section table start");
	for (int i = 0; i < ehdr->e_shnum; i++) {
		dump_section_table_entry(&shtable[i]);
	}
	puts("section table done");
	puts("===");
}

void
breakpoint()
{
}

void
dump_symbol_table(Elf32_Shdr *section)
{
	Elf32_Sym *symtab = (Elf32_Sym *) ((char *) ehdr + section->sh_offset);
	Elf32_Shdr *strings = &shtable[section->sh_link];
	char *stringstab = (char *) ehdr + strings->sh_offset;
	int i, symno = section->sh_size / sizeof(Elf32_Sym);

	for (i = 0; i < symno; i++) {
		breakpoint();
		printf("%d ", i);

		printf("%s\n", stringstab + symtab[i].st_name);
		printf("  size=%d ", symtab[i].st_size);
		printf("  value=%x ", symtab[i].st_value);
		printf("\n");
	}
}

void
dump_symbol_tables()
{
	puts("symbol table list start");
	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (shtable[i].sh_type != SHT_SYMTAB) {
			continue;
		}
		dump_symbol_table(&shtable[i]);
	}
	puts("symbol table list end");
	puts("===");
}

void
dump_elf()
{
	if (test_eident() < 0) {
		puts("Stopping dump for safety reasons");
		return;
	}
	test_ehdr();
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
