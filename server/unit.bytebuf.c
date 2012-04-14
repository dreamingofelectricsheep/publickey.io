#include "bytebuf.c"
#include <stdio.h>




int main(int argc, char ** argv) {
	
	utf8_t spare = bufferalloc(1024);

	utf8print(utf8fromint(1024, spare));
	utf8print(utf8("\n"));

	utf8print(utf8fromint(9999999, spare));
	utf8print(utf8("\n"));



	bufferfree(spare);	


	return 0; }
