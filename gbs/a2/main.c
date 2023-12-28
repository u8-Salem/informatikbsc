#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	halde_print();

	char* m1 = halde_malloc(512);
	halde_print();
	char* m2 = halde_malloc(512);
	halde_print();

	halde_free(m1);
	halde_free(m2);
	halde_print();

	exit(EXIT_SUCCESS);
}
