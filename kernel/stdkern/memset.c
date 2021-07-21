#include <stddef.h>

void *
memset(void *dest, int byte, size_t count)
{
	unsigned char *ptr = (unsigned char *) dest;
	while (count--)
		*ptr++ = byte;
	return dest;
}