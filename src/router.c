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

const size_t cache_key_bytes = 33;

typedef struct {
	uint8_t data[33];
} cachekey;

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

struct stream_connection;

typedef void (*stream_fn) (struct stream_connection *);

struct stream_connection {
	int socket;
	size_t streamid;
	stream_fn ondata;
	stream_fn onclose;
};

void stream_onclose(struct stream_connection *stream)
{
	debug("Closing stream connection: %d", stream->socket);
	close(stream->socket);
	free(stream);
}

int stream[1024];
size_t streamlen = 0;

void stream_ondata(struct stream_connection *stream)
{
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
			

			if(bcmp(f.before, Bs("/stream")) == 0) {
				debug("NEW STREAMING SOCKET!");
				stream[streamlen] = http->socket;
				streamlen++;

					
			}

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


			for(int i = 0; i <streamlen; i++) {
				send(stream[i], reply.as_void, reply.len, 0);
				send(stream[i], addr.as_void, addr.len, 0);
				send(stream[i], finish.as_void, finish.len, 0);
				send(stream[i], body.as_void, body.len, 0);
			}

			bfree(reply);



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
};


int main(int argc, char **argv)
{

	epoll = epoll_create(1);

	int http = setup_socket(8080, httplistener_ondata,
				httplistener_onclose);

	epoll_listen();
	return 0;
}
