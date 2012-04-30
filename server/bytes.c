#ifndef bytebuf
#define bytebuf

#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>


typedef struct {
	union {
		void * ptr;
		uint8_t * raw; };
	size_t length; } bytes, bytes, bytes;

size_t min(size_t f, size_t s) {
	return f < s ? f : s; }

bytes bytescpy(bytes dst, bytes src) {
	size_t s = min(src.length, dst.length);
	memcpy(dst.raw, src.raw, s);
	return (bytes) { dst.raw, s }; }

bytes bytesalloc(size_t length) {
	return (bytes) { malloc(length), length }; }

void bytesfree(bytes buf) {
	free(buf.raw); }

bytes bytesfrom(char * c_str) {
	size_t null = 0;
	while(c_str[null] != 0) {
		null++; }

	return (bytes) { (uint8_t *) c_str, null }; }

bytes bytesfromint(int number, bytes buf) {
	if(number == 0) {
		buf.raw[0] = '0';
		return (bytes) { buf.raw, 1 }; }
	else {
		int i = 0;
		while(number > 0) {
			buf.raw[i] = (number % 10) + (uint8_t) '0';
			number /= 10;
			i++; }
		for(int j = 0; j < i / 2; j++) {
			uint8_t t = buf.raw[j];
			buf.raw[j] = buf.raw[i - j - 1]; 
			buf.raw[i - j - 1] = t; }
		return (bytes) { buf.raw, i}; } }

bytes bytescut(bytes string, size_t start, size_t end) {
	if(end > 0) { return (bytes) { string.raw + start, end - start }; }
	else return (bytes) { string.raw + start, string.length + end - start, }; }

void bytesprint(bytes u) {
	write(1, u.raw, u.length); }

bytes bytesread(bytes mem) {
	ssize_t r = read(0, mem.raw, mem.length);
	return (bytes) { mem.raw, r }; }

bytes bytesconcat(bytes dst, size_t args, ...) {
	va_list ap;
	bytes r = dst;

	va_start(ap, args);
	for(int i = 0; i < args; i++) {
		bytes a = va_arg(ap, bytes);
		bytescpy(r, a);
		r = bytescut(r, a.length, 0); }
	va_end(ap);

	return (bytes) { dst.raw, dst.length - r.length }; }


typedef struct {
	bytes before, found, after; } bytesslice_result;


bytesslice_result bytesslice(bytes slicee, bytes slicer) {
	for(int i = 0, j = 0; i < slicee.length; i++) {
		if(slicee.raw[i] == slicer.raw[j]) {
			if(++j == slicer.length) {
				return (bytesslice_result) {
					(bytes) { slicee.raw, i - slicer.length + 1 },
					(bytes) { slicee.raw + i + 1 - slicer.length,
						slicer.length },
					(bytes) { slicee.raw + i + 1, 
						slicee.length - i - 1} }; } }
		else { j = 0; } }

	return (bytesslice_result) { slicee, (bytes) { 0, 0 }, (bytes) { 0, 0 } }; }

int bytescmp(bytes first, bytes second) {
	size_t length = min(first.length, second.length);

	for(int i = 0; i < length; i++) {
		if(first.raw[i] < second.raw[i]) return -1;
		if(first.raw[i] > second.raw[i]) return 1; }

	if(first.length != second.length) return length;
	return 0; }


uint64_t byteshash(bytes s) {
	uint64_t u = 0;
	for(int i = 0; i < s.length; i++) {
		u ^= ((uint64_t) s.raw[i]) << (i % 8); }

	return u; } 

#endif
