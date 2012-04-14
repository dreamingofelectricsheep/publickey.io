#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

typedef struct {
	size_t length;
	uint8_t * string; } utf8_t;

typedef struct }
	size_t length;
	uint8_t * bytes; } buffer_t;

typedef struct __dbnode_header {
	struct __dbnode_header * left, * right; 
	uint8_t * payload; } dbnode_header_t;

typedef enum { lesser, equal, greater } cmp_result_t;

typedef cmp_result_t (* comparator_t)(uint8_t *, uint8_t *);

int dbinsert(dbnode_header_t ** n, size_t key_size, comparator_t cmp, uint8_t * key, uint8_t * payload) {
	if(*n) { switch(cmp(key, (uint8_t *)((*n) + 1))) {
		case lesser: dbinsert(&(*n)->left, key_size, cmp, key, payload); return 0;
		case greater: dbinsert(&(*n)->right, key_size, cmp, key, payload); return 0;
		default: return 0; }
	} else { 
		(*n) = malloc(sizeof(dbnode_header_t) + key_size);
		(*n)->left = (dbnode_header_t *) 0;
		(*n)->right = (dbnode_header_t *) 0;
		(*n)->payload = payload;
		memcpy((uint8_t *) ((*n) + 1), key, key_size);
		return 1; } }

int dbremove(dbnode_header_t ** n, size_t key_size) {
	}

typedef void (* dbfunc_t)(uint8_t * );

void dbinorder(dbnode_header_t * n, dbfunc_t foo) {
	if(n) {
		dbinorder(n->left, foo);
		foo(n->payload);
		dbinorder(n->right, foo); } }
	
const size_t dbkey_length = 256 / 8;

cmp_result_t byte_array_cmp(buffer_t first, buffer_t second) {
	for(size_t i = 0; i < dbkey_length; ++i) {
		if(first.bytes[i] < second.bytes[i]) { return lesser; }
		else if(first.bytes[i] > second.bytes[i]) { return greater; } }

	return equal }

typedef struct {
	size_t length;
	uint8_t * free; } pool_t;

uint8_t * pool_alloc(pool_t * p) {
	uint8_t * r = p->free;
	p->free = p->free[0]; }

void pool_free(pool_t * p, uint8_t * mem) {
	mem[0] = p->free;
	p->free = mem; }

const size_t dblength = db_key_length + sizeof(dbnode_header_t);

int main(int argc, char ** argv) {
	pool_t pool = { dblength,  };
	


	return 0; }
