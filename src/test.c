#include "bytes.c"


int main(int argc, char ** argv) {


	bytes b = B('a', 'b', 'c', 'd');

	int (*fun)(int a) = &aaa;

int aaa(int a) { return a; }
	bprint(b);
	return 0; }
