; This file is part of NativeOS
; Copyright (C) 2015 Dani Rodríguez
;
; gdt.s - actually set up the GDT table.

global gdt_load
extern gdt_toc;

; This function receives as an argument the GDT table that has to be loaded.
gdt_load:
	; Extract the table from the stack and set the register values to this.
	lgdt [gdt_toc]

	; Start flushing the segment registers for data segments.
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Do a far jump to flush the code register.
	jmp 0x08:update_registers

update_registers:
	; That's it. We actually just do the jump for changing the CS value.
	ret
