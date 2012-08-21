#ifndef bytebuf
#define bytebuf

#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define B(...) (bytes) { (char[]) { __VA_ARGS__ }, PP_NARG(__VA_ARGS__) }

typedef struct {
	union {
		char * as_char;
		void * as_void;
		uint8_t * as_uint8; };
	size_t length; } bytes;

/* All functions here take into account the specified length of their arguments.
No memory allocation is done! */

size_t min(size_t f, size_t s) {
	return f < s ? f : s; }

bytes bcpy(bytes dst, bytes src) {
	size_t s = min(src.length, dst.length);
	memcpy(dst.as_uint8, src.as_uint8, s);
	return (bytes) { dst.as_uint8, s }; }

bytes balloc(size_t length) {
	return (bytes) { malloc(length), length }; }

void bfree(bytes buf) {
	free(buf.as_uint8); }

bytes bfromstr(char * c_str) {
	size_t null = 0;
	while(c_str[null] != 0) {
		null++; }

	return (bytes) { (uint8_t *) c_str, null }; }

bytes bfromint(int number, bytes buf) {
	if(number == 0) {
		buf.as_uint8[0] = '0';
		return (bytes) { buf.as_uint8, 1 }; }
	else {
		int i = 0;
		while(number > 0) {
			buf.as_uint8[i] = (number % 10) + (uint8_t) '0';
			number /= 10;
			i++; }
		for(int j = 0; j < i / 2; j++) {
			uint8_t t = buf.as_uint8[j];
			buf.as_uint8[j] = buf.as_uint8[i - j - 1]; 
			buf.as_uint8[i - j - 1] = t; }
		return (bytes) { buf.as_uint8, i}; } }

bytes bcut(bytes string, size_t start, size_t end) {
	if(end > 0) { return (bytes) { string.as_uint8 + start, end - start }; }
	else return (bytes) { string.as_uint8 + start, string.length + end - start, }; }

void bprint(bytes u) {
	write(1, u.as_uint8, u.length); }

void berror(bytes u) {
	write(2, u.as_uint8, u.length); }

bytes bread(bytes mem) {
	ssize_t r = read(0, mem.as_uint8, mem.length);
	return (bytes) { mem.as_uint8, r }; }

bytes bcat(bytes dst, bytes arg1, bytes arg2) {
	bytes r = dst;
	bcpy(r, arg1); r = bcut(r, arg1.length, 0); }
	bcpy(r, arg2); r = bcut(r, arg2.length, 0); }

	return (bytes) { dst.as_uint8, dst.length - r.length }; }


typedef struct {
	bytes before, found, after; } bfound;

bfound bfind(bytes in, bytes what) {
	for(int i = 0, j = 0; i < in.length; i++) {
		if(in.as_uint8[i] == what.as_uint8[j]) {
			if(++j == what.length) {
				return (bfound) {
					(bytes) { in.as_uint8, i - what.length + 1 },
					(bytes) { in.as_uint8 + i + 1 - what.length,
						what.length },
					(bytes) { in.as_uint8 + i + 1, 
						in.length - i - 1} }; } }
		else { j = 0; } }

	return (bfound) { in, (bytes) { 0, 0 }, (bytes) { 0, 0 } }; }

int bcmp(bytes first, bytes second) {
	size_t length = min(first.length, second.length);

	for(int i = 0; i < length; i++) {
		if(first.as_uint8[i] < second.as_uint8[i]) return -1;
		if(first.as_uint8[i] > second.as_uint8[i]) return 1; }

	if(first.length != second.length) return length;
	return 0; }


uint64_t bhash(bytes s) {
	uint64_t u = 0;
	for(int i = 0; i < s.length; i++) {
		u ^= ((uint64_t) s.as_uint8[i]) << (i % 8); }

	return u; } 

#endif
