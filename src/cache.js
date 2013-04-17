var http = require('http'),
	url = require('url'),
	pgp = require('./openpgp.js')

	

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
				var pubkey = ''
				req.addListener('data', function(chunk) { pubkey += chunk })
				req.addListener('end', function()
					{
						pubkey = pgp.openpgp_encoding_deArmor(pubkey.replace(/\r/g, ''))
							.openpgp
						pubkey = pgp.openpgp_packet.read_packet(pubkey, 0, pubkey.length)
						console.log(p)
					})
			}

			res.writeHead(200, {'Content-Type': 'text/plain'})
			res.end('Hello World\n')
		}
		else if(requrl.pathname == '/pks/lookup')
		{
			if(req.method == 'GET')
			{
				if(requrl.query.op != 'get' || 
					requrl.query.search.substr(0, 2) == '0x')
				{
					res.writeHead(501)
					res.end()
				}

				var email = requrl.query.search
			}
		}
		else
		{
			res.writeHead(200, {'Content-Type': 'text/plain'})
			res.end('Hello World\n')
		}
	}).listen(1337)
