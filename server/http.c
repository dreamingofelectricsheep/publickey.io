#include "bytebuf.c"


#define bintree(x) http_bintree_ ## x

typedef uint64_t bintree(key_t);
typedef buffer_t bintree(payload_t);

#include "bintree_cmp.c"

bintree(cmp_t) bintree(cmp_fun)(bintree(key_t) a, bintree(key_t) b) {
	if(a == b) return bintree(equal);
	if(a < b) return bintree(less);
	return bintree(greater); }

#include "bintree.c"

#undef bintree

typedef enum {
	http_get,
	http_post,
	http_head } http_request_type;

typedef struct {
	http_request_type type;
	utf8_t url;
	struct http_bintree_node * headers;
	} http_request;

int http_request_parse(buffer_t buffer, http_request * request) {
	utf8slice_result r = utf8slice(buffer, utf8(" "));

	request->headers = 0;

	switch(r.before.bytes[0]) {
		case 'G': request->type = http_get; break;
		case 'H': request->type = http_head; break; };

	r = utf8slice(r.after, utf8(" "));
	request->url = r.before;
	r = utf8slice(r.after, utf8("\r\n"));
	
	while(r.after.length) {
		r = utf8slice(r.after, utf8(": "));
		
		if(r.before.length == 0) break;
		
		uint64_t key = utf8hash(r.before);
		
		r = utf8slice(r.after, utf8("\r\n"));
		
		http_bintree_push(&request->headers, key, r.before); }



	return 0; }


