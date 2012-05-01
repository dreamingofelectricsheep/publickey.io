#include <unistd.h>
#include <stdint.h>

int main(int argc, char ** argv) {
	uint8_t buffer[256];
	char * t = "\n";
	while(1) {
		int r = read(0, buffer, 256);
		write(1, buffer, r);
		write(1, t, 1); }
	return 0; }
