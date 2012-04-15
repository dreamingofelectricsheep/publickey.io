#include <stdint.h>
#include "bytebuf.c"
#include <fcntl.h>

#include "echelon-protocol.c"



int main(int argc, char ** argv) {

	struct bignum_st * author;
	
	{
		uint8_t from[32];
		
		int keyfile = open(argv[1], O_RDONLY);
		read(keyfile, from, 32);
		close(keyfile);

		author = BN_bin2bn(from, 32, 0);
	}

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	struct ec_point_st * recipient;

	{
		struct {
			uint8_t y;
			uint8_t x[32]; } pub;

		int file = open(argv[2], O_RDONLY);
		read(file, &pub, sizeof(pub));
		close(file);

		struct bignum_st * x = BN_bin2bn(pub.x, 32, 0);

		recipient = EC_POINT_new(curve);
		EC_POINT_set_compressed_coordinates_GFp(curve, 
			recipient, x, pub.y, 0);

		// BN_free(x);
	}


	uint8_t in[4096];
	size_t in_length = read(0, in, 4096);

	struct bignum_st * order = BN_new();
	EC_GROUP_get_order(curve, order, 0);

	struct bignum_st * k = BN_new();
	BN_pseudo_rand_range(k, order);
	
	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	struct ec_point_st * encrypted = EC_POINT_new(curve);
	EC_POINT_mul(curve, encrypted, 0, g, k, 0); 

	struct bignum_st * x = BN_new();
	struct bignum_st * y = BN_new();
	EC_POINT_get_affine_coordinates_GFp(curve, encrypted, x, y, 0);


	struct {
		uint16_t length;
		uint8_t y;
		uint8_t x[32]; } envelope;

	envelope.length = in_length;
	envelope.y = y->neg;

	memset(envelope.x, 0, 32);
	BN_bn2bin(x, envelope.x);



	struct ec_point_st * secret = EC_POINT_new(curve);
	EC_POINT_mul(curve, secret, 0, recipient, k, 0);


	uint8_t material[32];
	memset(material, 0, 32);	
	EC_POINT_get_affine_coordinates_GFp(curve, secret, x, y, 0);
	BN_bn2bin(x, material);


	uint8_t * iv = material, * key = material + 16;
	int n = 0;

	uint8_t out[4096];

	struct aes_key_st aeskey;
	AES_set_encrypt_key(key, 128, &aeskey);

	struct gcm128_context * gcm = CRYPTO_gcm128_new(&aeskey, 
		(block128_f) &AES_encrypt);
	CRYPTO_gcm128_setiv(gcm, iv, 16);

	CRYPTO_gcm128_aad(gcm, (char *) &envelope, sizeof(envelope));
	CRYPTO_gcm128_encrypt(gcm, in, out, in_length);

	uint8_t sigmask[64];
	RAND_pseudo_bytes(sigmask, 64);
	CRYPTO_gcm128_encrypt(gcm, sigmask, out + in_length, 64);

	uint8_t tag[16];
	CRYPTO_gcm128_finish(gcm, tag, 16);

	CRYPTO_gcm128_release(gcm);

	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);
	EC_KEY_set_private_key(eckey, author);
	struct ECDSA_SIG_st * sig = ECDSA_do_sign(tag, 16, eckey);
	
	BN_bn2bin(sig->r, out + in_length + 64);
	BN_bn2bin(sig->s, out + in_length + 64 + 32);

	for(int i = 0; i < 6; i++) {
		(out + in_length + 64)[i] ^= sigmask[i]; }

	

	write(1, &envelope, sizeof(envelope));
	write(1, out, in_length + 128);

	return 0; }
















