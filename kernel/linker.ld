ENTRY (Bootstrap)

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

	.bss :
	{
		sbss = .;
		*(COMMON)
		*(.bss)
		ebss = .;
	}
	
	kernel_after = .;
}
