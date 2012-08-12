#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <netdb.h>

int main(int argc, char ** argv) {
	
	size_t bufferlength = 4096;
	uint8_t buffer[bufferlength];

	bufferlength = read(0, buffer, bufferlength);

	
	

	int mailman = socket(AF_INET6, SOCK_DGRAM, 0);

	{
		struct sockaddr_in6 mailman_definition = {
			AF_INET6,
			htons(8081),
			0, 0, 0 };
		
		int so_reuseaddr = true;

		setsockopt(mailman,
		       SOL_SOCKET,
		       SO_REUSEADDR,
		       &so_reuseaddr,
			sizeof so_reuseaddr);

		if(bind(mailman, (struct sockaddr *) &mailman_definition, 
			sizeof(struct sockaddr_in6))) {
			printf("Mailman bind failed\n"); }

	}

	

	struct addrinfo *postoffice_info, hint = {
		AI_V4MAPPED, AF_INET6, 0, 0, 0, 0, 0, 0 };
	

	if(0 != getaddrinfo("outerechelon.org", 0, &hint, &postoffice_info)) {
		printf("Error in getaddrinfo!"); }

	int postoffice_port = 8080;

	{
		struct sockaddr_in6 postoffice = {
			AF_INET6,
			htons(postoffice_port),
			0, ((struct sockaddr_in6 *) postoffice_info->ai_addr)->
				sin6_addr, 0 };

		
		sendto(mailman, buffer, bufferlength, 0, 
			(struct sockaddr *) &postoffice, sizeof(postoffice));
	}
	




	freeaddrinfo(postoffice_info);

	return 0; }

