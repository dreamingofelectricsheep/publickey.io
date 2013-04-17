
module('view_public_key', function(tags) {
return function()
{
	var box = tags.div(
		{ 
			class: 'box',
			style:
			{
				background: white
			}
		},
		tags.div({ name: 'close', class: 'close-button' }, 'Close'),
		tags.div({ class: 'armored', },
			openpgp.keyring.publicKeys[0].armored))

	box.$close.onclick = function()
	{
		panel.parentElement.removeChild(panel)
	}

	return box
}
})

module('view_keys', function(tags, view_public_key) {
return function()
{
	var panel = tags.div({ class: 'overlay' },
		tags.div({ name: 'mgr', class: 'box' }))

	var close = tags.div({ class: 'close-button' }, 'Close')
	close.onclick = function()
	{
		panel.parentElement.removeChild(panel)
	}

	panel.$mgr.appendChild(close)

	var key = openpgp.keyring.privateKeys[0]

	if(key != undefined)
	{
		panel.$mgr.appendChild(tags.div({ class: 'armored' },
			key.obj.extractPublicKey()))

		var button = tags.div({ class: 'button' }, 'Discard this key')
		button.onclick = function()
		{
			openpgp.keyring.removePrivateKey(0)
		}

		panel.$mgr.appendChild(button)
	}
	else
	{
		panel.$mgr.appendChild(tags.div({ style: 'text-align: center;' },
			tags.div({ 
				class: 'icon-key icon-4x', 
				style:
				{
					display: 'block',
					marginBottom: '30px'
				}}),
			'You don\'t have a key. Without one, you can\'t send secure messages.'))

		var generate = tags.div({ class: 'button' }, 'Generate a new key')
		var importkey = tags.div({ class: 'button' }, 'Import')

		panel.$mgr.appendChild(generate)
		
		generate.onclick = function()
		{
			panel.$mgr.innerHTML = ''

			panel.$mgr.appendChild(tags.div({}, tags.div(
				{ 
					class: 'icon-spinner icon-spin icon-4x',
					style: 
					{
						display: 'block',
						margin: '40px 150px'
					}
				}),
				'This might take a couple of minutes.'))

			setTimeout(function()
				{
					var key = openpgp.generate_key_pair(1, 2048, '', '')
					openpgp.keyring.importPrivateKey(key.privateKeyArmored, '')
					openpgp.keyring.importPublicKey(key.publicKeyArmored)
					openpgp.keyring.store()

					panel.parentElement.removeChild(panel)
				}, 10)
		}


		panel.$mgr.appendChild(importkey)

		importkey.onclick = function()
		{

		}
	}


	return panel
}
})

module('view_key_icon', function(tags, view_keys) {
return function() 
{
	var icon = tags.div({ class: 'key icon-key icon-2x' })

	icon.onclick = function()
	{
		this.parentElement.appendChild(view_keys())
	}

	return icon

}
})
	


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
					var edit = tags.span({ class: 'clickable' }, '●')
					var cont = tags.div({ class: 'contact' },
						edit,
						tags.span({ name: 'messages' },
							card.name,
							card.email.length == 0 ? undefined :
								tags.span({ class: 'email' }, ' <' 
									+ card.email + '>')))

					edit.onclick = function()
					{
						c.parentElement.appendChild(view_card(card, function(p)
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
	

module('model_email', function() {

function email(id)
{
	try
	{
		var self = this
		each(JSON.parse(localStorage['email:' + id]),
			function(v, k)
			{
				self[k] = v
			})
	}
	catch(e)
	{
		this.ciphertext = ''
	}
}

email.prototype = 
{
	store: function()
	{
		if(this.id != undefined)
			localStorage['email:' + this.id] = JSON.stringify(this.ciphertext)
	},
	put: function(ciphertext)
	{
		this.ciphertext = ciphertext
		this.updateid()
		this.store()
	},
	updateid: function()
	{
		this.id = str_sha256(this.ciphertext)
	}
}

})

module('view_all_emails', function(tags) {
return function(list)
{
	var view = tags.table({})

	each(list, function(email)
		{
			view.appendChild(tags.tr({},
				tags.td({}, email.id),
				tags.td({}, email.date),
				tags.td({}, email.peer)))
		})
	return view
}
})

module('view_new_email', function(tags, model_email) {
return function()
{
	var editor = tags.div({},
		tags.textarea({ name: 'pubkey' }),
		tags.textarea({ name: 'text' }),
		tags.input({ name: 'send', type: 'button', value: 'Send' }))

	editor.$send.onclick = function()
	{
		var mail = editor.$text.value,
			priv_key = openpgp.keyring.privateKeys[0].obj,
			pub_key = openpgp.read_publicKey(editor.$pubkey.value)

		if (pub_key < 1)
		{
			alert('Error processing the public key!')
			return
		}

		priv_key.decryptSecretMPIs('')

		var encrypted = openpgp.write_signed_and_encrypted_message(
			priv_key,
			pub_key,
			mail)

		console.log(openpgp_encoding_deArmor(encrypted))
	}

	return editor
}
})


module('entry', function(view_contacts, view_card, dom,
	view_all_emails, view_new_email, view_key_icon) {

var body = dom.body

body.appendChild(view_key_icon())

openpgp.init()



body.appendChild(view_contacts())
body.appendChild(view_new_email())

var emails = JSON.parse(localStorage['emails_all'])

body.appendChild(view_all_emails())

})



