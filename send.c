#include <stdio.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	size_t length;
	uint8_t * bytes; } utf8_t;

void print(utf8_t s) {
	write(1, s.bytes, s.length); }


int main(int argc, char * argv) {


	uint8_t bu[1024 * 16];

	ssize_t re = read(0, bu, 1024 * 16);

	
	write(1, bu, re);	


	/*
	int f = open("./hello", O_RDWR);

	void * p = mmap((void *) 111111111111111, 10, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);

	printf("%lld", (uint64_t) p);

	*((uint8_t *) p) = 'B';	

	int s = socket(AF_INET6, SOCK_STREAM, 0);//SOCK_NONBLOCK);

	struct sockaddr_in6 a = {
		AF_INET6,
		htons(80),
		0, 0, 0 };

	if(bind(s, (struct sockaddr *) &a, sizeof(struct sockaddr_in6))) {
		printf("bind failed\n"); }

	if(listen(s, 100)) {
		printf("listen failed\n"); }

	printf("Listening...\n");
	int b = -1;
	while(b == -1) {
		b = accept(s, 0, 0); }

	printf("Accepted!...\n");
	uint8_t buffer[1025];
	buffer[1024] = 0;
	while(1) {
		ssize_t r = recv(b, buffer, 1024, 0);

		if(r > 0) {
			printf(buffer); } }
	*/

	return 0; }
