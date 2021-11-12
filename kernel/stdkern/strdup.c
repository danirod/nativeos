#include <sys/stdkern.h>

char *strdup(const char *s)
{
	unsigned int nchar;
	char *copystr;

	nchar = strlen(s) + 1;
	copystr = malloc(nchar);
	if (copystr) {
		strcpy(copystr, s);
	}
	return copystr;
}
