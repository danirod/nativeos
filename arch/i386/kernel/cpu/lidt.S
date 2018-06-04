/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.text
.align 4

    .global idt_load
    idt_load:
        lidt (idt_toc)
        sti
        ret

/* Non erratic IDT handlers. */
.macro NON_ERROR_INTERRUPT num
    .global isr_\num
    isr_\num:
        pushl $0
        pushl $\num
        jmp idt_common_prehandler
.endm

/* Error IDT handlers. */
.macro ERROR_INTERRUPT num
    .global isr_\num
    isr_\num:
        pushl $\num
        jmp idt_common_prehandler
.endm

/* IDT declarations. */
NON_ERROR_INTERRUPT 0
NON_ERROR_INTERRUPT 1
NON_ERROR_INTERRUPT 2
NON_ERROR_INTERRUPT 3
NON_ERROR_INTERRUPT 4
NON_ERROR_INTERRUPT 5
NON_ERROR_INTERRUPT 6
NON_ERROR_INTERRUPT 7
ERROR_INTERRUPT 8
NON_ERROR_INTERRUPT 9
ERROR_INTERRUPT 10
ERROR_INTERRUPT 11
ERROR_INTERRUPT 12
ERROR_INTERRUPT 13
ERROR_INTERRUPT 14
NON_ERROR_INTERRUPT 15
NON_ERROR_INTERRUPT 16
NON_ERROR_INTERRUPT 17
NON_ERROR_INTERRUPT 18
NON_ERROR_INTERRUPT 19
NON_ERROR_INTERRUPT 20
NON_ERROR_INTERRUPT 21
NON_ERROR_INTERRUPT 22
NON_ERROR_INTERRUPT 23
NON_ERROR_INTERRUPT 24
NON_ERROR_INTERRUPT 25
NON_ERROR_INTERRUPT 26
NON_ERROR_INTERRUPT 27
NON_ERROR_INTERRUPT 28
NON_ERROR_INTERRUPT 29
NON_ERROR_INTERRUPT 30
NON_ERROR_INTERRUPT 31
NON_ERROR_INTERRUPT 32
NON_ERROR_INTERRUPT 33
NON_ERROR_INTERRUPT 34
NON_ERROR_INTERRUPT 35
NON_ERROR_INTERRUPT 36
NON_ERROR_INTERRUPT 37
NON_ERROR_INTERRUPT 38
NON_ERROR_INTERRUPT 39
NON_ERROR_INTERRUPT 40
NON_ERROR_INTERRUPT 41
NON_ERROR_INTERRUPT 42
NON_ERROR_INTERRUPT 43
NON_ERROR_INTERRUPT 44
NON_ERROR_INTERRUPT 45
NON_ERROR_INTERRUPT 46
NON_ERROR_INTERRUPT 47

    .global idt_common_prehandler
    idt_common_prehandler:
        pusha

/* Save ESP in the stack so we can access other registers. */
        mov %esp, %eax
        push %eax

        call idt_handler

/* Remove ESP */
        pop %eax

        popa

/* Removes error code and interrupt routine number. */
        add $8, %esp
        iret
