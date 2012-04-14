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
#include "bytebuf.c"
	
struct peer_data;
typedef void (*peer_fun)(struct peer_data *, int events);
struct peer_data {
	int fd;
	void * ptr;
	peer_fun fun; };

struct echelon_protocol_header {
	uint16_t length;
	uint16_t tag; };

void handlemail(struct peer_data * d, int events) {
	struct sockaddr_in6 mailer;
	socklen_t mailersize = sizeof(mailer);

	size_t bufferlength = 4096;
	uint8_t buffer[bufferlength];
	recvfrom(d->fd, buffer, bufferlength, 0, (struct sockaddr *) &mailer, &mailersize);

	sendto(d->fd, buffer, bufferlength, 0, (struct sockaddr *) &mailer, mailersize);

}

int main(int argc, char ** argv) {
	int postoffice_port = 8080;

	int postoffice = socket(AF_INET6, SOCK_DGRAM, 0);

	{
		struct sockaddr_in6 postoffice_definition = {
			AF_INET6,
			htons(postoffice_port),
			0, 0, 0 };
		
		int so_reuseaddr = true;

		setsockopt(postoffice,
		       SOL_SOCKET,
		       SO_REUSEADDR,
		       &so_reuseaddr,
			sizeof so_reuseaddr);

		if(bind(postoffice, (struct sockaddr *) &postoffice_definition, 
			sizeof(struct sockaddr_in6))) {
			printf("bind failed\n"); }

	}

	printf("Listening...\n");
	
	int epollfd = epoll_create(1);
	struct epoll_event * events;

	{
		struct peer_data * p = malloc(sizeof(struct peer_data));
		*p = (struct peer_data) { postoffice, 0, &handlemail };

		struct epoll_event e;
		e.events = EPOLLIN | EPOLLET;
		e.data.ptr = p;

		if(epoll_ctl(epollfd, EPOLL_CTL_ADD, postoffice, &e) < 0)
			utf8print(utf8("Listener epoll failed"));

	}

	events = malloc(sizeof(struct epoll_event) * 1024);

	uint64_t last;
	struct timeval tv;
	gettimeofday(&tv, 0);
	last = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	while(true) {
		int events_ready = epoll_wait(epollfd, events, 1024, -1);

		for(int i = 0; i < events_ready; i++) {
			struct peer_data * data = events[i].data.ptr;
			data->fun(data, events[i].events); } }
	
	free(events);

	return 0; }

