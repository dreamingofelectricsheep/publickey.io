var tags = require('../tags')

module.exports = function() 
{
	var icon = tags.div({ class: 'key icon-key icon-2x' })

	icon.onclick = function()
	{
		state('view_keys')
	}

	return icon

}
