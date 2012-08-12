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

// Before including this file define a struct peer_user_data.

struct net_st;
struct peer_data;
typedef void (:w*peer_fun)(struct net_st *, struct peer_data *, int events);
struct peer_data {
	int fd;
	peer_fun ondata, onhup;
	uint16_t port;
	uint8_t ip[16];
	struct peer_user_data data; };

struct peer_data_listen;
typedef void (*peer_fun_listen)(struct net_st *, struct peer_data_listen *, int events);
struct peer_data_listen {
	int fd;
	peer_fun_listen fun2;
	peer_fun fun; };

struct net_st {
	int epoll;
	struct epoll_event * events;
	struct peer_data * peers;
	size_t peers_length;
	size_t peers_max; };

void makenet(struct net_st * net) {
	net->epoll = epoll_create(1);
	net->events = malloc(sizeof(struct epoll_event) * 1024);
	net->peers = malloc(sizeof(struct peer_data) * 1024);
	net->peers_num = 0;
	net->peers_max = 1024; }


void netaccept(struct net_st * net, struct peer_data_listen * p, int events) {
	struct sockaddr_in6 def;
	socklen_t len = sizeof(def);

	int socket = accept(p->fd, (struct sockaddr *) &def, &len);

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	struct peer_data * n = e.data.ptr = net->peers + net->peers_length;
	net->peers_length++;
	n->fd = socket;
	n->fun = p->fun;
	n->port = ntohs(def.sin6_port);

	memcpy(n->ip, def.sin6_addr.s6_addr, 16);

	if(epoll_ctl(net->epoll, EPOLL_CTL_ADD, socket, &e) < 0) {
		printf("Listener epoll failed"); } }


int maketcp(struct net_st * net, uint16_t port, void * blob) {
	int sock = socket(AF_INET6, type, 0);
	
	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	
	int r = true;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	if(bind(sock, (struct sockaddr *) &def, sizeof(struct sockaddr_in6))) {
		printf("Bind failed!\n");
		return -1; }

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	e.data.ptr = blob;

	if(type == SOCK_STREAM) {
		struct peer_data_listen * p = e.data.ptr;
		p->fd = sock;
		p->fun = fun;
		p->fun2 = netaccept; }

	if(epoll_ctl(net->epoll, EPOLL_CTL_ADD, sock, &e) < 0)
		printf("Listener epoll failed");

	return sock; }


int netsocket(struct net_st * net, int type, uint16_t port) {
	int sock = socket(AF_INET6, type, 0);
	
	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	
	int r = true;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	if(bind(sock, (struct sockaddr *) &def, sizeof(struct sockaddr_in6))) {
		printf("Bind failed!\n");
		return -1; }

	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	e.data.ptr = net->peers + net->peers_length;
	net->peers_length++;

	if(type == SOCK_STREAM) {
		struct peer_data_listen * p = e.data.ptr;
		p->fd = sock;
		p->fun = fun;
		p->fun2 = netaccept; }
	else {
		struct peer_data * p = e.data.ptr;
		p->fd = sock;
		p->fun = fun; }

	if(epoll_ctl(net->epoll, EPOLL_CTL_ADD, sock, &e) < 0)
		printf("Listener epoll failed");

	return sock; }


void net(struct net_st * net) {

	while(true) {
		int events_ready = epoll_wait(net->epoll, net->events, 1024, -1);

		for(int i = 0; i < events_ready; i++) {
			printf("eventing!");
			int events = net->events[i].events;
			struct peer_data * d = net->events[i].data.ptr;

			if(events & EPOLLHUP) {
				close(d->fd); }
			else {
				d->fun(net, d, events); } } } }
					


	


