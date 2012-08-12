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
		struct epoll_event e;
		e.events = EPOLLIN | EPOLLET;

		if(epoll_ctl(epollfd, EPOLL_CTL_ADD, postoffice, &e) < 0)
			utf8print(utf8("Listener epoll failed"));

	}

	events = malloc(sizeof(struct epoll_event) * 1024);

	while(true) {
		int events_ready = epoll_wait(epollfd, events, 1024, -1);

		for(int i = 0; i < events_ready; i++) {
			size_t bufferlength = 4096;
			uint8_t buffer[bufferlength];
			size_t r = recvfrom(postoffice, buffer, bufferlength, 0, 0, 0);

			write(1, buffer, r); } }
	
	free(events);

	return 0; }

