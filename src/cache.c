#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>


	
#define bintree(var) cache ## var

typedef struct { uint8_t data[33]; } bintree(key_t);

struct cache_thing {
	uint16_t size;
	bintree(key_t) key; };
typedef struct { uint32_t size; uint32_t alloc; void ** arr; } bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	return memcmp(&first, &second, sizeof(first)); }

#include "bintree.c"

#undef bintree


void properpush(struct cachenode ** n, cachekey_t key, struct cache_thing * thing) {
	struct cachenode * f = cachefind(n, key);

	if(f == 0) {
		cachepayload_t payload = 
			{ 1, 1, malloc(sizeof(struct cache_thing *)) };

		cachepush(n, key, payload); }
	else {
		if(f->payload.size == f->payload.alloc) {
			f->payload.alloc*= 2;
			void * new = malloc(sizeof(void *) * f->payload.alloc);
			memcpy(new, f->payload.arr, f->payload.size);
			free(f->payload.arr);
			f->payload.arr = new; }

		f->payload.arr[f->payload.size] = thing;
		f->payload.size += 1; } }




int main(int argc, char ** argv) {
	if(argc != 2) printf("Usage:\nbin-name /path/to/disk/cache\n");

	int fd = open(argv[1], O_RDWR);
	if(fd < 0) {
		printf("Can not open file %s\n", argv[1]);
		exit(0); }
	
	struct stat st;
	fstat(fd, &st);
	printf("Cache will use %lld bytes.\n", st.st_size);

	void * mcache = mmap(0, st.st_size, 
		PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, 0);

	if(mcache == (void *) -1) {
		printf("Mmap failed.\n");
		exit(0); }

	struct cachenode * cachetree = 0;

	{
		off_t offset = 0;
		while(1) {
			struct cache_thing * entry = mcache + offset;
			if(entry->size == 0) break;
			
			properpush(&cachetree, entry->key, entry); } }

	return 0; }

