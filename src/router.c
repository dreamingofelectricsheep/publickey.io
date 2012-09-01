#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/timerfd.h>
#include <time.h>

#include "bytes.c"

struct objdata;
typedef int (*objfun)(int epoll, struct objdata * blob);
struct objdata {
	int fd;
	objfun in, err, hup, kill; };


	
int eventloop(int epollfd) {
	int max_events = 1024;
	struct epoll_event * buffer = malloc(max_events * sizeof(*buffer));
	while(true) {
		int ready = epoll_wait(epollfd, buffer, max_events, -1);

		debug("Events ready: %d", ready);
		for(int i = 0; i < ready; i++) {
			int events = buffer[i].events;
			struct objdata * d = buffer[i].data.ptr;
			objfun fun;
			int r = 0;
			
			if(events & EPOLLERR) r += d->err(epollfd, d);
			else {
				if(events & EPOLLIN) r += d->in(epollfd, d);
				if(events & EPOLLHUP) r += d->hup(epollfd, d); }

			if(r) { 
				debug("Socket callback failed")
				d->kill(epollfd, d); }
			}} }

#define bintree(var) peertree ## var

typedef struct {
	in_port_t port;
	struct in6_addr addr; } bintree(key_t);
typedef time_t bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	return memcmp(&first, &second, sizeof(first)); }

#include "bintree.c"
#undef bintree
 


void pingpeer(struct sockaddr_in6 * addr, int fd) {
	char data[256];
	if(sendto(fd, data, 256, 0, (void*)addr, sizeof(*addr)) == -1) {
		debug("Something went wrong : ("); } }

int epoll_add(int epoll, int fd, void * data) {
		struct epoll_event e;
		e.events = EPOLLIN; // | EPOLLET;
		e.data.ptr = data;

		if(epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &e) < 0) {
			debug("Stdin epoll_ctl failed"); 
			return -1; }
		else return 0; }




int main(int argc, char ** argv) {

	int epoll = epoll_create(1);

	int socketfd = socket(AF_INET6, SOCK_DGRAM, 0);
	uint16_t port = 8080;

	{	
		int r = true;
		setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

		struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
		if(bind(socketfd, (struct sockaddr *) &def, sizeof(def))) {
			debug("Bind failed!");
			return -1; }
	}
	

	int kill(int epoll, struct objdata * data) {
		// Don't worry, epoll concatenates events on the same destriptor.
		debug("Terminating!");
		close(data->fd);
		return 0; }

	int err(int epoll, struct objdata * data ) {
		debug("Receiving error: %s", strerror(errno));
		data->kill(epoll, data);
		return 0; }

	int hup(int epoll, struct objdata * data ) {
		debug("Hangup received.");
		data->kill(epoll, data);
		return 0; }

	struct routerdata { struct objdata obj; struct peertreenode ** p; }; 
	int in(int epoll, struct objdata * data) {
		debug("Incoming packet.");
		struct sockaddr_in6 src_addr;
		socklen_t addrlen = sizeof(src_addr);

		size_t len = 4096;
		uint8_t buf[len];

		ssize_t r = recvfrom(data->fd, buf, len, 0, (void *)&src_addr, &addrlen);
		struct peertreenode *** p = (void *) (data + 1);
		peertreekey_t key = { src_addr.sin6_port, src_addr.sin6_addr };
		
		time_t t = time(0);
		peertreepush(*p, key, t);

		return 0;
	}
		
	struct peertreenode * n = 0;
	struct routerdata pd = 
		{ { socketfd, &in, &err, &hup, &kill }, &n };

	epoll_add(epoll, socketfd, &pd);

	// Interactive mode support.
	if(argc == 2 && strcmp(argv[0], "-i")) {
		printf("Interactive mode. Commands: list, add, remove, ping.\n");


		struct termdata { int socketfd; struct peertreenode ** node; };

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
				pingpeer(&addr, td->socketfd);
				
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
		*td = (struct termdata) { socketfd, &n };


		epoll_add(epoll, 0, data);



	}

	int huptimerfd = timerfd_create(CLOCK_MONOTONIC, 0);
	int pingtimerfd = timerfd_create(CLOCK_MONOTONIC, 0);


	int hupfun(int epoll, struct objdata * data) {
		debug("HUP timer event.");
		char o[256];
		read(data->fd, o, 256);
		return 0; }

	struct pingtimer_st { 
		struct objdata data;
		struct peertreenode ** n;
		int socketfd; };

	int pingtimerfun(int epoll, struct objdata * data) {
		debug("Ping timer event.");
		struct pingtimer_st * st = (void*) data;
		uint64_t junk;
		read(data->fd, &junk, sizeof(junk));	
		void fun(struct peertreenode * n, void * data) {
			int * socketfd = data;
			struct sockaddr_in6 addr = { AF_INET6, n->key.port, 0,
				n->key.addr, 0 };
			uint8_t buffer[256];
			sendto(*socketfd, buffer, 256, 0, (void*)&addr, sizeof(addr)); }

		peertreepreorder(st->n, &fun, &st->socketfd);
		return 0; }	

	struct pingtimer_st pingtimerst = { 
			{ pingtimerfd, &pingtimerfun, &err, &hup, &kill },
			&n, socketfd }; 
	struct routerdata td = 
		{ { huptimerfd, &hupfun, &err, &hup, &kill }, &n };
	
	epoll_add(epoll, huptimerfd, &td);
	epoll_add(epoll, pingtimerfd, &pingtimerst);	

	struct itimerspec spec = { { 10, 0 }, { 1, 0 } };

	timerfd_settime(huptimerfd, 0, &spec, 0);
	timerfd_settime(pingtimerfd, 0, &spec, 0);


	eventloop(epoll);
	return 0; }


