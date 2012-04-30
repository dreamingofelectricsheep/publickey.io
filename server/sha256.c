#include "bytes.c"
#include <fcntl.h>

#include "echelon-protocol.c"



int main(int argc, char ** argv) {

	bytes mem = bytesalloc(2 ^ 16);
	bytes in = bytesread(mem);

	uint8_t hash[32];

	SHA256(in.ptr, in.length, hash);

	write(1, hash, 32);

	bytesfree(mem);

	return 0; }
















