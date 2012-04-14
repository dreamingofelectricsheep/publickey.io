#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct __dbnode_header {
	struct __dbnode_header * left, * right; 
	void * payload; } dbnode_header_t;

typedef enum { lesser, equal, greater } cmp_result_t;

typedef cmp_result_t (* comparator_t)(void *, void *);

int dbinsert(dbnode_header_t ** n, size_t key_size, comparator_t cmp, void * key, void * payload) {
	if(*n) { switch(cmp(key, (void *)((*n) + 1))) {
		case lesser: dbinsert(&(*n)->left, key_size, cmp, key, payload); return 0;
		case greater: dbinsert(&(*n)->right, key_size, cmp, key, payload); return 0;
		default: return 0; }
	} else { 
		(*n) = malloc(sizeof(dbnode_header_t) + key_size);
		(*n)->left = (dbnode_header_t *) 0;
		(*n)->right = (dbnode_header_t *) 0;
		(*n)->payload = payload;
		memcpy((void *) ((*n) + 1), key, key_size);
		return 1; } }

typedef void (* dbfunc_t)(void * );

void dbinorder(dbnode_header_t * n, dbfunc_t foo) {
	if(n) {
		dbinorder(n->left, foo);
		foo(n->payload);
		dbinorder(n->right, foo); } }
			

cmp_result_t int_cmp(void * a, void * b) {
	if(*((int *) a) == *((int *) b)) return equal;
	if(*((int *) a) < *((int *) b)) return lesser;
	return greater; }

void dbprint(void * s) { printf("%s\n", (char*) s); }

int main(int argc, char ** argv) {

	dbnode_header_t * root = 0;
	int keys[16];

	for(int i = 0; i < 16; i += 1) { keys[i] = i; }

	dbinsert(&root, sizeof(int), &int_cmp, keys + 6, (void *) "... my little pony");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 11, (void *) "So Sweet and strong");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 10, (void *) "Beautiful heart");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 9, (void *) "Tons of fun");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 15, (void *) "ALL COMPLETE!!!");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 4, (void *) "My little pony...!");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 8, (void *) "Big adventure");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 12, (void *) "Sharing kindress");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 14, (void *) "Magic makes it");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 3, (void *) "aaaaaaa...");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 7, (void *) "Until you all shared it's magic with me");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 2, (void *) "my little pony");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 5, (void *) "I still wonder what friendship could be");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 1, (void *) "My little pony");
	dbinsert(&root, sizeof(int), &int_cmp, keys + 13, (void *) "Is an easy feat");
	dbinorder(root, &dbprint);


	return 0; }
