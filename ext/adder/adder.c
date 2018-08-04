#include <string.h>

int add(int a, int b) {
	return a + b;
}

int dup(int a) {
	return add(a, a);
}

int sub(int a, int b) {
	return a - b;
}

int len() {
	int a = strlen("hola");
	int b = strlen("adios");
	return a + b;
}
