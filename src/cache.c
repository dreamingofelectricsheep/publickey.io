#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bytes.c"

#define psize (sizeof(void *))	
#define bintree(var) cache ## var

typedef struct { uint8_t data[33]; } bintree(key_t);

struct cache_item {
	uint16_t size;
	bintree(key_t) key;
	uint8_t hash[32]; };
typedef struct { size_t size; void * array; } bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	return memcmp(&first, &second, sizeof(first)); }

#include "bintree.c"

#undef bintree


size_t nearest2(size_t n) {
	return 1 << ceil(log(n)/log(2)); }

void cachepushthing(struct cachenode ** n, cachekey_t key, struct cache_item * thing) {
	struct cachenode * f = cachefind(n, key);

	if(f == 0) {
		cachepayload_t payload = 
			{ 1, malloc(psize * 2) };

		cachepush(n, key, payload); }
	else {
		f->payload.size++;
		size_t alloc = nearest2(f->payload.size);
		if(alloc != f->payload.alloc) {
			f->payload.alloc = alloc
			void * new = malloc(psize * alloc);
			memcpy(new, f->payload.array, (psize * (f->payload.size - 1));
			free(f->payload.array);
			f->payload.array = new; }

		f->payload.array[f->payload.size - 1] = thing; } }




int main(int argc, char ** argv) {
	if(argc != 2) printf("Usage:\nbin-name /path/to/disk/cache\n");

	int fd = open(argv[1], O_RDWR);
	if(fd < 0) {
		printf("Can not open file %s\n", argv[1]);
		exit(0); }
	
	struct stat st;
	fstat(fd, &st);
	printf("Cache will use %lld bytes.\n", st.st_size);

	void * memcache = mmap(0, st.st_size, 
		PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, 0);

	if(memcache == (void *) -1) {
		printf("Mmap failed.\n");
		exit(0); }

	struct cachenode * cachetree = 0;
	return 0; }

