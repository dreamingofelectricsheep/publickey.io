var emails = require('./emails.js'),
	sidebar = require('./sidebar.js'),
	tags = require('../tags')


module.exports = function()
{
	var page = tags.fragment({}, 
		emails(),
		tags.div({ class: 'nav', style:
			{
				position: 'fixed',
				bottom: '0',
				padding: '13px',
				width: '100%',
				background: '#90D66D'
			} },
			tags.span({}, 'Mailbox'),
			tags.span({}, 'Contacts'),
			tags.span({}, 'Configuration')))


	return page
}

