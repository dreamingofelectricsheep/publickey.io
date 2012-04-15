#include <stdint.h>
#include "bytebuf.c"
#include <fcntl.h>

#include "echelon-protocol.c"



int main(int argc, char ** argv) {

	struct bignum_st * private;
	
	{
		uint8_t bytes[32];
		
		int keyfile = open(argv[1], O_RDONLY);
		read(keyfile, bytes, 32);
		close(keyfile);

		private = BN_bin2bn(bytes, 32, 0);
	}

	struct ec_group_st * curve = EC_GROUP_new_by_curve_name(NID_secp256k1);

	struct {
		uint16_t length;
		uint8_t y;
		uint8_t x[32]; } envelope;

	read(0, &envelope, sizeof(envelope));

	struct bignum_st * x = BN_bin2bn(envelope.x, 32, 0);
	
	struct ec_point_st * secret = EC_POINT_new(curve);
	EC_POINT_set_compressed_coordinates_GFp(curve, secret, x, envelope.y, 0);
	EC_POINT_mul(curve, secret, 0, secret, private, 0);


	uint8_t material[32];
	memset(material, 0, 32);
	EC_POINT_get_affine_coordinates_GFp(curve, secret, x, 0, 0);
	BN_bn2bin(x, material);


	uint8_t * iv = material, * key = material + 16;
	int n = 0;


	uint8_t data[4096];
	size_t data_length = read(0, data, 4096);
	uint8_t out[4096];

	struct aes_key_st aeskey;
	AES_set_encrypt_key(key, 128, &aeskey);

	struct gcm128_context * gcm = CRYPTO_gcm128_new(&aeskey, 
		(block128_f) &AES_encrypt);
	CRYPTO_gcm128_setiv(gcm, iv, 16);

	CRYPTO_gcm128_aad(gcm, (char *) &envelope, sizeof(envelope));
	CRYPTO_gcm128_decrypt(gcm, data, out, envelope.length);

	uint8_t sigmask[64];
	CRYPTO_gcm128_decrypt(gcm, data + envelope.length, sigmask, 64);

	uint8_t tag[16];
	CRYPTO_gcm128_finish(gcm, tag, 16);

	CRYPTO_gcm128_release(gcm);

	struct ec_key_st * eckey = EC_KEY_new();
	EC_KEY_set_group(eckey, curve);
	EC_KEY_set_private_key(eckey, private);

	const struct ec_point_st * g = EC_GROUP_get0_generator(curve);
	struct ec_point_st * pub = EC_POINT_new(curve);
	EC_POINT_mul(curve, pub, 0, g, private, 0);
	EC_KEY_set_public_key(eckey, pub);


	for(int i = 0; i < 6; i++) {
		(data + envelope.length + 64)[i] ^= sigmask[i]; }

	struct ECDSA_SIG_st * sig = ECDSA_SIG_new();
	sig->r = BN_bin2bn(data + envelope.length + 64, 32, 0);
	sig->s = BN_bin2bn(data + envelope.length + 64 + 32, 32, 0);

	int verification = ECDSA_do_verify(tag, 16, sig, eckey);
	

	write(1, out, envelope.length);
	printf("\nVerification: %s\n", verification == 1 ? "yes" : "no");

	return 0; }
















