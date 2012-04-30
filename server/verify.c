#include "bytes.c"
#include <fcntl.h>

#include "echelon-protocol.c"



int main(int argc, char ** argv) {

	if(argc != 6) {
		printf("Usage:\nverify file and signature with public-key\n");
		return 0; }

	
	bytes mem = bytesalloc(10^16);
		
	{
		int file = open(argv[1], O_RDONLY);
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

		int file = open(argv[5], O_RDONLY);
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

		int file = open(argv[3], O_RDONLY);
		read(file, &epsig, sizeof(epsig));
		close(file);

		sig->r = BN_bin2bn(epsig.r, 32, 0);
		sig->s = BN_bin2bn(epsig.s, 32, 0);
	}


	int verification = ECDSA_do_verify(hash, 32, sig, eckey);
	

	printf("%d\n", verification);
	if(verification == 1) { printf("Signature matches the public key.\n"); }
	else printf("Invalid signature.\n");


	return 0; }
















