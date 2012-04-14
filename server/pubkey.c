#include <unistd.h>
#include <stdint.h>

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/rand.h>


#include "echelon-protocol.c"








int main(int argc, char ** argv) {

	uint8_t bytes[32];
	read(0, bytes, 32);

	struct bignum_st * key = BN_bin2bn(bytes, 32, 0);

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	

	struct ec_point_st * pub = EC_POINT_new(curve);
	EC_POINT_mul(curve, pub, 0, g, key, 0); 


	struct bignum_st * x = BN_new();
	struct bignum_st * y = BN_new();


	EC_POINT_get_affine_coordinates_GFp(curve, pub, x, y, 0);

	struct {
		struct echtop top;
		uint8_t y;
		uint8_t x[32]; } packet = 
	{ 
		{ echtag_publickey, 33 },
		y->neg };

	for(int i = 0; i < 32; i++) { packet.x[i] = 0; }
	
	BN_bn2bin(x, packet.x);

	write(1, &packet, sizeof(packet));

	return 0; }













