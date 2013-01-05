#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#include "bytes.c"

typedef int mapkey;
typedef int mappayload;



int mapcmp(mapkey * first, mapkey * second)
{
	return -memcmp(first, second, sizeof(*first));
}

#define MAP(var) map ## var
#include "map.c"
#undef MAP

void mapprint(struct map * m, void * v) {
	debug("%d", m->key);
}
int main(int argc, char **argv)
{

	struct map * m = 0;
	int key = 21;

	mapinsert(&m, &key, &key);

	key = 2;
	mapinsert(&m, &key, &key);

	key = 5;
	mapinsert(&m, &key, &key);

	key = 9;
	mapinsert(&m, &key, &key);

	key = 20;mapinsert(&m, &key, &key);

	key = 10;
	mapinsert(&m, &key, &key);
	key = 8;mapinsert(&m, &key, &key);
	key = 11;mapinsert(&m, &key, &key);
	key = 4;mapinsert(&m, &key, &key);
	key = 12;mapinsert(&m, &key, &key);
	key = 3;mapinsert(&m, &key, &key);
	key = 14;mapinsert(&m, &key, &key);
	key = 15;mapinsert(&m, &key, &key);
	key = 16;mapinsert(&m, &key, &key);
	key = 1;mapinsert(&m, &key, &key);
	key = 7;mapinsert(&m, &key, &key);
	key = 6;mapinsert(&m, &key, &key);
	key = 13;mapinsert(&m, &key, &key);
	key = 133;mapinsert(&m, &key, &key);
	key = 23;mapinsert(&m, &key, &key);
	key = 156;mapinsert(&m, &key, &key);
	key = 199;mapinsert(&m, &key, &key);
	key = 100;mapinsert(&m, &key, &key);
	key = 99;mapinsert(&m, &key, &key);
	key = 98;mapinsert(&m, &key, &key);
	key = 94;mapinsert(&m, &key, &key);
	key = 95;mapinsert(&m, &key, &key);
	key = 132;mapinsert(&m, &key, &key);
	key = 130;mapinsert(&m, &key, &key);
	key = 101;mapinsert(&m, &key, &key);
	key = 200;mapinsert(&m, &key, &key);
	key = 144;mapinsert(&m, &key, &key);
	key = 145;mapinsert(&m, &key, &key);
	key = 146;mapinsert(&m, &key, &key);
	key = 147;mapinsert(&m, &key, &key);
	key = 155;mapinsert(&m, &key, &key);
	key = 103;mapinsert(&m, &key, &key);
	key = 104;mapinsert(&m, &key, &key);
	key = 201;mapinsert(&m, &key, &key);

/*

	key = 17; mapprint(*mapfindnext(&m, &key), 0);
	key = 21; mapprint(*mapfindnext(&m, &key), 0);
	key = 0; mapprint(*mapfindnext(&m, &key), 0);
	key = 1; mapprint(*mapfindnext(&m, &key), 0);
	key = 2; mapprint(*mapfindnext(&m, &key), 0);
	key = 22; mapprint(*mapfindnext(&m, &key), 0);
*/

	key = 220;mapprint(mapfindnext(m, &key), 0);
	key = 0;mapprint(mapfindnext(m, &key), 0);
	key = 198;mapprint(mapfindnext(m, &key), 0);
	key = 130;mapprint(mapfindnext(m, &key), 0);

	return 0;
}
