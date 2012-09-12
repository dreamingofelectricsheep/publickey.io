#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <time.h>
#include <arpa/inet.h>

#include "bytes.c"
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
 

void p2pprint(struct peertreenode * n, void * data) {
	char a[256];
	inet_ntop(AF_INET6, &n->key.addr.s6_addr,
		a, 256);
	printf("%s %d, seen: %ld\n", a, ntohs(n->key.port),
		n->payload); }



int prepare_socket(int fd, uint16_t port) {	
	int r = true;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	if(bind(fd, (struct sockaddr *) &def, sizeof(def))) {
		debug("Bind failed!");
		return -1; }
	return 0; }



typedef int (*p2pexternal_fn)(struct sockaddr_in6 addr, bytes data, void * ad);
struct p2p_st {
	int socket;
	struct peertreenode * peer;
	p2pexternal_fn in;
	void * additional;
	struct objdata_ex ping, huptimer, pingtimer; };



int p2pin_fn(int epoll, struct objdata * data) {
	debug("Incoming packet.");
	struct p2p_st * p2p = *((struct p2p_st**)(data+1));
	
	struct sockaddr_in6 src_addr;
	socklen_t addrlen = sizeof(src_addr);

	size_t len = 4096;
	char buf[len];

	
	ssize_t r = recvfrom(data->fd, buf, len, 0, (void *)&src_addr, &addrlen);

	if(r > 0) {
		if(buf[0] == 0) {
			peertreekey_t key = { src_addr.sin6_port, src_addr.sin6_addr };
			
			time_t t = time(0);
			peertreepush(&p2p->peer, key, t); }
		else { 
			p2p->in(src_addr, (bytes) { buf, r}, p2p->additional); }
		return 0; }
	else return -1; }





int huptimer_fn(int epoll, struct objdata * data) {
	debug("HUP timer event.");
	char o[256];
	read(data->fd, o, 256);
	return 0; }


struct p2psend_st {
	int fd;
	bytes data; };

void p2psendpeer(struct peertreenode * n, void * data) {
	struct p2psend_st * st = data;
	struct sockaddr_in6 addr = { AF_INET6, n->key.port, 0,
		n->key.addr, 0 };
	sendto(st->fd, st->data.as_void, st->data.length, 0, 
		(void*)&addr, sizeof(addr)); }

void p2psend(struct p2p_st * p2p, bytes data) {
	struct p2psend_st st = { p2p->socket, data };
	peertreepreorder(&p2p->peer, &p2psendpeer, &st); }

void p2ppingpeer(struct sockaddr_in6 * addr, int fd) {
	bytes pingjunk = Bs("\0ping");
	sendto(fd, pingjunk.as_void, pingjunk.length, 0, 
		(void*)addr, sizeof(*addr)); }


int pingtimer_fn(int epoll, struct objdata * data) {
	debug("Ping timer event.");
	struct p2p_st ** p2p = (void*)(data+1);
	uint64_t junk;
	read(data->fd, &junk, sizeof(junk));	
	
	bytes pingjunk = Bs("\0ping");
	p2psend(*p2p, pingjunk);
	return 0; }	


	
int p2pprepare(int epollfd, uint16_t port, p2pexternal_fn in, void * a, 
	struct p2p_st * p2p) {
	
	p2p->additional = a;
	p2p->peer = 0;
	p2p->socket = socket(AF_INET6, SOCK_DGRAM, 0);
	p2p->in = in;

	prepare_socket(p2p->socket, port);

	prepare_objdata_ex(&p2p->ping, p2p->socket, 
		&p2pin_fn, &err, &hup, &kill, p2p);

	epoll_add(epollfd, p2p->socket, &p2p->ping);


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




















