#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "bytes.c"
#include "p2p.c"


int indata(int epoll, struct objdata * data) {
	debug("Incoming email.");
	struct sockaddr_in6 src_addr;
	socklen_t addrlen = sizeof(src_addr);

	size_t len = 4096;
	struct dst {
		int fd;
		uint8_t buf[len];
		ssize_t len; } recvd;
	recvd.fd = data->fd;
	recvd.len = recvfrom(data->fd, recvd.buf, len, 0, 
		(void *)&src_addr, &addrlen);

	if(recvd.len > 0) {
		void f(struct peertreenode * n, void * data) {
			struct dst * recvd = data;
			
			struct sockaddr_in6 addr = { AF_INET6, n->key.port + 1, 0,
				n->key.addr, 0 };
			sendto(recvd->fd, recvd->buf, recvd->len, 0,
				(void*)&addr, sizeof(addr)); }

		struct p2p_st * p2p = (void*) data;
		peertreepreorder(&p2p->peer, f, &recvd);
		return 0; }
	else {
		debug("Problem receiving data.");
		return -1; } }

int main(int argc, char ** argv) {

	int epoll = epoll_create(1);
	int datafd = socket(AF_INET6, SOCK_DGRAM, 0);
	uint16_t port = 8080;
	prepare_socket(datafd, port + 1);


	struct p2p_st p2p;
	prepare_p2p(epoll, port, &p2p);




	struct objdata_ex dd = { { datafd, &indata, &err, &hup, &kill }, &p2p };
	epoll_add(epoll, datafd, &dd);

	// Interactive mode support.
	if(argc == 2 && strcmp(argv[0], "-i")) {
		printf("Interactive mode. Commands: list, add, remove, ping.\n");


		struct termdata { int pingfd; struct peertreenode ** node; };

		int in(int epoll, struct objdata * obj) { 
			size_t len = 4096;
			uint8_t buf[len];
			debug("Received data from terminal!");
			ssize_t r = read(0, buf, len);

			struct termdata * td = (void*) (obj + 1);
			struct peertreenode ** n = td->node;

			bytes re = { buf, r };

			bfound f = bfind(re, Bs("list"));
			if(f.found.length > 0) {
				debug("List command recevied");
				void printer(struct peertreenode * n, void * data) {
					char a[256];
					inet_ntop(AF_INET6, &n->key.addr.s6_addr,
						a, 256);
					printf("%s %d\n", a, ntohs(n->key.port)); }

				peertreepreorder(n, &printer, 0); }

			f = bfind(re, Bs("ping "));
			if(f.found.length > 0) {
				debug("Ping command received");

				f = bfind(f.after, B(' '));
				struct sockaddr_in6 addr;
				memset(&addr, 0, sizeof(addr));
				f.found.as_char[0] = 0;
				inet_pton(AF_INET6, f.before.as_void, &addr.sin6_addr);
				f = bfind(f.after, B('\n'));
				f.found.as_char[0] = 0;
				addr.sin6_port = htons(atoi(f.before.as_char));
				addr.sin6_family = AF_INET6;
				pingpeer(&addr, td->pingfd);
				
			}
			f = bfind(re, Bs("logging"));
			if(f.found.length > 0) {
				debug("Flipping logging bit!");
				logging = !logging; }
			
			return 0; }

		struct objdata * data = malloc(sizeof(struct objdata)
			+ sizeof(struct termdata));

		*data = (struct objdata) { 0, &in, &err, &hup, &kill };
		struct termdata * td = (void*) (data + 1);
		*td = (struct termdata) { p2p.pingfd, &p2p.peer };


		epoll_add(epoll, 0, data);



	}

	eventloop(epoll);
	return 0; }


