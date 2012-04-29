#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

/*

#define bintree(var) bintreename ## var

typedef enum {
	bintree(less),
	bintree(equal),
	bintree(greater) } bintree(cmp_t);

typedef int bintree(key_t);
typedef float bintree(payload_t);

bintree(cmp_t) bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	if(first == second) return bintree(equal);
	if(first < second) return bintree(less);
	return bintree(greater); }

#include "bintree.c"

#undef bintree

*/

struct bintree(node) {
	struct bintree(node) * left, * right;
	bintree(key_t) key;
	bintree(payload_t) payload; };

struct bintree(node) ** bintree(locate)(struct bintree(node) ** n, bintree(key_t) key) {
	if(*n) {
		switch(bintree(cmp_fun)((*n)->key, key)) {
			case bintree(less): 
				return bintree(locate)(&((*n)->left), key);
			case bintree(greater): 
				return bintree(locate)(&((*n)->right), key); } }
	return n; }

struct bintree(node) * bintree(find)(struct bintree(node) * n, bintree(key_t) key) {
	return *bintree(locate)(&n, key); }

void bintree(push)(struct bintree(node) ** n, 
		bintree(key_t) key, bintree(payload_t) payload) {
	struct bintree(node) ** i = bintree(locate)(n, key);
	if(*i) { (*i)->payload = payload; }
	else {
		*i = malloc(sizeof(struct bintree(node)));
		**i = (struct bintree(node)) { 0, 0, key, payload }; } }




















