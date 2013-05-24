var tags = require('../tags')

module.exports = function()
{

	var list = window.emails
	var view = tags.table({ style:
			{
				tableLayout: 'fixed',
				width: '100%',
			}})

	each(list, function(email)
		{
			email.date = new Date(email.date)

			var dir = '', peer;
			if(email.from != undefined) {

				peer = email.from
			}
			else {
				dir = 'icon-double-angle-right'
				peer = email.to
			}

			var attachment = undefined;
			
			if(Math.random() < 0.2)
				attachment = tags.div({ 
					title: 'This message has attachments',
					class: 'icon-paper-clip icon-large'
				})

			tags.append(view, 
				tags.tr({ style: { color: 'gray', height: '1em' }},
					tags.td({ style: { width: '1em', color: '#C9C9C9' }}, 
						tags.span({ class: dir + ' icon-large' })),
					tags.td({ style: { width: '180px' }},
						tags.div({}, peer)),
					tags.td({ style: { width: '2em' }}, 
						attachment),
					tags.td({ style: { overflow: 'hidden', color: '#C9C9C9', 
						whiteSpace: 'nowrap'} }, email.body)))
		})

	return view
}


