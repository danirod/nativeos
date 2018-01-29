#include <arch/x86/idt.h>
#include <arch/x86/io.h>
#include <driver/vga.h>

static void keyboard_int_handler(struct idt_data* idt)
{
    int scancode = IO_InP(0x60);
    VGACon_PutChar('?');
}

void keyboard_init()
{
    idt_set_handler(0x21, &keyboard_int_handler);
}
