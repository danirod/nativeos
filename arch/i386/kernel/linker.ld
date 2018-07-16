ENTRY (Bootstrap)

/* Size of the kernel heap.  Note that the kernel heap is only used to allocate
 * kernel objects using kmalloc and kfree, and it's not shared with other
 * binary programs.  Size of the kernel heap is 1 MB, but feel free to increase
 * this value if the heap exhausts too often.  */
heap_size = 0x100000;

SECTIONS
{
	. = 0x00100000;
	kernel_start = .;

	.text ALIGN (0x1000) :
	{
		*(.text)
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
}
