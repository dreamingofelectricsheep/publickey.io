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


struct peer_data;

typedef (*peer_fun)(struct peer_data *, int events);

struct peer_data {
	int fd;
	
	uint16_t port; };



void handlemail(struct peer_data * d, int events) {
	struct sockaddr_in6 mailer;
	socklen_t mailersize = sizeof(mailer);

	size_t bufferlength = 4096;
	uint8_t buffer[bufferlength];
	recvfrom(d->fd, buffer, bufferlength, 0, (struct sockaddr *) &mailer, &mailersize);

	sendto(d->fd, buffer, bufferlength, 0, (struct sockaddr *) &mailer, mailersize);

}

	

int main(int argc, char ** argv) {
	const uint16_t router_port = 8080;
	const uint16_t storage_port = 8081;

	int router = socket(AF_INET6, SOCK_DGRAM, 0);

	{
		struct sockaddr_in6 router_definition = {
			AF_INET6,
			htons(router_port),
			0, 0, 0 };
		
		int so_reuseaddr = true;

		setsockopt(router,
		       SOL_SOCKET,
		       SO_REUSEADDR,
		       &so_reuseaddr,
			sizeof so_reuseaddr);

		if(bind(router, (struct sockaddr *) &router_definition, 
			sizeof(struct sockaddr_in6))) {
			printf("bind failed\n"); }

	}

	int storage = socket(AF_INET6, SOCK_STREAM, 0);
	
	{
		struct sockaddr_in6 storage_definition = {
			AF_INET6,
			htons(storage_port),
			0, 0, 0 };
		
		int so_reuseaddr = true;

		setsockopt(storage,
		       SOL_SOCKET,
		       SO_REUSEADDR,
		       &so_reuseaddr,
			sizeof so_reuseaddr);

		if(bind(storage, (struct sockaddr *) &storage_definition, 
			sizeof(struct sockaddr_in6))) {
			printf("bind failed\n"); }

	}
	
	int epollfd = epoll_create(1);
	struct epoll_event * events;
	struct cache * caches = malloc(sizeof(struct cache) * 16);
	size_t caches_length;

	{

		struct epoll_event e;
		e.events = EPOLLIN | EPOLLET;
		e.data.fd = router;

		if(epoll_ctl(epollfd, EPOLL_CTL_ADD, router, &e) < 0)
			utf8print(utf8("Listener epoll failed"));

	}

	{

		struct epoll_event e;
		e.events = EPOLLIN | EPOLLET;
		e.data.fd = storage;

		if(epoll_ctl(epollfd, EPOLL_CTL_ADD, storage, &e) < 0)
			utf8print(utf8("Listener epoll failed"));

	}

	events = malloc(sizeof(struct epoll_event) * 1024);

	while(true) {
		int events_ready = epoll_wait(epollfd, events, 1024, -1);

		for(int i = 0; i < events_ready; i++) {
			switch(events[i].data.fd) {
				case storage: {
					int new_cache = accept(storage);

					struct epoll_event e;
					e.events = EPOLLIN | EPOLLET;
					e.data.fd = new_cache

					if(epoll_ctl(epollfd, EPOLL_CTL_ADD, 
						new_cache, &e) < 0)
						utf8print(utf8("Storage epoll failed"));
					break; }
				case router: {
					struct sockaddr_in6 mailer;
					socklen_t mailersize = sizeof(mailer);

					size_t bufferlength = 4096;
					uint8_t buffer[bufferlength];
					recvfrom(d->fd, buffer, bufferlength, 
						0, (struct sockaddr *) 
						&mailer, &mailersize);

						sendto(d->fd, buffer, 
							bufferlength, 0, 
							(struct sockaddr *) &mailer, 
							mailersize);

				
					


	
	free(events);

	return 0; }

