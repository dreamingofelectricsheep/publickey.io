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
 

/*void pingfun(struct pingfunctordata * data) {
	void fun(struct peertreenode * n, void * data) {
		struct sockaddr_in6 addr = 
			{ AF_INET6, n->key.port, 0,
			n->key.addr, 0 };

		uint8_t s[256];
		int * ppingfd = data;
		sendto(*ppingfd, s, 256, 0, &addr, sizeof(addr)); }
		
	peertreepreorder(data->peers, &fun, &data->fd);
*/

int main(int argc, char ** argv) {

	int epoll = epoll_create(1);

	int pingfd = socket(AF_INET6, SOCK_DGRAM, 0);
	uint16_t port = 8080;

	{	
		int r = true;
		setsockopt(pingfd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

		struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
		if(bind(pingfd, (struct sockaddr *) &def, sizeof(def))) {
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

	struct routerdata { struct objdata obj; struct peertreenode * p; }; 
	int in(int epoll, struct objdata * data) {
		debug("Incoming packet.");
		struct sockaddr_in6 src_addr;
		socklen_t addrlen = sizeof(src_addr);

		size_t len = 4096;
		uint8_t buf[len];

		ssize_t r = recvfrom(data->fd, buf, len, 0, (void *)&src_addr, &addrlen);
		struct peertreenode ** p = (void *) (data + 1);
		peertreekey_t key = { src_addr.sin6_port, src_addr.sin6_addr };
		
		time_t t = time(0);
		peertreepush(p, key, t);

		return 0;
	}
		

	struct routerdata pd = 
		{ { pingfd, &in, &err, &hup, &kill }, 0 };


	{	
		struct epoll_event e;
		e.events = EPOLLIN; // | EPOLLET;
		e.data.ptr = &pd;

		if(epoll_ctl(epoll, EPOLL_CTL_ADD, pingfd, &e) < 0) {
			debug("Listener epoll_ctl failed"); return -1; }
	}

	if(argc == 2 && strcmp(argv[0], "-i")) {
		printf("Interactive mode. Commands: list, add, remove, ping.\n");



		int in(int epoll, struct objdata * obj) { 
			size_t len = 4096;
			uint8_t buf[len];
			debug("Received data from terminal!");
			ssize_t r = read(0, buf, len);
			write(1, buf, r);
			return 0; }
		struct objdata * data = malloc(sizeof(struct objdata));
		*data = (struct objdata) { 0, &in, &err, &hup, &kill };


		{	
			struct epoll_event e;
			e.events = EPOLLIN; // | EPOLLET;
			e.data.ptr = data;

			if(epoll_ctl(epoll, EPOLL_CTL_ADD, 0, &e) < 0) {
				debug("Stdin epoll_ctl failed"); return -1; }
		}


	}

	eventloop(epoll);
	return 0; }



/*
int main(int argc, char ** argv) {

		
	int pingfd = timerfd_create(CLOCK_REALTIME, 0);
	int hupfd = timerfd_create(CLOCK_REALTIME, 0);

	struct timerspec tp;
	clock_gettime(CLOCK_REALTIME, &tp);

	tp.tv_sec += 1024;
	timerfd_settime(pongfd, 0, &tp, 0);

	struct { struct epoll_functor; 
		struct pingfunctordata; } pingfunctor 
		= { { pingfd, &pingfun }, { pingsockfd, &peers } };

	
	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	e.data.ptr = &pingfunctor;


	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, pingfd, &e) < 0)
		debug("Listener epoll failed");


	tp.tv_sec += 3072;
	timerfd_settime(hupfd, 0, &tp, 0);
*/				


	


