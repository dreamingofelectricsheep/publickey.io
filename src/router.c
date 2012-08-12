#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>



#define debug(d...) printf("[f: %s l: %d] ", __FILE__, __LINE__); \
	printf(d); printf("\n"); 

int create_socket(int epollfd, uint16_t port, void * data) {
	int sock = socket(AF_INET6, SOCK_DGRAM, 0);
	
	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	
		int r = true;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	if(bind(sock, (struct sockaddr *) &def, sizeof(struct sockaddr_in6))) {
		printf("Bind failed!\n"); }

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	e.data.ptr = data;


	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &e) < 0)
		printf("Listener epoll failed");

	return sock; }


#define bintree(var) peertree ## var

typedef struct {
	in_port_t port;
	struct in6_addr addr; } bintree(key_t);
typedef time_t bintree(payload_t);

int bintree(cmp_fun)(bintree(key_t) first, bintree(key_t) second) {
	return memcmp(&first, &second, sizeof(first)); }

#include "bintree.c"
#undef bintree
 

struct pingfunctordata { int fd; struct peertreenode ** peers; };

void pingfun(struct pingfunctordata * data) {
	void fun(struct peertreenode * n, void * data) {
		struct sockaddr_in6 addr = 
			{ AF_INET6, n->key.port, 0,
			n->key.addr, 0 };

		uint8_t s[256];
		int * ppingfd = data;
		sendto(*ppingfd, s, 256, 0, &addr, sizeof(addr)); }
		
	peertreepreorder(data->peers, &fun, &data->fd);

struct epoll_functor {
	int fd;
	void (*fun)(int events, struct epoll_functor *);
	/* Unspecified fields */ };


int main(int argc, char ** argv) {
	if(argc == 2 && strcmp(argv[0], "-i")) {
		printf("Interactive mode. Commands: list, add, remove, ping.\n"); }

	int max_peers = 1024;
	int npeers = 0;
	struct peertreenode * peers = 0;

	int epollfd = epoll_create(1);
	int pingsockfd = create_socket(epollfd, 8080, 0);
	int peerfd = create_socket(epollfd, 8081, 0);

	
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
	
	int max_events = 1024;
	struct epoll_events = malloc(sizeof(struct epoll_event) * max_events);
	while(true) {
		int events_ready = epoll_wait(epollfd, epoll_events, max_events, -1);

		for(int i = 0; i < events_ready; i++) {
			debug("Processing event #%d", i);
			int events = epoll_events[i].events;
			struct epoll_functor * d = events[i].data.ptr;
			
			if(events & EPOLLERR) {
				debug("Epoll error event."); }
			if(events & EPOLLHUP) {
				debug("Epoll hangup."); }
			d->fun(events, d); } } } }
					


	


