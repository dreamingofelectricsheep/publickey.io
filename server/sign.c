#include "bytes.c"
#include <fcntl.h>

#include "echelon-protocol.c"



int main(int argc, char ** argv) {

	if(argc != 4) {
		printf("Usage:\nsign file with key\n");
		return 0; }

	struct bignum_st * author;
	
	{
		uint8_t from[32];
		
		int keyfile = open(argv[3], O_RDONLY);
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
		int file = open(argv[1], O_RDONLY);
		mem.length = read(file, mem.ptr, mem.length);
		close(file);
	}

	uint8_t hash[32];
	SHA256(mem.ptr, mem.length, hash);


	



	struct ECDSA_SIG_st * sig = ECDSA_do_sign(hash, 32, eckey);

	
	struct ep_signature packet = { ep_secp256k1_signature, 64 };
	
	BN_bn2bin(sig->r, packet.r);
	BN_bn2bin(sig->s, packet.s);






	write(1, &packet, sizeof(packet));


	return 0; }
















