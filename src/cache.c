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

static int base64_char(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 26;
	if (c >= '0' && c <= '9')
		return c - '0' + 52;
	if (c == '+')
		return 62;
	if (c == '/')
		return 63;
	if (c == '=')
		return 0;

	return -1;
}

bytes base64_decode(bytes base64, bytes mem)
{
	uint8_t n[4];
	int k = 0;

	if (mem.len < base64.len)
		return (bytes) {
		0, 0};

	for (int i = 0; i < base64.len; i += 4, k += 3) {
		for (int j = 0; j < 4; j++) {
			if (j + i < base64.len) {
				n[j] = base64_char(base64.as_char[i++]);
			} else {
				n[j] = 0;
			}
		}

		char *q = mem.as_char;

		q[0] = (n[0] << 2) + (n[1] >> 4);
		q[1] = ((n[1] & 15) << 4) + (n[2] >> 2);
		q[2] = ((n[2] & 3) << 6) + n[3];
	}

	return (bytes) {
	mem.as_void, k};
}

const size_t cache_key_bytes = 33;

typedef struct {
	uint8_t data[33];
} cachekey;

struct diskitem {
	time_t time;
	cachekey addr;
	size_t len;
	uint32_t checksum;
};

typedef time_t itemcachekey;

typedef struct diskitem *itemcachepayload;

typedef struct itemcache *cachepayload;

int itemcachecmp(itemcachekey * first, itemcachekey * second)
{
	return memcmp(first, second, sizeof(*first));
}

int cachecmp(cachekey * first, cachekey * second)
{
	return memcmp(first, second, sizeof(*first));
}

#define MAP(var) cache ## var
#include "map.c"
#undef MAP
#define MAP(var) itemcache ## var
#include "map.c"
#undef MAP

int epoll;
struct cache *db = 0;

int epoll_add(int fd, void *data)
{
	struct epoll_event e;
	e.events = EPOLLIN;
	e.data.ptr = data;

	if (epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &e) < 0) {
		debug("Stdin epoll_ctl failed");
		return -1;
	}

	return 0;
}

struct generic_epoll_object;

typedef void (*epoll_fn) (struct generic_epoll_object *);

struct generic_epoll_object {
	int fd;
	epoll_fn ondata;
	epoll_fn onclose;
};

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

#define CRC24_INIT 0xB704CEL
#define CRC24_POLY 0x1864CFBL

uint32_t crc(bytes data)
{
	uint32_t crc = CRC24_INIT;
	int i;

	while (data.len--) {
		crc ^= (*data.as_char++) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000) {
				crc ^= CRC24_POLY;
			}
		}
	}

	return crc & 0xFFFFFFL;
}

int max(int a, int b)
{
	return a > b ? a : b;
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

			bytes requested = f.before;

			requested = bslice(requested, 1, 0);
			bfound s = bfind(requested, Bs("/"));

			bytes addrb64 = s.before;
			bytes tmp = balloc(max(addrb64.len + 4, 33));
			memset(tmp.as_void, 0, 33);
			bytes addr = base64_decode(addrb64, tmp);

			time_t time = btoi(s.after);

			struct cache **mailbox = cachefind(&db, addr.as_void);

			if (*mailbox == 0) {
				bytes resp =
				    Bs("HTTP/1.1 404 Not Found\r\n"
				       "Content-Length: 0\r\n\r\n");
				send(http->socket, resp.as_void, resp.len, 0);
				goto complete_request;
			}

			struct itemcache *found =
			    itemcachefindnext((*mailbox)->payload,
					      &time);

			struct diskitem *mail = found->payload;
			bytes len = atob(tmp, mail->len);

			bytes resp =
			    Bs("HTTP/1.1 200 Ok\r\n" "Content-Length: ");
			bytes resp2 = Bs("\r\n\r\n");
			send(http->socket, resp.as_void, resp.len, 0);
			send(http->socket, len.as_void, len.len, 0);
			send(http->socket, resp2.as_void, resp2.len, 0);
			send(http->socket, mail + sizeof(*mail), mail->len, 0);

			bfree(tmp);
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

			bytes mem = balloc(max(addr.len + 4, 33));
			memset(mem.as_void, 0, 33);
			cachekey *key = base64_decode(addr, mem).as_void;

			struct cache **ca = cachefind(&db, key);

			if (*ca == 0) {
				void *p = 0;
				cacheinsert(&db, key, (void *)&p);
			}

			struct itemcache **ic = &((*ca)->payload);

			struct timespec t;
			clock_gettime(CLOCK_REALTIME, &t);

			itemcachepayload payload =
			    malloc(sizeof(*payload) + contentlen);

			payload->time = t.tv_sec;
			payload->addr = *key;
			payload->checksum = crc(body);
			payload->len = contentlen;

			memcpy(payload + sizeof(*payload), body.as_void,
			       contentlen);

			struct itemcache **found;

			do {
				t.tv_sec++;
				found = itemcachefind(ic, &t.tv_sec);
			}
			while (*found != 0);

			itemcacheinsert(ic, &t.tv_sec, &payload);

			bfree(mem);
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
/*	
	if(argc != 2) printf("Usage:\nbin-name /path/to/disk/cache\n");

	int fd = open(argv[1], O_RDWR);
	if(fd < 0) {
		printf("Can not open file %s\n", argv[1]);
		exit(0); }
	
	struct stat st;
	fstat(fd, &st);
	printf("Cache will use %ld bytes.\n", st.st_size);

	void * memcache = mmap(0, st.st_size, 
		PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, 0);

	if(memcache == (void *) -1) {
		printf("Mmap failed.\n");
		exit(0); }

	struct cachenode * cachetree = 0;
*/
	epoll = epoll_create(1);

	int http = setup_socket(8081, httplistener_ondata,
				httplistener_onclose);

	int max_events = 1024;
	struct epoll_event buffer[max_events];

	while (true) {
		int ready = epoll_wait(epoll, buffer, max_events, -1);

		for (int i = 0; i < ready; i++) {
			int events = buffer[i].events;
			int socket = *(int *)buffer[i].data.ptr;
			struct generic_epoll_object *object =
			    buffer[i].data.ptr;

			if (events & EPOLLERR) {
				debug("Error reported: %s", strerror(errno));
				object->onclose(object);
			}
			else if (events & EPOLLIN) {
				object->ondata(object);
			}
			else if (events & EPOLLHUP) {
				debug("Hangup received on socket %d", socket);
				object->onclose(object);
			}
		}
	}

	return 0;
}
