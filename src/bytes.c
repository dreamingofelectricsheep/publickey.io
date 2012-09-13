#ifndef bytebuf
#define bytebuf

#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

int logging = 1;
#define debug(d...) { if(logging) { printf("[f: %s l: %d] ", __FILE__, __LINE__); \
	printf(d); printf("\n"); fsync(1); } } 

#define B(...) (bytes) { (char[]) \
	{ __VA_ARGS__ }, sizeof((char[]) { __VA_ARGS__ }) }
#define Bs(s) (bytes) { (void*)s, sizeof(s) - 1 }

typedef struct {
	union {
		char * as_char;
		void * as_void; };
	size_t length; } bytes;
 
/* All functions here take into account the specified length of their arguments.
No memory allocation is done! */

size_t min(size_t f, size_t s) {
	return f < s ? f : s; }

bytes bcpy(bytes dst, bytes src) {
	size_t s = min(src.length, dst.length);
	memcpy(dst.as_char, src.as_char, s);
	return (bytes) { dst.as_char, s }; }

bytes balloc(size_t length) {
	return (bytes) { malloc(length), length }; }

void bfree(bytes buf) {
	free(buf.as_char); }

bytes bfromstr(char * c_str) {
	size_t null = 0;
	while(c_str[null] != 0) {
		null++; }

	return (bytes) {  c_str, null }; }

bytes bfromint(bytes buf, int number) {
	if(number == 0) {
		buf.as_char[0] = '0';
		return (bytes) { buf.as_char, 1 }; }
	else {
		int i = 0;
		while(number > 0) {
			buf.as_char[i] = (number % 10) + (uint8_t) '0';
			number /= 10;
			i++; }
		for(int j = 0; j < i / 2; j++) {
			uint8_t t = buf.as_char[j];
			buf.as_char[j] = buf.as_char[i - j - 1]; 
			buf.as_char[i - j - 1] = t; }
		return (bytes) { buf.as_char, i}; } }

int btoint(bytes b) {
	int acc = 0;
	for(int i = 0; i < b.length; i++) {
		acc *= 10;
		acc += b.as_char[i]; }
	return acc; }

bytes bslice(bytes string, size_t start, size_t end) {
	if(end > 0) { return (bytes) { string.as_char + start, end - start }; }
	else return (bytes) { string.as_char + start, string.length + end - start, }; }

void bprint(bytes u) {
	write(1, u.as_char, u.length); }

void berror(bytes u) {
	write(2, u.as_char, u.length); }

bytes bread(bytes mem) {
	ssize_t r = read(0, mem.as_char, mem.length);
	return (bytes) { mem.as_char, r }; }

bytes bcat(bytes dst, bytes arg1, bytes arg2) {
	bytes r = dst;
	bcpy(r, arg1); r = bslice(r, arg1.length, 0); 
	bcpy(r, arg2); r = bslice(r, arg2.length, 0); 

	return (bytes) { dst.as_char, dst.length - r.length }; }


typedef struct {
	bytes before, found, after; } bfound;

bfound bfind(bytes in, bytes what) {
	for(int i = 0, j = 0; i < in.length; i++) {
		if(in.as_char[i] == what.as_char[j]) {
			if(++j == what.length) {
				return (bfound) {
					(bytes) { in.as_char, i - what.length + 1 },
					(bytes) { in.as_char + i + 1 - what.length,
						what.length },
					(bytes) { in.as_char + i + 1, 
						in.length - i - 1} }; } }
		else { j = 0; } }

	return (bfound) { in, (bytes) { 0, 0 }, (bytes) { 0, 0 } }; }

int bcmp(bytes first, bytes second) {
	size_t length = min(first.length, second.length);
	return memcmp(first.as_char, second.as_char, length); }

#endif
