#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>


struct ppp {
	int a; int r; };

typedef uint8_t iii[16];
int main(int argc, char ** argv) {


	iii a = {  1, 2, 2, 3, 3 };

	printf("%d", a[4]);

	void * p = a;

	int * r = p;

	int s = sizeof(*r);

	struct ppp l;
	l = { 1, 2 };
	struct {
		int a; int x; } * o = p;
	return 0; }
