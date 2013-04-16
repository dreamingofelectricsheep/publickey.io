var http = require('http'),
	packet = require('./packet.js'),
	armor = require('./armor.js')

	

var by_fingerprint = {},
	mail = {}

http.createServer(function (req, res)
	{
		if(req.method == 'PUT')
		{
			var data = ''
			req.addListener('data', function(chunk) { data += chunk })
			req.addListener('end', function()
				{
					var raw = armor.dearmor(data).openpgp
					console.log(raw)
					var p = packet.openpgp_packet.read_packet(raw, 0, raw.length)
					console.log(p)
				})
		}
		
		res.writeHead(200, {'Content-Type': 'text/plain'})
		res.end('Hello World\n')
	}).listen(1337)
