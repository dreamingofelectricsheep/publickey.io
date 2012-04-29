#include <stdint.h>
#include "bytebuf.c"
#include <stdio.h>

typedef uint8_t id[32];

#define bintree(var) cache ## var

#include "bintree_cmp.c"

typedef id bintree(key_t);
typedef bytes bintree(payload_t);

bintree(cmp_t) bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	printf("%d, %d\n", sizeof(id), 32);

	for(int i = 31; i >= 0; i--) {
		if(first[i] > second[i]) return bintree(greater);
		if(first[i] < second[i]) return bintree(less); }

	return bintree(equal); }

#include "bintree.c"

#undef bintree


struct peer_user_data { };

#include "net.c"


void in_data(struct net_st * net, struct peer_data * d, int events) {


void in_handshake(struct net_st * net, struct peer_data * d, int events) {
	d->fun = in_data; }
	

int main(int argc, char ** argv) {
	const uint16_t in_port = 8080;
	const uint16_t storage_port = 8081;

	struct net_st network;
	netsetup(&network);

	int in = netsocket(&network, SOCK_STREAM, in_port, cache_handshake);

	listen(in, 1024);

	net(&network);



	return 0; }

