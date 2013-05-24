var tags = require('../tags')

module.exports = function()
{
	var contacts = tags.span({ style: 
		{ 
			color: '#8AC1D1',
			textDecoration: 'underline',
			cursor: 'pointer'
		}}, 'Contacts')

	contacts.onclick = function()
	{
		switch_state('contacts')
	}



	var sidebar = tags.div({ style:
		{
			height: '100%',
			width: '100%',
		}}, contacts)





	return sidebar
}
