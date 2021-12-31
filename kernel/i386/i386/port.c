#include <stdint.h>

void
port_out_byte(uint16_t port, uint8_t value)
{
	__asm__("outb %0, %1" : : "a"(value), "d"(port));
}

void
port_out_word(uint16_t port, uint16_t value)
{
	__asm__("outw %0, %1" : : "a"(value), "d"(port));
}

void
port_out_long(uint16_t port, uint32_t value)
{
	__asm__("outl %0, %1" : : "a"(value), "d"(port));
}

uint8_t
port_in_byte(uint16_t port)
{
	uint8_t value;
	__asm__("inb %1, %0" : "=a"(value) : "d"(port));
	return value;
}

uint16_t
port_in_word(uint16_t port)
{
	uint16_t value;
	__asm__("inw %1, %0" : "=a"(value) : "d"(port));
	return value;
}

uint32_t
port_in_long(uint16_t port)
{
	uint32_t value;
	__asm__("inl %1, %0" : "=a"(value) : "d"(port));
	return value;
}