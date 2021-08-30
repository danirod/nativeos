#include <sys/stdkern.h>

static inline int
matches(char chr, char *delims)
{
	while (*delims) {
		if (chr == *delims)
			return 1;
		delims++;
	}
	return 0;
}

/*
 * This function does a lot of things.
 * - The original value pointed by strptr must be returned.
 * - The pointer in strptr must change to the location of the
 *   coincidence in the string.
 */
char *
strsep(char **strptr, const char *delimiter)
{
	char *delim, *strcur, *origstrptr;

	if (!strptr || !*strptr) {
		/* Shortcircuit to NULL if given strptr is NULL. */
		return 0;
	}

	strcur = *strptr;
	origstrptr = *strptr;
	while (*strcur) {
		delim = (char *) delimiter;
		while (*delim) {
			if (*delim == *strcur) {
				*strcur = 0;
				*strptr = (strcur + 1);
				return origstrptr;
			}
			delim++;
		}
		strcur++;
	}

	/* When nothing matched, it will return NULL via the pointer. */
	*strptr = 0;
	return origstrptr;
}
