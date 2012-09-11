#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <time.h>
#include <arpa/inet.h>

#include "event.c"

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


int prepare_socket(int fd, uint16_t port) {	
	int r = true;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	if(bind(fd, (struct sockaddr *) &def, sizeof(def))) {
		debug("Bind failed!");
		return -1; }
	return 0; }



struct p2p_st {
	int pingfd;
	struct peertreenode * peer;
	struct objdata_ex ping, huptimer, pingtimer; };


int ping_in_fn(int epoll, struct objdata * data) {
	debug("Incoming packet.");
	struct sockaddr_in6 src_addr;
	socklen_t addrlen = sizeof(src_addr);

	size_t len = 4096;
	uint8_t buf[len];

	struct p2p_st ** p2p = (void*)(data+1);
	
	ssize_t r = recvfrom(data->fd, buf, len, 0, (void *)&src_addr, &addrlen);
	peertreekey_t key = { src_addr.sin6_port, src_addr.sin6_addr };
	
	time_t t = time(0);
	peertreepush(&(*p2p)->peer, key, t);

	return 0; }





int huptimer_fn(int epoll, struct objdata * data) {
	debug("HUP timer event.");
	char o[256];
	read(data->fd, o, 256);
	return 0; }

int pingtimer_fn(int epoll, struct objdata * data) {
	debug("Ping timer event.");
	struct p2p_st ** p2p = (void*)(data+1);
	uint64_t junk;
	read(data->fd, &junk, sizeof(junk));	
	void fun(struct peertreenode * n, void * data) {
		int * pingfd = data;
		struct sockaddr_in6 addr = { AF_INET6, n->key.port, 0,
			n->key.addr, 0 };
		uint8_t buffer[256];
		sendto(*pingfd, buffer, 256, 0, (void*)&addr, sizeof(addr)); }

	peertreepreorder(&(*p2p)->peer, &fun, &(*p2p)->pingfd);
	return 0; }	


	
int prepare_p2p(int epollfd, uint16_t port, struct p2p_st * p2p) {
	p2p->peer = 0;
	p2p->pingfd = socket(AF_INET6, SOCK_DGRAM, 0);

	prepare_socket(p2p->pingfd, port);

	prepare_objdata_ex(&p2p->ping, p2p->pingfd, 
		&ping_in_fn, &err, &hup, &kill, p2p);

	epoll_add(epollfd, p2p->pingfd, &p2p->ping);


	int huptimerfd = timerfd_create(CLOCK_MONOTONIC, 0);
	int pingtimerfd = timerfd_create(CLOCK_MONOTONIC, 0);


	prepare_objdata_ex(&p2p->pingtimer, pingtimerfd, 
		&pingtimer_fn, &err, &hup, &kill, p2p); 
	prepare_objdata_ex(&p2p->huptimer, huptimerfd, 
		&huptimer_fn, &err, &hup, &kill, p2p); 

	epoll_add(epollfd, huptimerfd, &p2p->huptimer);
	epoll_add(epollfd, pingtimerfd, &p2p->pingtimer);	

	struct itimerspec spec = { { 10, 0 }, { 1, 0 } };

	timerfd_settime(huptimerfd, 0, &spec, 0);
	timerfd_settime(pingtimerfd, 0, &spec, 0);



	return 0; }
