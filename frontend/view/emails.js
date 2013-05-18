var tags = require('../tags')

module.exports = function()
{

	var list = window.emails
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
		tags.table({ style:
			{
				tableLayout: 'fixed',
				width: '100%',
			}}))

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

			tags.append(view.firstChild, 
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

	var u, o = function() { view.style.left = '100%'
		setTimeout(u, 1000) }

	u = function() { view.style.left = '20%'
		setTimeout(o, 1000) }

//	u()

	return view
}


