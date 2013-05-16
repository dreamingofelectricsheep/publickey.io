var tags = require('../tags'),
	key_icon = require('./key-icon.js')

module.exports = function()
{

	var list = JSON.parse(localStorage['emails'])
	var view = tags.table({})

	var page = tags.fragment({}, 
		key_icon(),
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


