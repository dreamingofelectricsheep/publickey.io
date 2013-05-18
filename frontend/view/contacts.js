

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


var tags = require('../tags')


module.exports = function()
{
	var c = tags.div({ style:
		{
			background: '#110617',
			width: '70%',
			padding: '1em 15%',
			marginBottom: '1em',
			color: 'white',
			textAlign: 'left'
		}})

	var categories = {}

	each(contacts.list, function(card)
		{
			each(card.tags, function(tag)
				{ 
					if(categories[tag] == undefined) categories[tag] = []
					categories[tag].push(card)
				})
		})

	var container = tags.ul({ class: 'category' })

	each(categories, function(cat, name)
		{
			var ul = tags.ul({})

			each(cat, function(card)
				{
					var cont = tags.li({},
							card.name,
							card.email.length == 0 ? undefined :
								tags.span({ class: 'email' }, ' <' 
									+ card.email + '>'))

					/*edit.onclick = function()
					{
						c.parentElement.appendChild(view_card(card, function(p)
							{
								p.appendChild(view_contacts())
							}))

						c.parentElement.removeChild(c)
					}*/

					tags.append(ul, cont)
				})

			var collapsible = tags.li({},
				tags.label({ 'for': name }, name),
				tags.input({ type: 'checkbox', id: name }),
				ul)

			tags.append(container, collapsible)
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

	tags.append(c, container)


	//c.appendChild(add_new)

	return c
}
