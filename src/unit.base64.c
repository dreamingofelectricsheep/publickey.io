#include "base64.c"



int main(int argc, char ** argv) {
	uint8_t out[1024];
	out[1023] = 0;
	buffer_t o = {1024, out};
	uint8_t * str = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
	base64_encode((buffer_t) { 271, str }, o);

	uint8_t out2[1024];
	buffer_t bb = {1024, out2 };
	base64_decode(o, bb);
	printf("%s\n", out2);

	return 0; }



