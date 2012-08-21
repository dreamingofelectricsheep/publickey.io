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

#define debug(d...) { printf("[f: %s l: %d] ", __FILE__, __LINE__); \
	printf(d); printf("\n"); }

void makehandshake() {


}
	

int main(int argc, char ** argv) {

	uint16_t port = 25;
	int epoll = epoll_create(1);
	struct epoll_event events[1024];

	int listener = socket(AF_INET6, SOCK_STREAM, 0);
	
	int r = true;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	struct sockaddr_in6 def = { AF_INET6, htons(port), 0, 0, 0 };
	if(bind(listener, (struct sockaddr *) &def, sizeof(def))) {
		debug("Bind failed!");
		return -1; }

	
	struct sockdata;
	typedef int (*sockfun)(int epoll, struct sockdata * blob);
	struct sockdata {
		int fd;
		sockfun in, err, hup, kill; };


	int kill(int epoll, struct sockdata * data) {
		// Don't worry, epoll concatenates events on the same destriptor.
		debug("Terminating!");
		close(data->fd);
		free(data);
		return 0; }

	int err(int epoll, struct sockdata * data ) {
		debug("Receiving error: %s", strerror(errno));
		data->kill(epoll, data);
		return 0; }

	int hup(int epoll, struct sockdata * data ) {
		debug("Hangup received.");
		data->kill(epoll, data);
		return 0; }


	int in(int epoll, struct sockdata * data) {
		debug("Accepting a new connection.");
		struct sockaddr_in6 def;
		socklen_t len = sizeof(def);

		int socket = accept(data->fd, (struct sockaddr *) &def, &len);

		int in_first(int epoll, struct sockdata * data) {
			uint8_t buffer[4096];
			debug("Received data.");
			ssize_t length = recv(data->fd, buffer, 4096, 0);
			debug("%d bytes of data.", length);
			if(length <= 0 || length > 4096) {
				debug("Ending connection.")
				kill(epoll, data);
				return 0; }
			write(0, buffer, length);
			char * m = "250 Ok\r\n";
			write(data->fd, m, strlen(m));
			return 0;
		}

		struct sockdata * d = malloc(sizeof(struct sockdata));
		*d = (struct sockdata) { socket, &in_first, &err, &hup, &kill };
		
		struct epoll_event e;
		e.events = EPOLLIN | EPOLLET;
		e.data.ptr = d;

		if(epoll_ctl(epoll, EPOLL_CTL_ADD, socket, &e) < 0) {
			debug("Accept epoll_ctl failed"); return -1; }

		char * m = "220 outerechelon.org\r\n";
		write(socket, m, strlen(m));
		return 0;
	}
		

	struct sockdata listenerdata = { listener, &in, &err, &hup, &kill };

	
	struct epoll_event e;
	e.events = EPOLLIN | EPOLLET;
	e.data.ptr = &listenerdata;

	if(epoll_ctl(epoll, EPOLL_CTL_ADD, listener, &e) < 0) {
		debug("Listener epoll_ctl failed"); return -1; }

	listen(listener, 1024);


	while(true) {
		int ready = epoll_wait(epoll, events, 1024, -1);

		debug("Events ready: %d", ready);
		for(int i = 0; i < ready; i++) {
			int ev = events[i].events;
			struct sockdata * d = events[i].data.ptr;
			sockfun fun;
			
			if(ev & EPOLLHUP) fun = d->hup;
			else if(ev & EPOLLERR) fun = d->err;
			else if(ev & EPOLLIN) fun = d->in;

			if(fun(epoll, d)) { 
				debug("Socket callback failed")
				d->kill(epoll, d); }
			}}

	return 0; }


	


