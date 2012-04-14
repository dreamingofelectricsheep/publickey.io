#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

typedef struct {
	uint16_t length;
	uint16_t tag; } packet_header;

typedef enum {
	packet_tag_plaintext } packet_tag;

int main(int argc, char ** argv) { 

	if(argc < 2) return 0;

	if(strcmp(argv[1], "pack") == 0) {
		const size_t l = 1024 * 1024;
		uint8_t * buffer = malloc(l);
		ssize_t r = read(0, buffer + sizeof(packet_header), l - sizeof(packet_header));

		if(r <= 0) write(1, "error", 5);
		
		*((packet_header *) buffer) = (packet_header) { r, atoi(argv[2]) };

		write(1, buffer, r + sizeof(packet_header));
		free(buffer); }


	return 0; }
