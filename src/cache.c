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

const size_t hash_bytes = 32

struct hash
{
	uint8_t data[public_key_hash_bytes];
};

struct message_header
{
	time_t time;
	size_t len;
	size_t public_keys;
};

#define MAP(var) maildb ## var
typedef struct hash MAP(key);
typedef struct message_header* MAP(payload);

int MAP(cmp)(MAP(key) * first, MAP(key) * second)
{
	return memcmp(first, second, sizeof(*first);
}
#include "map.c"
#undef MAP

struct list_node
{
	struct list_node *next;
	struct hash payload;
};

struct list
{
	struct list_node *first, *last;
};


#define MAP(var) publickeydb ## var
typedef struct hash MAP(key);
typedef struct list MAP(payload);

int MAP(cmp)(MAP(key) * first, MAP(key) * second)
{
	return memcmp(first, second, sizeof(*first);
}
#include "map.c"
#undef MAP


struct publickeydb *publickeys = 0;
struct maildb *mail = 0;


#include "tcp.c"


struct tcp_ondata_handler_result tcp_ondata_handler(bytes buffer)
{
	struct http_request req = http_parse_request(buffer);


	if(req.method == http_get)
	{
		bytes pubkey = bslice(req.addr, 1, 0);

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
	int socket = tcp_socket(8080, tcp_server_ondata, tcp_server_onclose, 0);

	epoll_listen();
	return 0;
}
