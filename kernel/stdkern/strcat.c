/**
 * \file
 * \brief Implementation of strcat and strncat
 */

#include <sys/stdkern.h>

char *
strcat(char *s1, const char *s2)
{
	char *ptr1 = s1;
	char *ptr2 = (char *) s2;

	if (ptr1) {
		/* Make ptr1 point to the \0 at the end of s1. */
		while (*ptr1)
			ptr1++;
		if (ptr2) {
			while (*ptr2) {
				*ptr1++ = *ptr2++;
			}
			*ptr1++ = '\0';
		}
	}
	return s1;
}

char *
strncat(char *s1, const char *s2, size_t count)
{
	char *ptr1 = s1;
	char *ptr2 = (char *) s2;
	if (ptr1) {
		/* Make ptr1 point to the \0 at the end of s1. */
		while (*ptr1)
			ptr1++;
		if (ptr2 && count) {
			while (count-- && *ptr2) {
				*ptr1++ = *ptr2++;
			}
			*ptr1++ = '\0';
		}
	}
	return s1;
}
