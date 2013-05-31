var tags = require('../tags')


module.exports = function()
{

	var page = tags.div({},
		tags.div({},
		tags.input({ placeholder: 'someone@example.com' })),
		tags.textarea({ style: 
			{
				width: '600px',
				height: '500px',
				padding: '10px'
			} }))



	return page
}

