#include <kernel/cpu/idt.h>

// The page directory in use by the kernel itself.
static unsigned int kernel_page_directory[1024] __attribute__((aligned(4096)));

void
virtual_memory_init()
{
	unsigned int i;

	// Mark all pages as PS=1, but they are not present.
	for (i = 0; i < 512; i++) {
		kernel_page_directory[i] = (i << 22) | 0x83;
	}
	for (i = 512; i < 1024; i++) {
		kernel_page_directory[i] = ((i - 512) << 2) | 0x83;
	}
}

void
enable_paging()
{
	register unsigned int cr;

	// Load the memory address of the kernel page directory.
	__asm__("movl %0, %%cr3" : : "r"(kernel_page_directory));

	// Enable CR4 PSE
	__asm__("movl %%cr4, %0" : "=r"(cr));
	cr |= 0x10;
	__asm__("movl %0, %%cr4" : : "r"(cr));

	// Set pagination in CR0.
	__asm__("movl %%cr0, %0" : "=r"(cr));
	cr |= 0x80000001;
	__asm__("movl %0, %%cr0" : : "r"(cr));
}
