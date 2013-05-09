
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
	var panel = tags.fragment({},
			tags.div({ name: 'back', class: 'back-button' }, '« Back'))

	panel.$back.onclick = function()
	{
		history.back()
	}

	var key = openpgp.keyring.privateKeys[0]

	var view_key = function(key)
	{
		var armored = tags.div({ class: 'armored' },
			key.obj.extractPublicKey())

		var button = tags.div({ class: 'button' }, 'Discard this key')
		button.onclick = function()
		{
			openpgp.keyring.removePrivateKey(0)
		}

		return tags.div({}, armored, button)
	}

	var view_generate = function()
	{
		var form = tags.div({}, tags.div({}, 'Pick an email address.'),
			tags.input({ 
				name: 'addr',
				type: 'text', 
				placeholder: 'fancymail',
				style: { textAlign: 'right' }}),
			tags.span({ class: 'light-text' }, '@publickey.io'))
			
		var generate = tags.div({ class: 'button' }, 'Generate')

		generate.onclick = function()
		{
			this.innerHTML = ''

			tags.append(this, tags.div({ class: 'icon-spinner icon-spin' }),
				'Generating...')


			var key = openpgp.generate_key_pair(1, 512, 
				'<' + form.$addr.value + '@publickey.io>', '')

			var req = new XMLHttpRequest()
			req.open('PUT', '/pks')
			var self = this
			req.onreadystatechange = function()
			{
				if(this.readyState == 4)
				{
					openpgp.keyring.importPrivateKey(key.privateKeyArmored, '')
					openpgp.keyring.importPublicKey(key.publicKeyArmored)
					openpgp.keyring.store()

					self.innerHTML = ''
					tags.append(self, 'Done!')
				}
			}

			req.send(key.publicKeyArmored)

		}

		return tags.div({}, form, generate)
	}

	var view_keygen = function()
	{
		var icon = tags.div({ style: 'text-align: center;' },
			tags.div({ 
				class: 'icon-key icon-4x', 
				style:
				{
					display: 'block',
					marginBottom: '30px'
				}}),
			'You don\'t have a key. Without one, you can\'t send secure messages.')

		var generate = tags.div({ class: 'button' }, 'Generate a new key')
		var importkey = tags.div({ class: 'button' }, 'Import')

		generate.onclick = function()
		{
			panel.removeChild(this.parentElement)

			panel.appendChild(view_generate())
		}


		importkey.onclick = function()
		{

		}

		return tags.div({}, icon, generate, importkey)
	}

	if(key != undefined)
	{
		panel.appendChild(view_key(key))
	}
	else
	{
		panel.appendChild(view_keygen())
	}


	return tags.div({ class: 'overlay' }, panel)
}
})

module('view_key_icon', function(tags, state) {
return function() 
{
	var icon = tags.div({ class: 'key icon-key icon-2x' })

	icon.onclick = function()
	{
		state('view_keys')
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

module('view_all_emails', function(tags, view_key_icon) {
return function()
{

	var list = JSON.parse(localStorage['emails'])
	var view = tags.table({})

	var page = tags.fragment({}, 
		view_key_icon(),
		view)

	each(list, function(email)
		{
			email.date = new Date(email.date)

			var dir, peer;
			if(email.from != undefined) {

				dir = 'from '
				peer = email.from
			}
			else {
				dir = 'to'
				peer = email.to
			}

			var attachment = undefined;
			
			if(Math.random() < 0.2)
				attachment = tags.div({ 
					title: 'This message has attachments',
					class: 'icon-paper-clip icon-large', 
					style: { display: 'inline-block', padding: '5px' }
				})

			tags.append(view, tags.tr({ style: { height: '130px', }},
				tags.td({ style: { width: '150px' } }, 
					tags.div({ style: { fontSize: '0.8em' } }, dir),
					tags.div({ style: { fontSize: '1.5em' } }, peer),
					tags.div({ style: { fontSize: '0.5em' }, 
						title: email.date.toString() }, email.date.toDateString()),
					attachment),
				tags.td({ style: { color: '#827887' } }, email.body)))
		})

	return page
}
})

module('view_new_email', function(tags, model_email) {
return function()
{


	var page = tags.fragment({},
		tags.div({ name: 'back', class: 'back-button' }, '« Back'),
		tags.textarea({ name: 'pubkey', style: { marginTop: '150px' } }),
		tags.textarea({ name: 'text' }),
		tags.div({ name: 'send', class: 'button' },'Send' ))

	page.$back.onclick = function()
	{
		history.back()
	}

	page.$send.onclick = function()
	{
		var mail = page.$text.value,
			priv_key = openpgp.keyring.privateKeys[0].obj,
			pub_keys = openpgp.read_publicKey(page.$pubkey.value)

		if (pub_keys < 1)
		{
			alert('Error processing the public key!')
			return
		}

		pub_keys.push(openpgp.read_publicKey(
			openpgp.keyring.privateKeys[0].obj.extractPublicKey())[0])

		priv_key.decryptSecretMPIs('')

		var encrypted = openpgp.write_signed_and_encrypted_message(
			priv_key,
			pub_keys,
			mail)

		var req = new XMLHttpRequest()
		req.open('PUT', '/mail')
		req.onreadystatechange = function()
		{
			if(this.readyState == 4)
			{
				alert('done')
			}
		}

		req.send(encrypted)


		console.log(encrypted)
	}

	return page
}
})


module('state', function(require, tags) {

function switch_state(view)
{
	while(tags.body.firstChild)
		tags.body.removeChild(tags.body.firstChild)

	tags.append(tags.body, require(view)())

	if(history.state == null)
		history.replaceState(view, '')
	else
		history.pushState(view, '')
}

window.onpopstate = function()
{
	var state = history.state

	if(state == null) return
		
	switch_state(state)
}

return switch_state
})


module('view_splash', function(tags) { return function() {

var page = tags.div({ style: {
		display: 'inline-block',
		width: '700px'
	}},
	tags.div({ style: {
		display: 'block',
		margin: '50px',
	}, class: 'icon-key icon-4x' }),
	tags.div({ style:
		{
			margin: '20px',
			fontSize: '2em'
		} },
		'publickey.io'),
	tags.div({ style:
		{
			color: '#E8CEF5',
			fontSize: '0.8em'
		} },
		'OpenPGP encrypted email right in your browser.'),
	tags.div({ style: {
			color: '#E8CEF5',
		margin: '40px',
		textAlign: 'justify'
	}}, 'Publickey.io is an effort to create an easy-to-use, no-fuss-required ' +
		"email encryption solution that puts focus on accessibility first. Let's be honest - " +
		'the "absolute security" approach to cryptography has failed in case of email. ' +
		'Currently available tools are confusing and difficult to use; scaring users with ' +
		'bit lengths, alien acronyms and incomprehensible choices. We aim to change that.'),
	tags.div({ style: {
			color: '#E8CEF5',
			fontSize: '0.9em'
		}}, 'Send me a notification when publickey.io launches!'),
	tags.input({ style: {
	}, name: 'input', type: 'text', placeholder: 'johndoe@example.com' }),
	tags.div({ class: 'button', name: 'button' }, 
		'Send me a notification!'))

page.$button.onclick = function() {
	var email = page.$input.value
	var req = new XMLHttpRequest()
	req.open('POST', '/storeemail')
	req.send(email)

	page.removeChild(page.$input)
	this.onclick = undefined;
	this.innerHTML = 'Thanks for showing interest!'
}
	

return page } })


module('entry', function(dom, state) {

var body = dom.body


openpgp.init()



//var emails = JSON.parse(localStorage['emails_all'])
var emails = []
var names = ['Sean Bean', 'Tony Stark', 'The Incredible Hulk', 'Anthony Hopkins']

for(var i = 0; i < 100; i++) {
	emails.push({
		to: names[Math.floor(names.length * Math.random())],
		body: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.' +
		 'Fusce mollis, mauris eu ullamcorper rutrum, enim urna aliquam nisi,' +
		 'in tempus nulla erat ut libero. Pellentesque tempor dolor nec sem ornare' +
		 'in varius sapien gravida. Ut gravida auctor orci et pulvinar. Nulla facilisi.',
		date: new Date(),
	})		 
}

localStorage.emails = JSON.stringify(emails)


//if(location.origin.indexOf('publickey.io') != -1)
	state('view_splash')
//state('view_all_emails')
//else
//	state('view_new_email')



var colors = {
	darkmedium: '#533B5E',
	dark: '#1E0729',
	medium: '#AE67CF',
	light: '#E8CEF5'
}

var style = document.getElementById('style')


each(colors, function(v, k) {
	var r = new RegExp(k, 'g')
	style.innerHTML = style.innerHTML.replace(r, v)
})


})
