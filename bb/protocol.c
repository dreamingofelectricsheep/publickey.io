
#include <inttypes.h>

typedef enum {
	protocol_suite_none,
	protocol_suite_echelon } protocol_suite;

typedef enum {
	protocol_tag_plaintext,
	protocol_tag_ciphertext,
	protocol_tag_
	protocol_tag_suite } protocol_tag;

typedef struct {
	uint16_t length;
	uint16_t type; } protocol_packet_header;

typedef struct {
	protocol_packet_header header;
	protocol_suite suite; }
