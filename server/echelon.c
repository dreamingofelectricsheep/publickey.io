#include "bytes.c"
#include <fcntl.h>

#include "flex.c"
#include "echelon-protocol.c"

struct bignum_st * readkeyfile(char * f) {
	uint8_t key[32];
	
	int keyfile = open(f, O_RDONLY);
	read(keyfile, key, 32);
	close(keyfile);

	return BN_bin2bn(key, 32, 0); }

bytes readfile(char * f) {
	bytes mem = bytesalloc(1 << 15);

	int file = open(f, O_RDONLY);
	mem.length = read(file, mem.ptr, mem.length);
	close(file);
	
	return mem; }

size_t readheader(bytes stream, uint64_t * tag, uint64_t * length) {
	int t = fromflex(stream, tag);
	stream = bytescut(stream, t, 0);
	int l = fromflex(stream, length);
	
	return t + l; }

size_t writeheader(bytes stream, uint64_t tag, uint64_t length) {
	int t = toflex(stream, tag);
	stream = bytescut(stream, t, 0);
	int l = toflex(stream, length);
	
	return t + l; }



void sign(int argc, char ** argv) {
	struct bignum_st * author = readkeyfile(argv[1]);


	// Setup the signature.
	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct bignum_st * order = BN_new();
	EC_GROUP_get_order(curve, order, 0);

	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);
	EC_KEY_set_private_key(eckey, author);


	bytes mem = readfile(argv[0]);

	uint8_t hash[32];
	SHA256(mem.ptr, mem.length, hash);


	struct ECDSA_SIG_st * sig = ECDSA_do_sign(hash, 32, eckey);


	bytes stream = bytesalloc(100);
	stream.length = writeheader(stream, ep_secp256k1_signature, 64);	

	uint8_t r[32], s[32];	
	BN_bn2bin(sig->r, r);
	BN_bn2bin(sig->s, s);

	write(1, stream.ptr, stream.length);
	write(1, r, 32);
	write(1, s, 32); }

struct ec_point_st * readpublickeyfile(char * f, struct ec_group_st * curve) {

	struct ec_point_st * publickey = EC_POINT_new(curve);

	bytes stream = bytesalloc(100);

	int file = open(f, O_RDONLY);
	read(file, stream.ptr, stream.length);
	close(file);

	uint64_t tag, length;
	int s = readheader(stream, &tag, &length);
	stream = bytescut(stream, s, 0);

	stream.raw[0] += 2;
	
	EC_POINT_oct2point(curve, publickey, stream.ptr, 33, 0);

	return publickey; }



void verify(int argc, char ** argv) {
	
	bytes mem = readfile(argv[0]);

	uint8_t hash[32];
	SHA256(mem.ptr, mem.length, hash);
	bytesfree(mem);



	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);

	struct ec_point_st * author = readpublickeyfile(argv[2], curve);

	EC_KEY_set_public_key(eckey, author);



	struct ECDSA_SIG_st * sig = ECDSA_SIG_new();

	{
		bytes stream = bytesalloc(100);

		int file = open(argv[1], O_RDONLY);
		read(file, stream.ptr, stream.length);
		close(file);

		uint64_t tag, length;
		int s = readheader(stream, &tag, &length);
		stream = bytescut(stream, s, 0);
		sig->r = BN_bin2bn(stream.raw, 32, 0);
		sig->s = BN_bin2bn(stream.raw + 32, 32, 0);
	}


	int verification = ECDSA_do_verify(hash, 32, sig, eckey);
	

	printf("%d\n", verification);
	if(verification == 1) { printf("Signature matches the public key.\n"); }
	else printf("Invalid signature.\n");
}


void genkey(int argc, char ** argv) {

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct bignum_st * order = BN_new();
	struct bignum_st * key = BN_new();

	EC_GROUP_get_order(curve, order, 0);
	BN_rand_range(key, order);

	uint8_t bytes[32];
	memset(bytes, 0, 32);

	BN_bn2bin(key, bytes);

	write(1, bytes, 32); }

void publickey(int argc, char ** argv) {
	struct bignum_st * key = readkeyfile(argv[0]);

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	

	struct ec_point_st * pub = EC_POINT_new(curve);
	EC_POINT_mul(curve, pub, 0, g, key, 0); 





	bytes out = bytesalloc(100);
	int s = writeheader(out, ep_secp256k1_public_key, 33);

	EC_POINT_point2oct(curve, pub, 2, out.raw + s, 33, 0);	

	out.raw[s] -= 2;
	write(1, out.ptr, s + 33); }



void encrypt(int argc, char ** argv) {
	
	bytes mem = readfile(argv[0]);
 
	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct ec_point_st * recipient = readpublickeyfile(argv[1], curve);

	struct bignum_st * order = BN_new();
	EC_GROUP_get_order(curve, order, 0);

	struct bignum_st * k = BN_new();
	BN_pseudo_rand_range(k, order);
	
	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	struct ec_point_st * encrypted = EC_POINT_new(curve);
	EC_POINT_mul(curve, encrypted, 0, g, k, 0); 






	bytes envelope = bytesalloc(100);
	int s = writeheader(envelope, ep_secp256k1_public_key, 33);

	EC_POINT_point2oct(curve, encrypted, 2, envelope.raw + s, 33, 0);	

	envelope.raw[s] -= 2;

	envelope.length = 33 + s;



	struct ec_point_st * secret = EC_POINT_new(curve);
	EC_POINT_mul(curve, secret, 0, recipient, k, 0);


	struct { uint8_t y; uint8_t key[32]; } secretbytes;

	EC_POINT_point2oct(curve, secret, 2, (void *) &secretbytes, 33, 0);	


	bytes out = bytesalloc(1 << 15);

	int o = writeheader(out, ep_aes256_ciphertext, mem.length);

	struct aes_key_st aeskey;
	AES_set_encrypt_key(secretbytes.key, 256, &aeskey);

	uint8_t iv[16], ecount[16];
	int num = 0;
	memset(iv, 0, 16);
	memset(ecount, 0, 16);
	
	AES_ctr128_encrypt(mem.ptr, out.ptr + o, mem.length, &aeskey, iv, ecount, &num);

	out.length = o + mem.length;
	

	write(1, envelope.ptr, envelope.length);
	write(1, out.ptr, out.length); }




void decrypt(int argc, char ** argv) {

	struct bignum_st * private = readkeyfile(argv[1]);

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);

	bytes stream = bytesalloc(1 << 15);
	int file = open(argv[0], O_RDONLY);	
	stream.length = read(file, stream.ptr, stream.length);

	uint64_t tag, length;
	int s = readheader(stream, &tag, &length);
	stream = bytescut(stream, s, 0);

	stream.raw[0] += 2;
	struct ec_point_st * secret = EC_POINT_new(curve);
	
	EC_POINT_oct2point(curve, secret, stream.ptr, 33, 0);
	EC_POINT_mul(curve, secret, 0, secret, private, 0);

	stream = bytescut(stream, 33, 0);

	struct { uint8_t y; uint8_t key[32]; } secretbytes;

	EC_POINT_point2oct(curve, secret, 2, (void *) &secretbytes, 33, 0);	


	bytes out = bytesalloc(1 << 15);

	s = readheader(stream, &tag, &length);
	stream = bytescut(stream, s, 0);
	

	struct aes_key_st aeskey;
	AES_set_encrypt_key(secretbytes.key, 256, &aeskey);

	uint8_t iv[16], ecount[16];
	int num = 0;
	memset(iv, 0, 16);
	memset(ecount, 0, 16);
	
	AES_ctr128_encrypt(stream.ptr, out.ptr, stream.length, &aeskey, iv, ecount, &num);
	

	write(1, out.ptr, stream.length); }







int main(int argc, char ** argv) {

	if(argc <= 1) {
		printf("Usage:\n"
			"echelon sign file [with] key\n"
			"echelon genkey\n"
			"echelon encrypt file [for] public-key\n"
			"echelon decrypt file [with] key\n"
			"echelon public-key key\n"
			"echelon verify file [and] signature [with] public-key\n");
		return 0; }

	argc -= 2;
	char * cmd = argv[1];
	argv += 2;

	if(strcmp(cmd, "sign") == 0 && argc == 2) {
		sign(argc, argv); }
	else if(strcmp(cmd, "verify") == 0 && argc == 3) {
		verify(argc, argv); }
	else if(strcmp(cmd, "genkey") == 0 && argc == 0) {
		genkey(argc, argv); }
	else if(strcmp(cmd, "public-key") == 0 && argc == 1) {
		publickey(argc, argv); }
	else if(strcmp(cmd, "encrypt") == 0 && argc == 2) {
		encrypt(argc, argv); }
	else if(strcmp(cmd, "decrypt") == 0 && argc == 2) {
		decrypt(argc, argv); }



	else printf("Unknown command!\n");

	return 0; }
















