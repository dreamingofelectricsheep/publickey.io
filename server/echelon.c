#include "bytes.c"
#include <fcntl.h>

#include "echelon-protocol.c"


void sign(int argc, char ** argv) {
	struct bignum_st * author;
	
	{
		uint8_t from[32];
		
		int keyfile = open(argv[1], O_RDONLY);
		read(keyfile, from, 32);
		close(keyfile);

		author = BN_bin2bn(from, 32, 0);
	}

	// Setup the signature.
	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct bignum_st * order = BN_new();
	EC_GROUP_get_order(curve, order, 0);

	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);
	EC_KEY_set_private_key(eckey, author);


	bytes mem = bytesalloc(10^16);
		
	{
		int file = open(argv[0], O_RDONLY);
		mem.length = read(file, mem.ptr, mem.length);
		close(file);
	}

	uint8_t hash[32];
	SHA256(mem.ptr, mem.length, hash);


	



	struct ECDSA_SIG_st * sig = ECDSA_do_sign(hash, 32, eckey);

	
	struct ep_signature packet = { ep_secp256k1_signature, 64 };
	
	BN_bn2bin(sig->r, packet.r);
	BN_bn2bin(sig->s, packet.s);






	write(1, &packet, sizeof(packet)); }


void verify(int argc, char ** argv) {
	
	bytes mem = bytesalloc(10^16);
		
	{
		int file = open(argv[0], O_RDONLY);
		mem.length = read(file, mem.ptr, mem.length);
		close(file);
	}

	uint8_t hash[32];
	SHA256(mem.ptr, mem.length, hash);
	bytesfree(mem);



	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);
	struct ec_point_st * author = EC_POINT_new(curve);

	{
		struct ep_public_key pub;

		int file = open(argv[2], O_RDONLY);
		read(file, &pub, sizeof(pub));
		close(file);

		struct bignum_st * x = BN_bin2bn(pub.x, 32, 0);

		EC_POINT_set_compressed_coordinates_GFp(curve, 
			author, x, pub.y, 0);

		// BN_free(x);
	}

	EC_KEY_set_public_key(eckey, author);



	struct ECDSA_SIG_st * sig = ECDSA_SIG_new();

	{
		struct ep_signature epsig;

		int file = open(argv[1], O_RDONLY);
		read(file, &epsig, sizeof(epsig));
		close(file);

		sig->r = BN_bin2bn(epsig.r, 32, 0);
		sig->s = BN_bin2bn(epsig.s, 32, 0);
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
	for(int i = 0; i < 32; i++) { bytes[i] = 0; }

	BN_bn2bin(key, bytes);

	write(1, bytes, 32); }

void publickey(int argc, char ** argv) {
	uint8_t bytes[32];

	{
		int keyfile = open(argv[0], O_RDONLY);
		read(keyfile, bytes, 32);
		close(keyfile);
	}

	struct bignum_st * key = BN_bin2bn(bytes, 32, 0);

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	

	struct ec_point_st * pub = EC_POINT_new(curve);
	EC_POINT_mul(curve, pub, 0, g, key, 0); 





	struct ep_public_key pubkey = { ep_secp256k1_public_key, 33 };

	EC_POINT_point2oct(curve, pub, 2, &pubkey.y, 33, 0);	

	pubkey.y -= 2;
	write(1, &pubkey, sizeof(pubkey)); }





int main(int argc, char ** argv) {

	if(argc <= 1) {
		printf("Usage:\n"
			"echelon sign file [with] key\n"
			"echelon genkey\n"
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



	else printf("Unknown command!\n");

	return 0; }
















