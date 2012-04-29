#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef bytebuf
#define bytebuf

typedef struct {
	union {
		void * ptr;
		uint8_t * bytes; };
	size_t length; } utf8_t, buffer_t, byetes;

size_t min(size_t f, size_t s) {
	return f < s ? f : s; }

buffer_t buffercpy(buffer_t dst, buffer_t src) {
	size_t s = min(src.length, dst.length);
	memcpy(dst.bytes, src.bytes, s);
	return (buffer_t) { dst.bytes, s }; }

buffer_t bufferalloc(size_t length) {
	return (buffer_t) { malloc(length), length }; }

void bufferfree(buffer_t buf) {
	free(buf.bytes); }

utf8_t utf8(char * c_str) {
	size_t null = 0;
	while(c_str[null] != 0) {
		null++; }

	return (utf8_t) { (uint8_t *) c_str, null }; }

utf8_t utf8fromint(int number, buffer_t buf) {
	if(number == 0) {
		buf.bytes[0] = '0';
		return (utf8_t) { buf.bytes, 1 }; }
	else {
		int i = 0;
		while(number > 0) {
			buf.bytes[i] = (number % 10) + (uint8_t) '0';
			number /= 10;
			i++; }
		for(int j = 0; j < i / 2; j++) {
			uint8_t t = buf.bytes[j];
			buf.bytes[j] = buf.bytes[i - j - 1]; 
			buf.bytes[i - j - 1] = t; }
		return (utf8_t) { buf.bytes, i}; } }

utf8_t utf8cut(utf8_t string, size_t start, size_t end) {
	if(end > 0) { return (utf8_t) { string.bytes + start, end - start }; }
	else return (utf8_t) { string.bytes + start, string.length + end - start, }; }

void utf8print(utf8_t u) {
	write(1, u.bytes, u.length); }

utf8_t utf8concat(buffer_t dst, size_t args, ...) {
	va_list ap;
	buffer_t r = dst;

	va_start(ap, args);
	for(int i = 0; i < args; i++) {
		utf8_t a = va_arg(ap, utf8_t);
		buffercpy(r, a);
		r = utf8cut(r, a.length, 0); }
	va_end(ap);

	return (utf8_t) { dst.bytes, dst.length - r.length }; }


typedef struct {
	utf8_t before, found, after; } utf8slice_result;


utf8slice_result utf8slice(utf8_t slicee, utf8_t slicer) {
	for(int i = 0, j = 0; i < slicee.length; i++) {
		if(slicee.bytes[i] == slicer.bytes[j]) {
			if(++j == slicer.length) {
				return (utf8slice_result) {
					(utf8_t) { slicee.bytes, i - slicer.length + 1 },
					(utf8_t) { slicee.bytes + i + 1 - slicer.length,
						slicer.length },
					(utf8_t) { slicee.bytes + i + 1, 
						slicee.length - i - 1} }; } }
		else { j = 0; } }

	return (utf8slice_result) { slicee, (utf8_t) { 0, 0 }, (utf8_t) { 0, 0 } }; }

int utf8cmp(utf8_t first, utf8_t second) {
	size_t length = min(first.length, second.length);

	for(int i = 0; i < length; i++) {
		if(first.bytes[i] < second.bytes[i]) return -1;
		if(first.bytes[i] > second.bytes[i]) return 1; }

	if(first.length != second.length) return length;
	return 0; }


uint64_t utf8hash(utf8_t s) {
	uint64_t u = 0;
	for(int i = 0; i < s.length; i++) {
		u ^= ((uint64_t) s.bytes[i]) << (i % 8); }

	return u; } 

#endif
