#define bintree(var) b ## var

typedef char bintree(key_t);
typedef float bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	return first - second; }

#include "bintree.c"

#undef bintree

#include <stdio.h>

void f(struct bnode * n, void * a) {
	printf("%c ", n->key); }

int main(int a, char ** b) {
	struct bnode * n = 0;
	bpush(&n, 'a', 0);
	bpush(&n, 'k', 0);
	bpush(&n, 'f', 0);
	bpush(&n, 'y', 0);
	bpush(&n, 'p', 0);
	bpush(&n, 'g', 0);
	bpush(&n, 'c', 0);
	bpush(&n, 'o', 0);
	bpush(&n, 'u', 0);
	bpush(&n, 'i', 0);
	bpush(&n, 'd', 0);
	bpush(&n, 'f', 0);
	bpush(&n, 'h', 0);
	bpush(&n, 't', 0);
	bpush(&n, 'n', 0);
	bpush(&n, 's', 0);
	bpush(&n, 'l', 0);
	bpush(&n, 'e', 0);
	bpush(&n, 'q', 0);
	bpush(&n, 'j', 0);
	bpush(&n, 'k', 0);
	bpush(&n, 'x', 0);
	bpush(&n, 'b', 0);
	bpush(&n, 'm', 0);
	bpush(&n, 'w', 0);
	bpush(&n, 'v', 0);

	bdelete_bykey(&n, 'o');
	bdelete_bykey(&n, 'h');
	bdelete_bykey(&n, 'k');
	binorder(&n, &f, 0);
	printf("\n");
	return 0; }
