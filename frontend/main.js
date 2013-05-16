var view = require('./view'),
	tags = require('./tags')

function switch_state(view_name)
{
	var body = document.getElementsByTagName('body')[0]

	while(body.firstChild)
		body.removeChild(body.firstChild)

	tags.append(body, view[view_name]())

	if(history.state == null)
		history.replaceState(view_name, '')
	else
		history.pushState(view_name, '')
}

window.onpopstate = function()
{
	var state = history.state

	if(state == null) return
		
	switch_state(state)
}








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





var colors = {
	darkmedium: '#533B5E',
	dark: '#1E0729',
	medium: '#AE67CF',
	light: '#E8CEF5'
}



window.onload = function() {

	var style = document.getElementById('style')


	each(colors, function(v, k) {
		var r = new RegExp(k, 'g')
		style.innerHTML = style.innerHTML.replace(r, v)
	})



	//state('view_all_emails')
	switch_state('emails')
}

