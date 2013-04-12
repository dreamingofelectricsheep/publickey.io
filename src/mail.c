#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#include "bytes.c"

#include "base64.c"
#include "epoll.c"
#include "crc.c"

int setup_socket(uint16_t port, void *ondata, void *onclose)
{
	int sock = socket(AF_INET6, SOCK_STREAM, 0);

	int r = true;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r));

	struct sockaddr_in6 def = {
		AF_INET6,
		htons(port),
		0, 0, 0
	};

	if (bind(sock, (void *)&def, sizeof(def))) {
		debug("Bind failed!");
		return -1;
	}

	listen(sock, 1024);

	struct generic_epoll_object *object = malloc(sizeof(*object));

	object->fd = sock;
	object->ondata = ondata;
	object->onclose = onclose;

	epoll_add(sock, object);

	return sock;
}

struct http_connection;

typedef void (*http_fn) (struct http_connection *);

struct http_connection {
	int socket;
	http_fn ondata;
	http_fn onclose;
	bytes buffer;
};

void http_onclose(struct http_connection *http)
{
	debug("Closing http connection: %d", http->socket);
	free(http->buffer.as_void);
	close(http->socket);
	free(http);
}

void http_ondata(struct http_connection *http)
{
	ssize_t len = recv(http->socket,
			   http->buffer.as_void + http->buffer.len,
			   4096 - http->buffer.len, 0);

	if (len == 0) {
		http->onclose(http);
	} else if (len < 0) {
		debug("Read error! %s", strerror(errno));
		http->onclose(http);
	} else {
		http->buffer.len += len;

		bfound f = bfind(http->buffer, Bs("\r\n\r\n"));

		if (f.found.len == 0) {
			debug("Partial http header.");
			return;
		}

		bytes header = f.before;
		bytes body = f.after;

		if (http->buffer.as_char[0] == 'G') {
			f = bfind(header, Bs(" "));
			f = bfind(f.after, Bs(" "));

			bytes resource = f.before;

			bytes addr = balloc(256);

			addr = bprintf(addr, "./html/%*s", resource.len, resource.as_char).first;

			int f = open(addr.as_char, O_RDWR);
			bytes page = balloc(1 << 20);
			page.length = read(f, page.as_void, page.length);

			bytes resp = balloc(4096);
			resp = bprintf(resp,
				"HTTP/1.1 200 Ok\r\n"
				"Content-Length: %zd\r\n\r\n", page.len).first;

			send(http->socket, resp.as_void, resp.len, 0);
			send(http->socket, page.as_void, page.len, 0);

	
			goto complete_request;

		} else if (http->buffer.as_char[0] == 'P') {
			f = bfind(header, Bs(" "));
			f = bfind(f.after, Bs(" "));
			bytes addr = bslice(f.before, 1, 0);

			if (addr.len == 0)
				goto bad_request;

			f = bfind(header, Bs("Content-Length: "));

			if (f.found.len == 0)
				goto bad_request;

			f = bfind(f.after, Bs("\r\n"));

			int contentlen = btoi(f.before);

			if (contentlen <= 0)
				goto bad_request;

			if (body.len < contentlen) {
				debug("Partial body. %zd out of %zd received.",
				      body.len, contentlen);
				return;
			}

			bytes reply = balloc(4096);
			reply.length = snprintf(reply.as_char, 4096,
				"Content-Length: %zd\r\n"
				"Recipient: ", body.len);
			bytes finish = Bs("\r\n\r\n");



			bfree(&reply);



			goto complete_request;
		} else {
			goto bad_request;
		}
	}
	return;

 bad_request:
	debug("Bad request");

	bytes resp = Bs("HTTP/1.1 400 Bad Request\r\n\r\n");
	send(http->socket, resp.as_void, resp.len, 0);
	http->onclose(http);
	goto complete_request;

 complete_request:
	http->buffer.len = 0;
}

void httplistener_ondata(struct generic_epoll_object *data)
{
	int socket = data->fd;
	int accepted = accept(socket, 0, 0);
	debug("Accepting a new connection: %d", accepted);
	struct http_connection *c = malloc(sizeof(*c));
	c->socket = accepted;
	c->ondata = http_ondata;
	c->onclose = http_onclose;
	c->buffer.len = 0;
	c->buffer.as_void = malloc(4096);

	epoll_add(accepted, c);
}

void httplistener_onclose(struct generic_epoll_object *data)
{
	debug("Closing listener socket. %d", data->fd);
	close(data->fd);
	free(data);
};

void stream_ondata(struct generic_epoll_object *stream) 
{
	char buffer[4096];
	ssize_t r = recv(stream->fd, buffer, 4096, 0);
	write(0, buffer, r);
}

void stream_onclose(struct generic_epoll_object *stream)
{
	debug("Closing stream socket. %d", stream->fd);
	close(stream->fd);
	free(stream);
};



int main(int argc, char **argv)
{
	epoll = epoll_create(1);

	int http = setup_socket(8081, httplistener_ondata,
				httplistener_onclose);

	int stream = socket(AF_INET6, SOCK_STREAM, 0);
	
	struct sockaddr_in6 addr = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(8080),
		.sin6_addr = 0,
		.sin6_scope_id = 0,
		.sin6_flowinfo = 0 };

	connect(stream, (void*) &addr, sizeof addr);

	struct generic_epoll_object *object = malloc(sizeof(*object));

	object->fd = stream;
	object->ondata = stream_ondata;
	object->onclose = stream_onclose;

	epoll_add(stream, object);

	epoll_listen();
	return 0;
}
