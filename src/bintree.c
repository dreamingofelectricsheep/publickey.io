#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

/*

#define bintree(var) bintreename ## var

typedef int bintree(key_t);
typedef float bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	if(first == second) return 0;
	if(first < second) return -1;
	return 1; }

#include "bintree.c"

#undef bintree

*/

struct bintree(node) {
	struct bintree(node) * left, * right;
	bintree(key_t) key;
	bintree(payload_t) payload; };

struct bintree(node) ** bintree(locate)(struct bintree(node) ** n, bintree(key_t) key) {
	if(*n) {
		int r = bintree(cmp_fun)((*n)->key, key);
		if(r < 0) return bintree(locate)(&((*n)->left), key);
		if(r > 0) return bintree(locate)(&((*n)->right), key); } 
	return n; }

struct bintree(node) * bintree(find)(struct bintree(node) ** n, bintree(key_t) key) {
	return *bintree(locate)(n, key); }

void bintree(push)(struct bintree(node) ** n, 
		bintree(key_t) key, bintree(payload_t) payload) {
	struct bintree(node) ** i = bintree(locate)(n, key);
	if(*i) { (*i)->payload = payload; }
	else {
		*i = malloc(sizeof(struct bintree(node)));
		**i = (struct bintree(node)) { 0, 0, key, payload }; } }

typedef void (*bintree(functor))(struct bintree(node) * n, void *);

void bintree(preorder)(struct bintree(node) ** n,
	bintree(functor) fun, void * data) {
	if(*n) {
		fun(*n, data);
		bintree(preorder)(&((*n)->left), fun, data);
		bintree(preorder)(&((*n)->right), fun, data); } }
		
	
















