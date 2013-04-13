#ifndef EPOLL_C
#define EPOLL_C
#include <time.h>

int epoll = 0;
int epoll_stop = false;
int epoll_time = 0;

int epoll_add(int fd, void *data)
{
	// Lever triggered mode - epoll events fire as long
	// as there is data available for reading.
	struct epoll_event e;
	e.events = EPOLLIN | EPOLLRDHUP;
	e.data.ptr = data;

	if (epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &e) < 0) {
		debug("Stdin epoll_ctl failed");
		return -1;
	}

	return 0;
}

struct generic_epoll_object;

typedef void (*epoll_fn) (struct generic_epoll_object *);

struct generic_epoll_object
{
	int fd;
	epoll_fn ondata;
	epoll_fn onclose;
	void *auxiliary;
};

void epoll_listen()
{
	int max_events = 1024;
	struct epoll_event buffer[max_events];

	while (epoll_stop == false)
	{
		int ready = epoll_wait(epoll, buffer, max_events, -1);

		// To avoid making multiple syscalls, we get the time once per
		// epoll tick.
		epoll_time = time(0);

		for (int i = 0; i < ready; i++)
		{
			int events = buffer[i].events;
			int socket = *(int *)buffer[i].data.ptr;

			struct generic_epoll_object *object =
			    buffer[i].data.ptr;

			if (events & EPOLLERR)
			{
				debug("Error reported: %s", strerror(errno));
				object->onclose(object);
			} 
			else if (events & EPOLLIN)
			{
				object->ondata(object);
			}
			else if (events & EPOLLHUP || events & EPOLLRDHUP)
			{
				debug("Hangup received on socket %d", socket);
				object->onclose(object);
			}
		}
	}

}
#endif
