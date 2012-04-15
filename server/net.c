#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>


// Before including this file defile a struct peer_user_data.

struct peer_data;
typedef void (*peer_fun)(struct peer_data *, int events);
struct peer_data {
	int fd;
	peer_fun fun;
	uint16_t port;
	uint8_t ip[16];
	struct peer_user_data data; };

struct peer_data_listen;
typedef void (*peer_fun_listen)(struct peer_data_listen *, int events);
struct peer_data_listen {
	int fd;
	peer_fun_int fun2;
	peer_fun fun; };

struct net_st {
	int epoll;
	struct epoll_event * events;
	struct peer_data * peers;
	size_t peers_length;
	size_t peers_max; }

void netaccept(struct peer_data_listen * p, int events) {
	struct sockaddr_in6 def;
	socklen_t len = sizeof(def);

	int socket = accept(p->fd, &definition, &len);

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	struct peer_data * n = e.data.ptr = net->peers + net->peers_length;
	net->peers_length++;
	n->fd = sock;
	n->fun = p->fun;
	n->port = ntohs(def.sin6_port);

	memcpy(p->ip, def.sin6_addr.s6_addr, 16);

	if(epoll_ctl(net->epoll, EPOLL_CTL_ADD, socket, &e) < 0) {
		printf("Listener epoll failed"); }


int netsocket(struct net_st * net, int type, uint16_t port, peer_fun fun) 
	int sock = socket(AF_INET6, type, 0);
	
	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	
	int r = true;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	if(bind(sock, (struct sockaddr *) &def, sizeof(struct sockaddr_in6))) {
		printf("Bind failed!\n"); }

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	 e.data.ptr = net->peers + net->peers_length;
	net->peers_length++;
	p->fd = sock;

	if(type == SOCK_STREAM) {
		struct peer_data_listen * p = e.data.ptr;
		p->fd = sock;
		p->fun = fun;
		p->fun2 = netaccept; }
	else {
		struct peer_data * p = e.data.ptr;
		p->fd = sock;
		p->fun = fun; }

	if(epoll_ctl(net->epoll, EPOLL_CTL_ADD, socket, &e) < 0)
		printf("Listener epoll failed");

	return sock; }


void netsetup(struct net_st * net) {
	net->epoll = epoll_create(1);
	net->events = malloc(sizeof(struct epoll_event) * 1024);
	net->peers = malloc(sizeof(struct peer_data) * 1024);
	net->peers_length = 0;
	net->peers_max = 1024; }

void net(struct net_st * net) {

	while(true) {
		int events_ready = epoll_wait(net->epoll, net->events, 1024, -1);

		for(int i = 0; i < events_ready; i++) {
			int events = net->events[i].evens;
			struct peer_data * d = net->events[i].ptr;

			if(events & EPOLL_HUP) {
				close(d->fd); }
			else {
				d->fun(d, events); } } } }
					


	


