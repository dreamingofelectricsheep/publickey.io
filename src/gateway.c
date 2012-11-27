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

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/modes.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>


int inmail(int epoll, struct objdata * data) {
	uint8_t buffer[4096];
	debug("Received data.");
	ssize_t length = recv(data->fd, buffer, 4096, 0);
	debug("%zd bytes of data.", length);
	if(length <= 0 || length > 4096) {
		debug("Ending connection.")
		kill(epoll, data);
		return 0; }
	write(0, buffer, length);
	char * m = "250 Ok\r\n";
	write(data->fd, m, strlen(m));
	return 0;
}

int inconn(int epoll, struct objdata * data) {
	debug("Accepting a new connection.");
	struct sockaddr_in6 def;
	socklen_t len = sizeof(def);
	struct objdata_ex * o = (void *) data;
	struct p2p_st * p2p = o->ex;

	int socket = accept(data->fd, (struct sockaddr *) &def, &len);

	struct objdata_ex * d = malloc(sizeof(struct objdata_ex));
	prepare_objdata_ex(d, socket, &inmail, &err, &hup, &kill, p2p);

	epoll_add(epoll, socket, d);

	char * m = "220 outerechelon.org\r\n";
	write(socket, m, strlen(m));
	return 0;
}


int main(int argc, char ** argv) {

	int epoll = epoll_create(1);
	uint16_t port = 8084;
	uint16_t mailport = 25;
	
	struct p2p_st p2p;
	p2pprepare(epoll, port, 0, 0, &p2p, &p2p);


	int listener = socket(AF_INET6, SOCK_STREAM, 0);

	prepare_socket(listener, mailport);
	

	struct objdata_ex d;
	prepare_objdata_ex(&d, listener, &inconn, &err, &hup, &kill, &p2p);

	epoll_add(epoll, listener, &d);

	listen(listener, 1024);


	eventloop(epoll);

	return 0; }


	


