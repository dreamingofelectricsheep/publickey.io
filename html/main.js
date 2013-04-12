


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

		this.list.push(card)
		this.by_id[card.id] = card
		this.store()
	}
	this.store = function()
	{
		localStorage['contacts'] = JSON.stringify(this.list)
	}
}

function contactsview()
{
	var c = tags.div({ class: 'contacts' })

	var categories = {}

	each(contacts.list, function(card)
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
					cat_view.appendChild(tags.div({ class: 'contact' },
						card.name,
						card.email.length == 0 ? undefined :
							tags.span({ class: 'email' }, ' <' + card.email + '>')))
				})

			c.appendChild(cat_view)
		})
	
	var add_new = tags.div({ class: 'add' }, 'add a new contact...')

	add_new.onclick = function()
	{
		c.parentElement.appendChild(cardview())
	}

	c.appendChild(add_new)

	return c
}


function cardview(contact)
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

	var save = input({ type: 'button', value: 'Save' })

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
		tr({}, td({ colspan: '2' }, save)))

	}

	each(inputs, function(v, k)
		{
			var n = '$' + k
			c[n] = v

			if(contact[k] != undefined)
				v.value = contact[k]
		})

	save.onclick = function()
	{
		contacts.change(
			{ 
				id: contact.id, 
				name: inputs.name.value,
				email: inputs.email.value,
				tags: inputs.tags.value.split(' '),
				key: inputs.key.value
			})
		c.parentElement.removeChild(c)
	}
	

				

	return c
}
	



window.onload = function() {

var body = document.getElementsByTagName('body')[0]

openpgp.init()

if(openpgp.keyring.privateKeys.length == 0)
{
	var key = openpgp.generate_key_pair(1, 4096, '', '')
	openpgp.keyring.importPrivateKey(key.privateKeyArmored, '')
	openpgp.keyring.importPublicKey(key.publicKeyArmored)
	openpgp.keyring.store()
}

body.appendChild(contactsview())












}

