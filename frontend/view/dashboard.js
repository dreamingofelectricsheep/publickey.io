var emails = require('./emails.js'),
	tags = require('../tags')


module.exports = function()
{
	var page = tags.fragment({}, 
		tags.ul({ class: 'nav', style:
			{
				margin: '0',
				position: 'fixed',
				bottom: '0',
				left: '0',
				padding: '10px',
				width: '100%',
				background: '#90D66D',
				boxShadow: '0 0 10px 0 silver',
			} },
			tags.li({ class: 'icon-key' }),
			tags.li({ style:
			{
				textShadow: '0 0 0.5em #694113',
				boxShadow: '0 0 0.5em 0 #496E37',
				border: '1px solid #B87B35',
				background: '#E08F31',
				borderRadius: '0.2em',
				padding: '0.3em 1.8em'
			} }, 'Send'),
			tags.li({ name: 'emails' }, 'Mailbox'),
			tags.li({ name: 'contacts' }, 'Contacts'),
			tags.li({}, 'Configuration')),
		emails())
	
	each(page.firstChild.children, function(child)
		{
			child.onclick = function()
			{
				switch_state(this.getAttribute('name'))
			}
		})


	return page
}

