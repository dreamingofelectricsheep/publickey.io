#include <unistd.h>
#include <stdint.h>

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/rand.h>


#include "echelon-protocol.c"




int main(int argc, char ** argv) {

	if(argc != 2) {
		printf("Usage:\npublic-key key\n");
		return 0; }

	uint8_t bytes[32];

	{
		int keyfile = open(argv[1], O_RDONLY);
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
	write(1, &pubkey, sizeof(pubkey));


	return 0; }













