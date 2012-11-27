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


int sendin(struct sockaddr_in6 addr, bytes data, void * ad) {
	return 0; }

struct html_st {
	struct objdata data;
	struct p2p_st * p2p; };

// Clients have their own "in" callback.
int clientin(int epoll, struct objdata * data) {
	char buffer[4096];
	debug("Received data.  ------------");
	
	struct html_st * h = (void*)data;

	ssize_t length = recv(data->fd, buffer, 4096, 0);
	debug("%ld bytes of data.", length);
	if(length <= 0 || length > 4096) {
		debug("Ending connection.")
		kill(epoll, data);
		return 0; }

	write(0, buffer, length);

	bytes rec = { buffer, length };

	
	if(buffer[0] == 'G') {
		if(buffer[5] == ' ') {
			int f = open("../src/mailbox.html", 0);
			bytes page = balloc(1 << 20);
			page.length = read(f, page.as_void, page.length);

			bytes num = balloc(16);
			num = bfromint(num, page.length);
			bytes h1 = Bs("HTTP/1.1 200 OK\r\n"
				"Content-Length: ");
			bytes h2 = Bs("\r\n"
				"Content-Type: text/html;charset=UTF-8\r\n"
				"\r\n");
			send(data->fd, h1.as_void, h1.length, 0);
			send(data->fd, num.as_void, num.length, 0);
			send(data->fd, h2.as_void, h2.length, 0);
			send(data->fd, page.as_void, page.length, 0);
			
			close(f);
			bfree(page); 
		}
		else if(buffer[5] == 'f') {
			int f = open("../src/fontawesome-webfont.ttf", 0);
			bytes page = balloc(1 << 20);
			page.length = read(f, page.as_void, page.length);

			bytes num = balloc(16);
			num = bfromint(num, page.length);
			bytes h1 = Bs("HTTP/1.1 200 OK\r\n"
				"Content-Length: ");
			bytes h2 = Bs("\r\n"
				"Content-Type: text/html;charset=UTF-8\r\n"
				"\r\n");
			send(data->fd, h1.as_void, h1.length, 0);
			send(data->fd, num.as_void, num.length, 0);
			send(data->fd, h2.as_void, h2.length, 0);
			send(data->fd, page.as_void, page.length, 0);
			
			close(f);
			bfree(page); 
		}
		else if(buffer[5] == 'g') {
			int f = open("../src/gray_sand.png", 0);
			bytes page = balloc(1 << 20);
			page.length = read(f, page.as_void, page.length);

			bytes num = balloc(16);
			num = bfromint(num, page.length);
			bytes h1 = Bs("HTTP/1.1 200 OK\r\n"
				"Content-Length: ");
			bytes h2 = Bs("\r\n"
				"Content-Type: text/html;charset=UTF-8\r\n"
				"\r\n");
			send(data->fd, h1.as_void, h1.length, 0);
			send(data->fd, num.as_void, num.length, 0);
			send(data->fd, h2.as_void, h2.length, 0);
			send(data->fd, page.as_void, page.length, 0);
			
			close(f);
			bfree(page); 
		}
		else {
			bytes h1 = Bs("HTTP/1.1 404 Not Found\r\n"
				"Content-Length: 0\r\n\r\n");
			send(data->fd, h1.as_void, h1.length, 0);
		}
	}
	else if(buffer[0] == 'P') {
		bfound f = bfind(rec, Bs("\r\n\r\n"));
		p2psend(h->p2p, f.after, p2prouter);
		char * res = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
		send(data->fd, res, strlen(res), 0); }

	debug("----------------------------");
	return 0; }



int listenerin(int epoll, struct objdata * data) {
	debug("Accepting a new connection.");
	struct sockaddr_in6 def;
	socklen_t len = sizeof(def);

	int socket = accept(data->fd, (struct sockaddr *) &def, &len);

	struct html_st * r = (void*)data;
	struct html_st * s = malloc(sizeof(*s));
	prepare_objdata(&s->data, socket, &clientin, &err, &hup, &kill);
	s->p2p = r->p2p;
	
	
	epoll_add(epoll, socket, s);
	return 0;
}

int main(int argc, char ** argv) {

	int epoll = epoll_create(1);
	int listener = socket(AF_INET6, SOCK_STREAM, 0);
	prepare_socket(listener, 80);

	struct p2p_st p2p;
	p2pprepare(epoll, 8082, 0, &sendin, 0, &p2p);	

		

	struct html_st s = { { listener, &listenerin, &err, &hup, &kill }, &p2p };

	epoll_add(epoll, listener, &s);

	listen(listener, 1024);


	eventloop(epoll);
	return 0; }


	


