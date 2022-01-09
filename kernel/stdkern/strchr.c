char *
strchr(const char *s, int c)
{
	char *ptr = (char *) s;
	if (ptr) {
		while (*ptr) {
			if (*ptr == c) {
				return ptr;
			}
			ptr++;
		}
	}
	return 0;
}

char *
strrchr(const char *s, int c)
{
	char *ptr = (char *) s;
	char *target = 0;
	if (ptr) {
		while (*ptr) {
			if (*ptr == c) {
				target = ptr;
			}
			ptr++;
		}
	}
	return target;
}
