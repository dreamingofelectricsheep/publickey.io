#include "bytes.c"

typedef enum {
	http_unknown,
	http_get,
	http_post,
	http_head } http_request_type;

typedef struct {
	http_request_type type;
	bytes location;
	} http_request;



int http_request_parse(bytes raw, http_request * request) {
	bytes crlf = B('\r', '\n');
	bytes space = B(' ');
	bfound f = bfind(raw, crlf);

	if(f.before.length < 1) return -1;

	switch(f.before.as_char[0]) {
		case 'G': request->type = http_get; break;
		case 'H': request->type = http_head; break; };
	
	//raw = f.after;
	
	f = bfind(f.before, space);
	f = bfind(f.after, space);
	request->location = f.before;


	return 0; }


