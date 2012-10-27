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

#include "bytes.c"
#include "p2p.c"

int inclient(int epoll, struct objdata * data) {
	uint8_t buffer[4096];
	debug("Received data.  ------------");
	ssize_t length = recv(data->fd, buffer, 4096, 0);
	debug("%zd bytes of data.", length);
	if(length <= 0 || length > 4096) {
		debug("Ending connection.")
		kill(epoll, data);
		return 0; }

	write(0, buffer, length);
	if(buffer[0] == 'G') {
		char * page = "HTTP/1.1 200 OK\r\n"
"Content-Length: 5\r\n"
"Content-Type: text/html\r\n"
"\r\nhello";
		send(data->fd, page, strlen(page), 0); }
	debug("----------------------------");
	return 0; }


int in(int epoll, struct objdata * data) {
	debug("Accepting a new connection.");
	struct sockaddr_in6 def;
	socklen_t len = sizeof(def);

	int socket = accept(data->fd, (struct sockaddr *) &def, &len);

	// Clients have their own "in" callback.
	struct objdata * d = malloc(sizeof(struct objdata));
	*d = (struct objdata) { socket, &inclient, &err, &hup, &kill };
	
	epoll_add(epoll, socket, d);

	return 0;
}
	

int main(int argc, char ** argv) {

	int epoll = epoll_create(1);

	int listener = socket(AF_INET6, SOCK_STREAM, 0);
	
	prepare_socket(listener, 80);

	struct objdata listenerdata = { listener, &in, &err, &hup, &kill };


	epoll_add(epoll, listener, &listenerdata);
	listen(listener, 1024);


	eventloop(epoll);
	return 0; }


	


