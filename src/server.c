#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#define _GNU_SOURCE
#include <dirent.h>

#include "sqlite3.h"

#include "http_server.c"

sqlite3 *db;
sqlite3_stmt *selectpassword;
sqlite3_stmt *selectsession;
sqlite3_stmt *updatepassword;
sqlite3_stmt *updatelogin;
sqlite3_stmt *updatelastseen;
sqlite3_stmt *insertuser;
sqlite3_stmt *selectuserid;

sqlite3_stmt *insertbranch;
sqlite3_stmt *selectbranches;

int lastblob = 0;

struct insert
{
	uint32_t pos;
	uint32_t lastlen;
	uint32_t newlen;
};

#define http_callback_fun(id) struct http_raw_response\
	http_callback_ ## id(struct http_request * request)



int getuserid(bytes header)
{

	bytes cookie = http_extract_param(header, Bs("Cookie"));
	bfound f = bfind(cookie, Bs("session="));
	f = bfind(f.after, Bs(";"));
	int64_t session = btoi64(f.before);

	debug("User session: %ld", session);
	sqlite3_reset(selectuserid);
	sqlite3_bind_int64(selectuserid, 1, session);


	if(sqlite3_step(selectuserid) != SQLITE_ROW)
		return 0;

	return sqlite3_column_int(selectuserid, 0);
}

	
http_callback_fun(root)
{
	int f = open("html/main.html", 0);
	bytes page = balloc(1 << 20);
	page.length = read(f, page.as_void, page.length);

	struct http_raw_response result = http_result(http_ok);
	result.payload = page;


	return result;
}

http_callback_fun(branch)
{
	int blobid = btoi(bslice(request->addr, Bs("/api/0/branch/").len, 0));

	bytes path = balloc(256);
	path = bprintf(path, "branches/%d", blobid).first;

	int f = open(path.as_char, O_RDONLY);
	bytes page = balloc(1 << 20);
	page.length = read(f, page.as_void, page.length);

	struct http_raw_response result = http_result(http_ok);
	result.payload = page;

	return result;
}

http_callback_fun(login)
{
	bfound f = bfind(request->payload, Bs("\n"));

	bytes user = f.before;
	bytes password = f.after;

	sqlite3_reset(selectpassword);
	sqlite3_bind_text(selectpassword, 1, user.as_void, user.len, 0);

	if (sqlite3_step(selectpassword) != SQLITE_ROW) {
		return http_result(http_bad_request);
	}

	bytes dbpassword = {
		.len = sqlite3_column_bytes(selectpassword, 0),
		.as_cvoid = sqlite3_column_blob(selectpassword, 0)
	};

	uint64_t session = sqlite3_column_int64(selectpassword, 1);

	if (bsame(password, dbpassword) == 0) {
		sqlite3_reset(updatelastseen);
		sqlite3_bind_int64(updatelastseen, 1, session);
		sqlite3_step(updatelastseen);

		bytes mem = balloc(128);

		struct http_raw_response result = http_result(http_ok);
		result.headers = 1;
		result.header_name[0] = Bs("Set-Cookie");
		result.header_val[0] = bprintf(mem, 
			"session=%ld; path=/; secure\r\n\r\n", session).first;

		debug("Handling login");
		return result;
	} else {
		return http_result(http_bad_request);
	}
}

http_callback_fun(branches)
{
	int64_t user = getuserid(request->header);

	if(user == 0) return http_result(http_bad_request);

	sqlite3_reset(selectbranches);
	sqlite3_bind_int64(selectbranches, 1, user);

	bytes mem = balloc(4096);
	bytes resp = mem;

	while(sqlite3_step(selectbranches) == SQLITE_ROW)
	{
		int64_t id = sqlite3_column_int64(selectbranches, 0);
		int64_t parent = sqlite3_column_int64(selectbranches, 1);
		int64_t pos = sqlite3_column_int64(selectbranches, 2);
		int64_t document = sqlite3_column_int64(selectbranches, 3);

		bytes printed = bprintf(resp, "%ld %ld %ld %ld\n", 
				id, parent, pos, document).first;

		resp.as_void += printed.len,
		resp.len -= printed.len;
	}

	struct http_raw_response result = http_result(http_ok);
	result.payload = (bytes) { .as_void = mem.as_void, .len = mem.len - resp.len };

	return result;
}

http_callback_fun(user)
{
	bfound f = bfind(request->payload, Bs("\n"));

	bytes user = f.before;
	bytes password = f.after;

	sqlite3_reset(insertuser);
	sqlite3_bind_text(insertuser, 1, user.as_void, user.len, 0);
	sqlite3_bind_blob(insertuser, 2, password.as_void, password.len, 0);

	if (sqlite3_step(insertuser) != SQLITE_DONE) {
		return http_result(http_bad_request);
	} else {
		return http_result(http_ok);
	}
}

http_callback_fun(void)
{
	return http_result(http_ok);
}

http_callback_fun(newbranch)
{
	int64_t user = getuserid(request->header),
			parent, pos, document;

	if(user == 0) return http_result(http_bad_request);




	bscanf(request->payload, "%ld %ld %ld", &parent, &pos, &document);


	sqlite3_reset(insertbranch);
	sqlite3_bind_int64(insertbranch, 1, parent);
	sqlite3_bind_int64(insertbranch, 2, pos);
	sqlite3_bind_int64(insertbranch, 3, document);
	sqlite3_bind_int64(insertbranch, 4, user);

	if (sqlite3_step(insertbranch) != SQLITE_DONE)
		return http_result(http_bad_request);
	else {
		return http_result(http_ok);
	}
}

http_callback_fun(feed)
{
	bytes p = request->payload;
	bytes commit = 
	{ 
		.len = 0, 
		.as_void = p.as_void 
	};

	int file = -1;

	while(p.len > 0) 
	{
		if(p.as_char[0] == 'b')
		{
			if(file != -1) write(file, commit.as_void, commit.len);

			bytes bid = bslice(p, 1, 5);

			if(bid.len != 4)
			{
				debug("Needs 4 bytes.");
				return http_result(http_bad_request);
			}

			uint32_t id = ntohl(*((uint32_t*)bid.as_void));

			bytes path = balloc(256);
			bprintf(path, "./branches/%d", id);
			file = open(path.as_char, O_APPEND | O_CREAT | O_RDWR);
			bfree(&path);

			if(file == -1)
			{ 
				debug("Could not open a file: %s",strerror(errno));
				return http_result(http_bad_request);
			}

			p = bslice(p, 5, 0);

			commit = (bytes) 
			{ 
				.len = 0, 
				.as_void = p.as_void 
			};
		}
		else if(p.as_char[0] == 'i') 
		{
			if(file == -1) 
			{ 
				debug("No file.");
				return http_result(http_bad_request);
			}

			if(p.len < 13)
			{
				debug("Incomplete data structure.");
				return http_result(http_bad_request);
			}
			
			struct insert * i = (void *) (p.as_char + 1);
			i->lastlen = ntohl(i->lastlen);
			i->newlen = ntohl(i->newlen);

			size_t len = 13 + i->lastlen + i->newlen;

			if(p.len < len) {
				debug("Change lies."); 
				return http_result(http_bad_request);
			}

			commit.len += len;
			p = bslice(p, len, 0);
		}
		else {
			debug("Unknown opcode: %d", p.as_char[0]);
			return http_result(http_bad_request);
		}
	}

	write(file, commit.as_void, commit.len);

	return http_result(http_ok);
}

void stop()
{
	sqlite3_interrupt(db);

	sqlite3_stmt *s = 0;

	do {
		sqlite3_finalize(s);
		s = sqlite3_next_stmt(db, s);
	} while (s != 0);

	sqlite3_close(db);

	exit(0);
}

http_callback_fun(sql)
{

	int64_t user = getuserid(request->header);
	if(user == 0) return http_result(http_forbidden);

	

}

int main(int argc, char **argv)
{
	signal(SIGABRT, &stop);
	signal(SIGHUP, &stop);
	signal(SIGINT, &stop);
	signal(SIGTERM, &stop);

	epoll = epoll_create(1);
	sqlite3_open("./db", &db);

	DIR *d = opendir("./branches/");

	if (d == 0)
		debug("Could not open branches directory: %s", strerror(errno));

	struct dirent *ent = readdir(d);
	while (ent != 0) {
		if (ent->d_type == DT_REG)
			lastblob++;
		ent = readdir(d);
	}

#define ps(s, v) \
	if(sqlite3_prepare_v2(db,\
		s, -1, &v, 0) != SQLITE_OK) {\
		debug(s); \
		goto cleanup; }

	ps("select password, session from users where login = ?", selectpassword)
	ps("select session from users where id = ?", selectsession)
	ps("select id from users where session = ?", selectuserid)
	ps("update users set password = ? where session = ?", updatepassword)
	ps("update users set login = ? where session = ?", updatelogin)
	ps("update users set lastseen = datetime('now') where session = ?",
		updatelastseen)
	ps("insert into users(login, password, created, lastseen, session) "
		"values(?, ?, datetime('now'), datetime('now'), random())", insertuser)

	ps("insert into branches(parent, pos, document, user) "
		"values(?, ?, ?, ?)", insertbranch);
	ps("select id, parent, pos, document from branches where "
		"user = ?", selectbranches);


#define q(fun, path) { http_callback_ ## fun, Bs(path) }

	struct http_callback_pair pairs[] = {
		q(root, "/"),
		q(newbranch, "/api/0/newbranch"),
		q(branch, "/api/0/branch/"),
		q(branches, "/api/0/branches"),
		q(feed, "/api/0/feed"),
		q(user, "/api/0/user"),
		q(login, "/api/0/login"),
		q(sql, "/api/0/sql"),
		q(void, "")
	};
#undef q

	struct http_server *http = http_init();

	int i = 0;
	while (pairs[i].addr.len > 0) {
		http_server_add_callback(http, pairs[i]);
		i++;
	}

	int tls = setup_socket(8080, tls_listener_ondata,
					tls_listener_onclose, http);

	epoll_listen();

 cleanup:

	stop();

	return 0;
}
