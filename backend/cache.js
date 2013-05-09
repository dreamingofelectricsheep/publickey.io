var http = require('http'),
	url = require('url'),
	pgp = require('./openpgp.js'),
	fs = require ('fs')

	

var pubkeys = {},
	by_fingerprint = {},
	mail = {}


http.createServer(function (req, res)
	{
		var requrl = url.parse(req.url, true)

		if(requrl.pathname == '/pks')
		{
			if(req.method == 'PUT')
			{
				var armored = ''
				req.addListener('data', function(chunk) { armored += chunk })
				req.addListener('end', function()
					{
						try
						{
							var key = pgp.openpgp.read_publicKey(armored)[0]

							var email = key.userIds[0].text


							pubkeys[email] = armored

							res.writeHead(200)
						}
						catch(e)
						{
							res.writeHead(404)
						}
						
						res.end()
					})
			}

		}
		else if(requrl.pathname == '/pks/lookup')
		{
			if(req.method == 'GET')
			{
				if(requrl.query.op == 'index' || 
					requrl.query.search.substr(0, 2) == '0x')
				{
					res.writeHead(501)
					res.end()
				}

				var email = '<' + requrl.query.search + '@publickey.io>'

				res.writeHead(200, {'Content-Type': 'text/plain'})
				res.end(pubkeys[email])
			}
		}
		else if(requrl.pathname == '/mail')
		{
			if(req.method == 'PUT')
			{
				var packets= ''
				req.addListener('data', function(chunk) { packets += chunk })
				req.addListener('end', function()
					{
						packets = pgp.openpgp.read_message(packets)

						res.writeHead(200, {'Content-Type': 'text/plain'})
						res.end('Hello World\n')
					})
			}

		}
		else if(requrl.pathname == '/storeemail')
		{
			var email= ''
			req.addListener('data', function(chunk) { email += chunk })
			req.addListener('end', function()
				{
					fs.appendFile('emails.txt', email.substr(0, 255) + '\n')

					res.writeHead(200, {'Content-Type': 'text/plain'})
					res.end()
				})
		}
		else
		{
			fs.readFile('../frontend' + requrl.pathname, function(err, data)
				{
					res.writeHead(200, {'Content-Type': 'text/html'})
					res.end(data)
				})
		}
	}).listen(1337)
