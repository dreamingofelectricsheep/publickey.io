int tcp_socket(uint16_t port, void *ondata, void *onclose, void *auxilary)
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
	object->auxiliary = auxilary;

	epoll_add(sock, object);

	return sock;
}

typedef int (*tcp_fn)(struct tcp_connection *);

struct tcp_connection 
{
	int socket;
	tcp_fn ondata, onclose;
	struct in6_addr ip;
	time_t last;
	bytes buffered;
	size_t allocated;
};

const size_t tcp_chuck_len = 8192;

struct tcp_ondata_handler_result
{
	bytes response;
	size_t consumed;
};

struct tcp_ondata_handler_result tcp_ondata_handler(bytes buffer);

void tcp_ondata(struct tcp_connection *tcp)
{
	bytes *buffer = tcp->buffer;

	if(tcp->allocated - buffer->len == 0)
	{
		realloc(buffer->as_void, buffer->len + tcp_chunk_len);
		tcp->allocated += tcp_chunk_len;
	}
	
	int len = recv(tcp->socket, buffer->as_void + buffer->len, 
		tcp->allocated - buffer->len, 0);

	debug("Received %d bytes of data from socket %d", len, tcp->socket);

	if (len <= 0)
	{
		if (len < 0)
			debug("Error reading data from socket %d: %s",
			      tcp->socket, strerror(errno));

		tcp->onclose(tcp);
	}

	buffer->len += len;

	struct tcp_ondata_handler_result result =
		tcp_ondata_handler(*buffer);

	if(result.consumed > 0)
	{
		memcpy(bytes->as_void, bytes->as_void + consumed,
			bytes->len - consumed);
		bytes->len -= consumed;

		send(tcp->socket, result.response.as_void, result.response.len, 0);
		bfree(&result.response);
	}

	return 0;
}

void tcp_onclose(struct tcp_connection *tcp)
{
	debug("Closing http connection: %d", tcp->socket);
	bfree(tcp->buffer.read);
	close(tcp->socket);
}




void tcp_onsetup(int socket, struct tcp_connection *tcp)
{
	struct sockaddr_in6 addr;
	socklen_t len = sizeof(addr);

	int accepted = accept(socket, (void *)&addr, &len);

	debug("Accepting a new tcp connection: %d", accepted);
	tcp->socket = accepted;
	memcpy(&tcp->ip, &addr.sin6_addr, sizeof(addr.sin6_addr));

	tcp->buffer = balloc(tcp_chunk_len);
	tcp->available = tcp_chunk_len;
}



void tcp_server_ondata(struct generic_epoll_object *data)
{
	struct tcp_connection *tcp = malloc(sizeof(*tcp));

	tcp_onsetup(data->fd, &con->tcp);
	epoll_add(tcp->socket, tcp);

	debug("Done with the connection setup.");
}

void tcp_server_onclose(struct generic_epoll_object *data)
{
	debug("Closing listener socket. %d", data->fd);
	close(data->fd);
	free(data);
};



