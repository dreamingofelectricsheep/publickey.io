#include "bytes.c"
#include <math.h>
#include <stdio.h>

int main(int argc, char ** argv) {

	uint32_t nearest2(uint32_t n) {
    	return ceil(log(n)/log(2)); }

	printf("%d: %d\n", 1, 2 << nearest2(1));
	printf("%d: %d\n", 2, 2 << nearest2(2));
	printf("%d: %d\n", 3, 2 << nearest2(3));
	printf("%d: %d\n", 10, 2 << nearest2(10));
	printf("%d: %d\n", 9, 2 << nearest2(9));
	printf("%d: %d\n", 0, 2 << nearest2(0));
	return 0; }
