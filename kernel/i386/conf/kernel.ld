OUTPUT_ARCH(i386);
OUTPUT_FORMAT(elf32-i386);

ENTRY (kernel_bootstrap)

/*
 * Size of the kernel heap, used to allocate kernel objects. This is not the
 * allocator that will be used by the userland. This value should be set to
 * something big enough to let the kernel do its work.
 */
heap_size = 0x100000;

SECTIONS
{
	. = 0x00100000;
	kernel_start = .;

	.multiboot ALIGN(0x1000) :
	{
		KEEP(*(.multiboot))
	}

	.text : ALIGN(0x1000)
	{
		*(.text)
	}

	.text.driver : ALIGN(0x1000)
	{
		devices_start = .;
		*(.text.driver)
		devices_end = .;
	}

	.text.fs : ALIGN(0x1000)
	{
		fs_descriptor__start = .;
		*(.text.fs);
		fs_descriptor__end = .;
	}

	.note.gnu.build-id : ALIGN(0x1000)
	{
		*(.note.gnu.build-id)
	}

	.rodata ALIGN (0x1000) :
	{
		*(.rodata*)
	}

	.data ALIGN (0x1000) :
	{
		*(.data)
	}

	.heap ALIGN (0x1000) :
	{
		heap_bottom = .;
		. = . + heap_size;
		heap_top = .;
	}

	.bss :
	{
		sbss = .;
		*(COMMON)
		*(.bss)
		ebss = .;
	}

	kernel_after = .;

	/DISCARD/ :
	{
		*(.interp)
	}
}
