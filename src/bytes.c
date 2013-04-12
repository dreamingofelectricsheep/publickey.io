#ifndef bytebuf
#define bytebuf

#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

int logging = 1;
#define debug(d...) { if(logging) { printf("[f: %s l: %d] ", __FILE__, __LINE__); \
	printf(d); printf("\n"); fsync(1); } }

#define B(...) (bytes) { (char[]) \
	{ __VA_ARGS__ }, sizeof((char[]) { __VA_ARGS__ }) }
#define Bs(s) (bytes) { (void*)s, sizeof(s) - 1 }

typedef struct {
	union {
		char *as_char;
		void *as_void;
		const void *as_cvoid;
		const char *as_cchar;
	};
	union {
		size_t length;
		size_t len;
	};
} bytes;

bytes bvoid = { 0, 0 };

/* All functions here take into account the specified length of their arguments.
No memory allocation is done! */

size_t min(size_t f, size_t s)
{
	return f < s ? f : s;
}

bytes bcpy(bytes dst, bytes src)
{
	size_t s = min(src.length, dst.length);
	memcpy(dst.as_char, src.as_char, s);

	bytes r = { dst.as_char, s };
	return r;
}

bytes balloc(size_t length)
{
	bytes r = { malloc(length), length };
	return r;
}

typedef struct 
{
	bytes first, second;
} bpair;

bpair bprintf(bytes b, const char *fmt, ...)
{
	va_list ptr;
	va_start(ptr, fmt);
	bytes result = b;
	result.len = vsnprintf(b.as_void, b.len, fmt, ptr);
	va_end(ptr);
	return (bpair) {
		result, 
		(bytes) {
			.as_void = result.as_void + result.len,
			.len = b.len - result.len
		}
	};
}
	
int bscanf(bytes b, const char *fmt, ...)
{
	va_list ptr;
	va_start(ptr, fmt);
	int r;
	FILE * f = fmemopen(b.as_void, b.len, "r");
	r = vfscanf(f, fmt, ptr);
	va_end(ptr);
	return r;
}

void bfree(bytes * buf)
{
	free(buf->as_char);
	buf->as_void = 0;
	buf->len = 0;
}

bytes bfromstr(const char *c_str)
{
	size_t null = 0;
	while (c_str[null] != 0) {
		null++;
	}

	bytes r = { (void *)c_str, null };
	return r;
}

bytes itob(bytes buf, int number)
{
	if (number == 0) {
		buf.as_char[0] = '0';

		bytes r = { buf.as_char, 1 };
		return r;

	} else {
		int i = 0;

		while (number > 0) {
			buf.as_char[i] = (number % 10) + (uint8_t) '0';
			number /= 10;
			i++;
		}

		for (int j = 0; j < i / 2; j++) {
			uint8_t t = buf.as_char[j];
			buf.as_char[j] = buf.as_char[i - j - 1];
			buf.as_char[i - j - 1] = t;
		}

		bytes r = { buf.as_char, i };
		return r;
	}
}

int64_t btoi64(bytes b)
{
	int64_t acc = 0;
	bool neg = false;	
	int i = 0;

	if(b.len > 0 && b.as_char[0] == '-') {
		neg = true;
		i++;
	}

	for (; i < b.len; i++) {
		acc *= 10;
		int digit = b.as_char[i] - '0';
		if(digit > 9 || digit < 0)
			return 0;
		acc += digit;
	}

	return neg == true ? -acc : acc;
}

int btoi(bytes b) { return btoi64(b); }

bytes bslice(bytes string, size_t start, size_t end)
{
	if (start >= string.len) {
		bytes r = { string.as_char, 0 };
		return r;
	}

	if (end > 0) {
		bytes r = { string.as_char + start, end - start };
		return r;
	} else {
		bytes r =
		    { string.as_char + start, string.length + end - start, };
		return r;
	}
}

void bprint(bytes u)
{
	write(1, u.as_char, u.length);
}

void berror(bytes u)
{
	write(2, u.as_char, u.length);
}

bytes bread(bytes mem)
{
	ssize_t r = read(0, mem.as_char, mem.length);

	bytes b = { mem.as_char, r };
	return b;
}

bytes bcat(bytes dst, bytes arg1, bytes arg2)
{
	bytes r = dst;
	bcpy(r, arg1);
	r = bslice(r, arg1.length, 0);
	bcpy(r, arg2);
	r = bslice(r, arg2.length, 0);

	bytes b = { dst.as_char, dst.length - r.length };
	return b;
}

typedef struct {
	bytes before, found, after;
} bfound;

bfound bfind(bytes in, bytes what)
{
	for (int i = 0, j = 0; i < in.length; i++) {
		if (in.as_char[i] == what.as_char[j]) {
			if (++j == what.length) {

				bfound f = { {in.as_char, i - what.length + 1}
				, {in.as_char + i + 1 - what.length, what.length}
				, {in.as_char + i + 1, in.length - i - 1}
				};

				return f;

			}
		} else {
			j = 0;
		}
	}

	bfound f = { in, {0, 0}
	, {0, 0}
	};
	return f;
}

int bcmp2(bytes first, bytes second)
{
	size_t length = min(first.length, second.length);
	return memcmp(first.as_char, second.as_char, length);
}

int bsame(bytes first, bytes second)
{
	if (first.length != second.length)
		return -1;
	return memcmp(first.as_char, second.as_char, first.length);
}

#endif
