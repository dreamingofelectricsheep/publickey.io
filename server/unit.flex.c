#include "flex.c"
#include <stdio.h>

void pbin(uint8_t b) {
	for(int i = 7; i >= 0; i--) {
		printf("%d", (b & (1 << i)) > 0); }

	printf(" "); }

void printbytes(bytes b) {
	for(int i = 0; i < b.length; i++) {
		if(i % 8 == 0) printf("\n");
		pbin(b.raw[i]); }

	printf("\n"); }

int main(int argc, char ** argv) {


	

	bytes mem = bytesalloc(1000);
	bytes mem2 = bytesalloc(1000);
	bytes r = bytesalloc(30);

	//for(int i = 0; i < 30; i++) { r.raw[i] = (1 << 7) + 1; }
	for(int i = 0; i < 30; i++) { r.raw[i] = (1 << 8) - 1; }


	bytes o = toflex64(4096, mem);
	printf("4096 = %lld\n", fromflex64(o));

	o = toflex64(178, mem);
	printf("178 = %lld\n", fromflex64(o));

	o = toflex64(16, mem);
	printf("16 = %lld\n", fromflex64(o));

	o = toflex64(65000, mem);
	printf("65000 = %lld\n", fromflex64(o));

	o = toflex64(3, mem);
	printf("3 = %lld\n", fromflex64(o));



	return 0; }
