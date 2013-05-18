var emails = require('./emails.js'),
	sidebar = require('./sidebar.js'),
	tags = require('../tags')


module.exports = function()
{
	var page = tags.fragment({}, 
		sidebar(),
		emails())




	return page
}

