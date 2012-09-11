#include <sys/epoll.h>

struct objdata;
typedef int (*objfun)(int epoll, struct objdata * blob);
struct objdata {
	int fd;
	objfun in, err, hup, kill; };

struct objdata_ex {
	struct objdata data;
	void * ex; };
	
int prepare_objdata_ex(struct objdata_ex * o, int fd, 
	objfun in, objfun err, objfun hup, objfun kill, void * d) {
	o->data.fd = fd;
	o->data.in = in;
	o->data.err = err;
	o->data.hup = hup;
	o->data.kill = kill;
	o->ex = d; }
	
int eventloop(int epollfd) {
	int max_events = 1024;
	struct epoll_event * buffer = malloc(max_events * sizeof(*buffer));
	while(true) {
		int ready = epoll_wait(epollfd, buffer, max_events, -1);

		debug("Events ready: %d", ready);
		for(int i = 0; i < ready; i++) {
			int events = buffer[i].events;
			struct objdata * d = buffer[i].data.ptr;
			objfun fun;
			int r = 0;
			
			if(events & EPOLLERR) r += d->err(epollfd, d);
			else {
				if(events & EPOLLIN) r += d->in(epollfd, d);
				if(events & EPOLLHUP) r += d->hup(epollfd, d); }

			if(r) { 
				debug("Socket callback failed")
				d->kill(epollfd, d); }
			}} }


int epoll_add(int epoll, int fd, void * data) {
		struct epoll_event e;
		e.events = EPOLLIN; // | EPOLLET;
		e.data.ptr = data;

		if(epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &e) < 0) {
			debug("Stdin epoll_ctl failed"); 
			return -1; }
		else return 0; }

int kill(int epoll, struct objdata * data) {
	// Don't worry, epoll concatenates events on the same destriptor.
	debug("Terminating!");
	close(data->fd);
	return 0; }

int err(int epoll, struct objdata * data ) {
	debug("Receiving error: %s", strerror(errno));
	data->kill(epoll, data);
	return 0; }

int hup(int epoll, struct objdata * data ) {
	debug("Hangup received.");
	data->kill(epoll, data);
	return 0; }


