var tags = require('../tags')

module.exports = function()
{
	var add = tags.div({ class: 'button' }, 'Add a new contact')

	var list = window.contacts

	var view = tags.div({ style: {
				boxShadow: '0 0 30px -5px black',
				background: 'white',
				position: 'fixed',
				left: '20%',
				top: '0',
				transition: 'left 1s ease',
				boxSizing: 'border-box',
				padding: '1em',
				width: '80%',
				height: '100%',
				overflowY: 'scroll',
				overflowX: 'hidden'
		}},
		add,
		tags.table({ style:
			{
				tableLayout: 'fixed',
				width: '100%',
			}}))

	each(list, function(contact)
		{
			var key;

			if(contact.key) key = tags.span({ class: 'key-icon', 
				style: { color: '#80DE66' }})

			tags.append(view.firstChild, 
				tags.tr({ style: { color: 'gray', height: '1em' }},
					tags.td({ style: { color: '#C9C9C9' }}, 
						contact.name),
					tags.td({ style: { }},
						contact.email),
					tags.td({ style: { width: '2em' }}, 
						key)))
		})

	return view
}


