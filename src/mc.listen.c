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

#include <netdb.h>

struct peer_user_data { };

#include "net.c"

void cache_handshake(struct net_st * net, struct peer_data * d, int events) {
	uint8_t buffer[4096];

	size_t r = read(d->fd, buffer, 4096);

	bytesprint((bytes) { buffer, r });
}

	
#include <errno.h>

int main(int argc, char ** argv) {
	const uint16_t router_port = 8080;
	const uint16_t storage_port = 8081;


	struct net_st network;
	netsetup(&network);

	int listener = netsocket(&network, SOCK_DGRAM, 8080, cache_handshake);

	struct addrinfo info;
	memset(&info, 0, sizeof(info));
	info.ai_family = AF_INET6;

	struct addrinfo * res;

	if(getaddrinfo("outerechelon.org", 0, &info, &res) != 0) {
		fprintf(stderr, "Error on getaddrinfo: %s\n", strerror(errno)); }

	struct ipv6_mreq mreq6;
	memcpy(&mreq6.ipv6mr_multiaddr, &(((struct sockaddr_in6 *)res->ai_addr)->sin6_addr),
	       sizeof(struct in6_addr));
	mreq6.ipv6mr_interface= 0;

	if(setsockopt(listener, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6))) {
		fprintf(stderr, "setsockopt IPV6_JOIN_GROUP: %s\n", strerror (errno)); }


	net(&network);

	return 0; }

