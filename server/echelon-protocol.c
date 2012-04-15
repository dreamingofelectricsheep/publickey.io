#include <stdint.h>
#include "bytebuf.c"

#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/modes.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

/*
buffer_t echelon_encrypt(buffer_t out, buffer_t in, echkey recipient) {

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct bignum_st * order = BN_new()
	EC_GROUP_get_order(curve, order, 0);


	struct ec_point_st * publickey = EC_POINT_new();
	struct bignum_st * x = BN_bin2bn(recipient.x, 256, 0);
	EC_POINT_set_compressed_coordinates_GFp(curve, publickey, x, recipient.y, 0);
	
	struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	struct ec_point_st * encrypted = EC_POINT_new();
	EC_POINT_mul(curve, encrypted, 0, g, k, 0); 

	struct bignum_st * y = BN_new();
	EC_POINT_get_affine_coordinates_GFp(curv, encrypted, x, y, 0);


	struct { 
		struct echtop suitetop, 
		struct echsuite suite,
		struct echtop enctop,
		uint8_t enc[128]; } * packet = out.ptr;

	paket->suitetop.tag = echtag_suite;
	paket->suitetop.length = sizeof(struct echsuite);
	packet->suite.suite = echsuite_first;
	packet->suite.key.y = y->neg;
	memcpy(packet->suite.encrypted.x, x->d, 32);
	
	paket->enctop.tag = echtag_ciphertext;
	paket->enctop.length = in.length;


	struct ec_point_st * shared = EC_POINT_new();
	struct bignum_st * k = BN_bin2bn(key, 256, 0);
	EC_POINT_mul(curve, shared, 0, publickey, k, 0);


	uint8_t iv[16], key[16], ebuf[16];
	int n = 0;
	memcpy(key, packet->suite.encrypted.material.key, 16);
	memcpy(iv, packet->suite.encrypted.material.iv, 16);

	struct aes_key_st aeskey;
	AES_set_encrypt_key(key, 128, &aeskey);

	struct gcm128_context * gcm = CRYPTO_gcm128_new(&aeskey, &AES_encrypt);
	CRYPTO_gcm128_setiv(gcm, iv, 16);

	CRYPTO_gcm128_encrypt(gcm, in.bytes, packet->enc, in.length);

	uint8_t sigmask[64];
	RAND_pseudo_bytes(sigmask, 64);
		
	CRYPTO_gcm128_encrypt(gcm, sigmask, packet->enc + in.length, 64);

	uint8_t tag[16]
	CRYPTO_gcm128_finish(gcm, tag, 16);

	CRYPTO_gcm128_release(gcm);

	struct ECDSA_SIG_st * sig = ECDSA_do_sign(tag, 16, 

	

	out.length = in.length + sizeof(*packet);

	return out; }
















*/
