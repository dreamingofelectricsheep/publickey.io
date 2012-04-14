#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

typedef struct {
	size_t length;
	uint8_t * string; } utf8_t;

typedef enum {
	pgp_public_key_algorithm_rsa = 1,
	pgp_public_key_algorithm_rsa_encrypt_only,
	pgp_public_key_algorithm_rsa_sign_only,
	pgp_public_key_algorithm_elgamal = 16,
	pgp_public_key_algorithm_dsa,
	pgp_public_key_algorithm_elliptic_curve,
	pgp_public_key_algorithm_ecdsa,
	pgp_public_key_algorithm_diffie_hellman } pgp_public_key_algorithm_t;

typedef enum {
	pgp_tag_reserved,
	pgp_tag_public_key_encrypted_session_key_packet,
	pgp_tag_signature_packet,
	pgp_tag_symmetric_key_encrypted_session_key_packet,
	pgp_tag_one_pass_signature_packet,
	pgp_tag_secret_key_packet,
	pgp_tag_public_key_packet,
	pgp_tag_secret_subkey_packet,
	pgp_tag_compressed_data_packet,
	pgp_tag_symmetrically_encrypted_data_packet,
	pgp_tag_marker_packet,
	pgp_tag_literal_data_packet,
	pgp_tag_trust_packet,
	pgp_tag_user_id_packet,
	pgp_tag_public_subkey_packet,
	pgp_tag_user_attribute_packet,
	pgp_tag_sym_encrypted_and_integrity_protected_data_packet,
	pgp_tag_modification_detection_code_packet } pgp_packet_tag_t;

typedef enum {
	pgp_symmetric_key_algorithm_unencrypted,
	pgp_symmetric_key_algorithm_idea,
	pgp_symmetric_key_algorithm_triple_des,
	pgp_symmetric_key_algorithm_cast5,
	pgp_symmetric_key_algorithm_blowfish,
	pgp_symmetric_key_algorithm_aes_128 = 7,
	pgp_symmetric_key_algorithm_aes_192,
	pgp_symmetric_key_algorithm_aes_256,
	pgp_symmetric_key_algorithm_twofish } pgp_symmetric_key_algorithm_t;

typedef enum {
	pgp_compression_algorithm_uncompressed,
	pgp_compression_algorithm_zip,
	pgp_compression_algorithm_zlib,
	pgp_compression_algorithm_bzip2 } pgp_compression_algorithm_t;

typedef enum {
	pgp_hash_algorithm_md5 = 1,
	pgp_hash_algorithm_sha1,
	pgp_hash_algorithm_ripemd160,
	pgp_hash_algorithm_sha256 = 8,
	pgp_hash_algorithm_sha384,
	pgp_hash_algorithm_sha512,
	pgp_hash_algorithm_sha224 } pgp_hash_algorithm_t

typedef struct {
	pgp_packet_tag_t tag;
	size_t length; } pgp_packet_header_t;

void pgp_packet_extract_header(pgp_packet_header_t * header, uint8_t * bytes, size_t length) {
	const uint8_t mask = ((1 << 6) - 1)

	if((bytes[0] >> 6) != 3) return -1;

	header->tag = bytes[0] & mask;
	
	if(bytes[1] < 192) { header->length = bytes[1]; }
	else if(bytes[1] < 224)  { 
		header->length = ((bytes[1] - 192) << 8) + (bytes[2]) + 192; }
	else if(bytes[1] == 225) {
		header->length = (bytes[2] << 24) | (bytes[3] << 16) | (bytes[4] << 8)  | bytes[5]; } }

void pgp_packet_make_header(pgp_packet_header_t * header, uint8_t * bytes, size_t length, size_t body_length) {
	bytes[0] = 3 << 6;
	bytes[0] |= header->tag;

	if(body_length < 192) { bytes[1] = body_length; }
	else if(body_length < 8384) {
		bytes[1] = (body_length >> 8) + 192;
		bytes[2] = body_length - 192; }
	else {
		bytes[1] = 255;
		bytes[2] = body_length >> 24;
		bytes[3] = body_length >> 16;
		bytes[4] = body_length >> 8;
		bytes[5] = body_length; } }

void pgp_packet_extract_user_id(utf8_t * user_id, uint8_t * bytes, size_t length) {
	user_id->length = length;
	user_id->string	= (char *) bytes; }

void pgp_packet_make_user_id(utf8_t * user_id, uint8_t * bytes, size_t length) {
	for(size_t i = 0; i < user_id->length; ++i) {
		bytes[i] = user_id->string[i]; } }

typedef struct {
	uint8_t key_id[8];
	pgp_public_key_algorithm_t public_key_algorithm;
	uint8_t * encrypted_session_key;
	size_t encryted_session_key_length; } pgp_public_key_encrypted_session_key_packet_t;

void pgp_packet_extract_public_key_encrypted_session_key_packet(pgp_public_key_encrypted_session_key_packet_t * packet, uint8_t * bytes, size_t length) {
	for(size_t i = 0; i < 8; ++i) {
		packet->key_id[i] = bytes[1 + i]; }
	packet->public_key_algorithm = bytes[9];
	packet->encrypted_session_key = bytes + 10;
	packet->encryted_session_key_length = length - 10; }


void pgp_packet_make_public_key_encrypted_session_key_packet(pgp_public_key_encrypted_session_key_packet_t * packet, uint8_t * bytes) {
	bytes[0] = 3;
	for(size_t i = 0; i < 8; ++i) {
		bytes[1 + i] = packet->key_id[i]; }

	bytes[9] = packet->public_key_algorithm;
	for(size_t i = 0; i < packet->encryted_session_key_length; ++i) {
		bytes[10 + i] = packet->encrypted_session_key[i]; } }

typedef struct {
	uint16_t length;
	uint8_t * bytes; } pgp_mpi_t;

int pgp_packet_extract_mpi(pgp_mpi_t * mpi, uint8_t * bytes) {
	mpi->length = (((uint16_t *) bytes)[0] + 7) >> 3;
	mpi->bytes = bytes + 2;
	return mpi->length + 2; }

void pgp_packet_make_mpi(pgp_mpi_t * mpi, uint8_t * bytes) {
	size_t first_bit = 7;
	for(; first_bit > 0; first_bit--) {
		if(mpi[0] >> first_byte) { break; } }

	((uint16_t *) bytes)[0] = mpi->length * 8 + first_bit + 1 - 8;
	
	memcpy(mpi->bytes, bytes + 2, mpi->length); }
		

typedef struct {
	pgp_public_key_algorithm_t public_key_algorithm;
	uint8_t algorithm_specific_data[sizeof(pgp_mpi_t) * 2];
	} pgp_public_key_packet_t;

typedef struct {
	pgp_mpi_t modulus;
	pgp_mpi_t exponent; } pgp_rsa_public_key_t;

int pgp_packet_extract_public_key_packet(pgp_public_key_packet_t * packet, uint8_t * bytes) {
	packet->public_key_algorithm = bytes[5];
	
	switch(packet->public_key_algorithm) {
		case pgp_public_key_algorithm_rsa:
			pgp_rsa_public_key_t * rsa = (pgp_rsa_public_key_t *) &packet->algorithm_specific_data;
			size_t modulus = pgp_packet_extract_mpi(&rsa->modulus, bytes + 7);
			size_t exponent = pgp_packet_extract_mpi(&rsa->exponent, bytes + 7 + 2 + rsa->modulus.length);
			return modulus + exponet + 6; }; }

void pgp_packet_make_public_key_packet(pgp_public_key_packet_t * packet, uint8_t * bytes) {
	for(size_t i = 0; i < 5; i++) { bytes[i] = 0; }

	bytes[5] = packet->public_key_algorithm;

	switch(packet->public_key_algorithm) {
		case pgp_public_key_algorithm_rsa:
			pgp_rsa_public_key_t * rsa = (pgp_rsa_public_key_t *) &packet->algorithm_specific_data;
			pgp_packet_make_mpi(&rsa->modulus, bytes + 6);
			pgp_packet_make_mpi(&rsa->exponent, bytes + 2 + rsa->modulus.length); }; }

typedef struct { 
	pgp_public_key_packet_t public_key;
	} pgp_secret_key_packet_t;

int pgp_packet_extract_secret_key_packet(pgp_secret_key_packet_t * packet, uint8_t * bytes) {
	size_t offset = pgp_packet_extract_public_key_packet(packet->public_key, bytes);

	bytes = bytes + offset;

			

typedef struct __dbnode_header {
	struct __dbnode_header * left, * right; 
	uint8_t * payload; } dbnode_header_t;

typedef enum { lesser, equal, greater } cmp_result_t;

typedef cmp_result_t (* comparator_t)(uint8_t *, uint8_t *);

int dbinsert(dbnode_header_t ** n, size_t key_size, comparator_t cmp, uint8_t * key, uint8_t * payload) {
	if(*n) { switch(cmp(key, (uint8_t *)((*n) + 1))) {
		case lesser: dbinsert(&(*n)->left, key_size, cmp, key, payload); return 0;
		case greater: dbinsert(&(*n)->right, key_size, cmp, key, payload); return 0;
		default: return 0; }
	} else { 
		(*n) = malloc(sizeof(dbnode_header_t) + key_size);
		(*n)->left = (dbnode_header_t *) 0;
		(*n)->right = (dbnode_header_t *) 0;
		(*n)->payload = payload;
		memcpy((uint8_t *) ((*n) + 1), key, key_size);
		return 1; } }

int dbremove(dbnode_header_t ** n, size_t key_size) {
	}

typedef void (* dbfunc_t)(uint8_t * );

void dbinorder(dbnode_header_t * n, dbfunc_t foo) {
	if(n) {
		dbinorder(n->left, foo);
		foo(n->payload);
		dbinorder(n->right, foo); } }
	
const size_t dbkey_length = 160 / 8;

cmp_result_t sha1cmp(uint8_t * first, uint8_t * second) {
	for(size_t i = 0; i < dbkey_length; ++i) {
		if(first[i] < second[i]) { return lesser; }
		else if(first[i] > second[i]) { return greater; } }

	return equal }

typedef struct {
	size_t length;
	uint8_t * free; } pool_t;

uint8_t * pool_alloc(pool_t * p) {
	uint8_t * r = p->free;
	p->free = p->free[0]; }

void pool_free(pool_t * p, uint8_t * mem) {
	((uint8_t *) *mem) = p->free;
	p->free = mem; }


int main(int argc, char ** argv) {

	return 0; } 
