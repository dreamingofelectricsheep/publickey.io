#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char ** argv) {
	char * t = "\033[35mhello!\033[0m\n";
	printf("%s", t);
	return 0; }
