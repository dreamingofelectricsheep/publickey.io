#include <unistd.h>
#include <stdint.h>

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/rand.h>










int main(int argc, char ** argv) {

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct bignum_st * order = BN_new();
	struct bignum_st * key = BN_new();

	EC_GROUP_get_order(curve, order, 0);
	BN_rand_range(key, order);

	uint8_t bytes[32];
	for(int i = 0; i < 32; i++) { bytes[i] = 0; }

	BN_bn2bin(key, bytes);

	write(1, bytes, 32);

	return 0; }













