#include <math.h>
#include "bytes.c"
#include <stdio.h>


int main(int argc, char ** argv) {
	printf("%d", sizeof((char[]) {1, 2, 3, 4, 5, 6, 7}));
	return 0; }

