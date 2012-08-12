#include "bytes.c"

int ufloor(int a, int b) {
	return a / b; }

int uceil(int a, int b) {
	return (a + b - 1) / b; }

size_t toflex(bytes mem, uint64_t in) {
	uint64_t high = 1 << 7;

	int sig = 0;
	for(int i = 0; i < 64; i++) {
		if((in & (1 << (63 - i))) > 0) {
			sig = 64 - i;
			break; } }

	mem.length = uceil(sig, 7);
	if(mem.length == 0) mem.length = 1;

	for(int i = 0; i < mem.length; i++) {
		mem.raw[i] = high | (in >> 7*i); }

	mem.raw[mem.length - 1] &= (1 << 7) - 1;


	return mem.length; }




size_t fromflex(bytes in, uint64_t * out) {
	uint64_t v = 0;
	uint8_t high = 1 << 7;
	int i = 0;
	while((in.raw[i] & high) && (i < in.length)) {
		v |= ((high - 1) & in.raw[i]) << (7 * i);
		i++; }

	if(i == in.length) return i;

	v |= ((high - 1) & in.raw[i]) << (7 * i);
	*out = v;
	return i + 1; }


