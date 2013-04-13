

struct http_request 
{
	bytes header;
	bytes payload;
	bytes addr;
};

enum http_result 
{
	http_ok = 0,
	http_bad_request,
	http_not_found,
	http_forbidden,
	http_switching_protocols,
	http_internal_server_error,
	http_last
};

bytes http_result_text[] = 
{
	Bs("200 Ok"),
	Bs("400 Bad Request"),
	Bs("404 Not Found"),
	Bs("403 Forbidden"),
	Bs("101 Switching Protocols"),
	Bs("500 Internal Server Error")
};


const size_t http_max_headers = 8;


struct http_raw_response 
{
	enum http_result status;
	bytes payload;
	size_t headers;
	bytes header_val[http_max_headers];
	bytes header_name[http_max_headers];
} 
http_ondata_callback(struct http_request *request);

struct http_response 
{
	bytes header, 
		payload;
};

struct http_raw_response http_result(enum http_result status)
{
	return (struct http_raw_response) 
	{
		.headers = 0,
		.status = status,
		.payload = bvoid 
	};
}

bytes http_extract_param(bytes header, bytes field)
{
	bfound f = bfind(header, field);
	f = bfind(bslice(f.after, 2, 0), Bs("\r\n"));
	return f.before;
}

struct http_request http_parse_request(bytes buffer)
{
	write(0, buffer.as_void, buffer.len);

	bfound f = bfind(buffer, Bs("\r\n\r\n"));

	if (f.found.len == 0) 
	{
		debug("Partial http header.");

		return (struct http_request) {
			.addr = (bytes) bvoid
		};
	}

	struct http_request request = {
		.header = f.before,
		.payload = f.after,
	};

	f = bfind(request.header, Bs("Content-Length: "));

	if (f.found.len > 0) {
		f = bfind(f.after, Bs("\r\n"));

		int contentlen = btoi(f.before);

		if (contentlen != request.payload.len) {
			debug
			    ("Partial body received. %lu out of %d bytes available.",
			     request.payload.len, contentlen);

			return (struct http_request) {
				.addr = bvoid
			};
		}
	}

	f = bfind(request.header, Bs(" "));
	f = bfind(f.after, Bs(" "));

	request.addr = f.before;

	return request;
}


// The result needs freeing
struct http_response http_assemble_response(struct http_raw_response raw)
{
	struct http_response result = { bvoid, bvoid };

	if(raw.status >= http_last)
		raw = http_result(http_internal_server_error);
	


	bytes response_header_mem = balloc(4096);


	bytes status = http_result_text[raw.status];
	bpair p = bprintf(response_header_mem, "HTTP/1.1 %*s\r\n", status.len,
		status.as_void);

	
	if(raw.status == http_switching_protocols)
	{
		p = bprintf(p.second, "Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n");
	}
	else
	{
		p = bprintf(p.second, "Content-Length: %zd\r\n", raw.payload.len);
	}

	
	for(int i = 0; i < raw.headers; i++)
	{
		p = bprintf(p.second, "%*s: %*s\r\n", 
			raw.header_name[i].len, raw.header_name[i].as_void,
			raw.header_val[i].len, raw.header_val[i].as_void);

		bfree(&raw.header_val[i]);
	}

	p = bprintf(p.second, "\r\n");

	result.header.as_void = response_header_mem.as_void;
	result.header.len = p.second.as_void - response_header_mem.as_void;
	
	result.payload = raw.payload;
	
	return result;
}



