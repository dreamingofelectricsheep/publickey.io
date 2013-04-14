


module('model_contacts', function() {

var contacts = new function()
{
	try
	{
		this.list = JSON.parse(localStorage['contacts'])
	}
	catch(e)
	{
		this.list = []
	}

	var by_id = this.by_id = {}

	each(this.list, function(card)
		{
			by_id[card.id] = card
		})

	this.change = function(card)
	{
		if(card.id == undefined)
			card.id = new Date().getTime()

		if(this.by_id[card.id] == undefined)
		{
			this.list.push(card)
			this.by_id[card.id] = card
		}
		else
		{
			var old = this.by_id[card.id]
			each(card, function(v, k) { old[k] = v })
		}
		this.store()
	}

	this.remove = function(id)
	{
		delete this.by_id[id]
		this.list.splice(this.list.indexOf(id), 1)
		this.store()
	}

	this.store = function()
	{
		localStorage['contacts'] = JSON.stringify(this.list)
	}
}

return contacts
})


module('view_contacts', function(tags, model_contacts, view_card) {

return function view_contacts()
{
	var c = tags.div({ class: 'contacts' })

	var categories = {}

	each(model_contacts.list, function(card)
		{
			each(card.tags, function(tag)
				{ 
					if(categories[tag] == undefined) categories[tag] = []
					categories[tag].push(card)
				})
		})


	each(categories, function(cat, name)
		{
			var cat_view = tags.div({ class: 'category' }, 
				tags.div({ class: 'label' }, name))

			each(cat, function(card)
				{
					var cont = tags.div({ class: 'contact' },
						card.name,
						card.email.length == 0 ? undefined :
							tags.span({ class: 'email' }, ' <' + card.email + '>'))

					cont.$card = card
					cont.onclick = function()
					{
						c.parentElement.appendChild(view_card(this.$card, function(p)
							{
								p.appendChild(view_contacts())
							}))
						c.parentElement.removeChild(c)
					}

					cat_view.appendChild(cont)
				})

			c.appendChild(cat_view)
		})
	
	var add_new = tags.div({ class: 'add' }, 'add a new contact...')

	add_new.onclick = function()
	{
		c.parentElement.appendChild(view_card(undefined, function(p)
			{
				p.appendChild(view_contacts())
			}))
		c.parentElement.removeChild(c)
	}

	c.appendChild(add_new)

	return c
}
})


module('view_card', function(tags, model_contacts) {

return function(contact, ondone)
{
	var c, inputs;

	if(contact == undefined) contact = {}

	with(tags)
	{
	inputs = 
	{
		name: input({ type: 'text', placeholder: 'John Doe' }),
		email: input({ type: 'email', 
				placeholder: 'email@example.com' }),
		tags: input({ type: 'text', placeholder: 'friends, family, business' }),
		key: textarea({})
	}

	var save = input({ type: 'button', value: 'Save' }),
		remove = input({ type: 'button', value: 'Remove' }),
		discard = input({ type: 'button', value: 'Discard Changes' })

	c = table({ class: 'cardview' },
		caption({}, 'Editing a contact'),
		tr({}, 
			td({}, 'Name'),
			td({}, inputs.name)),
		tr({},
			td({}, 'Email'),
			td({}, inputs.email)),
		tr({},
			td({}, 'Tags'),
			td({}, inputs.tags)),
		tr({},
			td({}, 'Public Key'),
			td({}, inputs.key)),
		tr({}, td({ colspan: '2' }, save, remove, discard)))

	}

	each(inputs, function(v, k)
		{
			var n = '$' + k
			c[n] = v

			var item = contact[k]
			if(item != undefined)
			{
				if(item instanceof Array)
					v.value = item.join(' ')
				if(typeof item == 'string')
					v.value = item
			}
		})

	save.onclick = function()
	{
		model_contacts.change(
			{ 
				id: contact.id, 
				name: inputs.name.value,
				email: inputs.email.value,
				tags: inputs.tags.value.split(' '),
				key: inputs.key.value
			})
		discard.click()
	}

	discard.onclick = function()
	{
		var p = c.parentElement
		p.removeChild(c)

		ondone(p)
	}

	remove.onclick = function()
	{
		model_contacts.remove(contact.id)
		discard.click()
	}

	return c
}
})
	



module('entry', function(view_contacts, view_card, dom) {

var body = dom.body

openpgp.init()

if(openpgp.keyring.privateKeys.length == 0)
{
	var key = openpgp.generate_key_pair(1, 4096, '', '')
	openpgp.keyring.importPrivateKey(key.privateKeyArmored, '')
	openpgp.keyring.importPublicKey(key.publicKeyArmored)
	openpgp.keyring.store()
}

body.appendChild(view_contacts())

})

