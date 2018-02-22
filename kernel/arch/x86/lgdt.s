; This file is part of NativeOS
; Copyright (C) 2015-2018 The NativeOS contributors
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
