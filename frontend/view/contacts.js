var tags = require('../tags'),
	model_contacts = require('../model/contacts.js')

module.exports = function()
{
	var add = tags.div({ class: 'button' }, 'Add a new contact')
	add.onclick = function() { switch_state('edit-contact') }

	var view = tags.div({},
		add,
		tags.table({ style:
			{
				tableLayout: 'fixed',
				width: '100%',
			}}))

	each(model_contacts.list, function(contact)
		{
			var key;

			if(contact.key) key = tags.span({ class: 'key-icon', 
				style: { color: '#80DE66' }})

			tags.append(view.lastChild, 
				tags.tr({ style: { color: 'gray', height: '1em' }},
					tags.td({ style: { color: '#C9C9C9' }}, 
						contact.name),
					tags.td({ style: { }},
						contact.email),
					tags.td({ style: { width: '2em' }}, 
						key)))

			view.lastChild.lastChild.onclick = function()
			{
				switch_state('edit-contact', contact)
			}
		})

	return view
}


