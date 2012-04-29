#include <stdio.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include <sys/time.h>
#include "echelon-protocol.c"

struct peer_user_data { };

#include "net.c"

void cache_handshake(struct net_st * net, struct peer_data * d, int events) {
	printf("Hello!");

	uint8_t buffer[4096];

	size_t r = read(d->fd, buffer, 4096);

	utf8print((utf8_t) { buffer, r });
}

	

int main(int argc, char ** argv) {
	const uint16_t router_port = 8080;
	const uint16_t storage_port = 8081;


	struct net_st network;
	netsetup(&network);

	int listener = netsocket(&network, SOCK_STREAM, 8080, cache_handshake);

	listen(listener, 1024);

	net(&network);

	return 0; }

