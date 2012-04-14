#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>

uint8_t * buffer;
const size_t s = 1 << 20;

void cfile(char * ff) {
	int f = open(ff, O_RDONLY);
	int r = read(f, buffer, s);

	if(buffer[0] == 'm' || buffer[0] == 'v' || buffer[0] == '/') {
		write(1, "<script>\n", 9); }
	else {
		write(1, "<style>\n", 8); }
	
	write(1, buffer, r);

	if(buffer[0] == 'm' || buffer[0] == 'v' || buffer[0] == '/') {
		write(1, "</script>\n", 10); }
	else {
		write(1, "</style>\n", 9); }

	close(f); }


int main(int argc, char ** argv) {

	write(1, "<html><body></body>", 19);


	buffer = malloc(s);

	cfile("require");
	for(int i = 1; i < argc; i++) {
		cfile(argv[i]); }

	write(1, "</html>", 7);

	free(buffer);

	return 0; }
