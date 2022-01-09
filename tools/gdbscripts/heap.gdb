# This file defines macros that make testing the heap a little bit easier.

# dump-heap-block $block
#  Prints to the debugger console information about the given block. The
#  block must be a pointer of type heap_block_t * (defined in heap.c).
#  Will print in a single line information and memory addresses.
#  Format: "heap block 0x[mem] [used/free] [ok/invl] [size] [start] [end]"
define dump-heap-block
	# Print memory address of the block
	printf "heap block 0x%08x", (int) $arg0

	# Print status of the block
	if $arg0->status == 0x40404040
		printf " free"
	else
		printf " used"
	end

	# Test the magic number (to detect corruption)
	if $arg0->magic == 0x51514949
		printf "  ok "
	else
		printf " INVL"
	end

	# Print memory region start and end address
	set $start = ((int) $arg0) + sizeof(heap_block_t)
	set $end = $start + $arg0->size
	printf " 0x%08x < 0x%08x", $start, $end

	# Print size
	printf ", %d bytes\n", $arg0->size
end

# dump-heap
#  Prints all the current slices present in the heap linked list.
define dump-heap
	set $block = heap_root
	while $block != 0
		dump-heap-block $block
		set $block = (heap_block_t *) $block->next
	end
end
