var view = require('./view'),
	tags = require('./tags')

window.switch_state = function(view_name, data)
{
	var body = document.getElementsByTagName('body')[0]

	body.removeChild(body.lastChild)


	tags.append(body, view[view_name](data))

	if(history.state == null)
		history.replaceState({ view: view_name, data: data }, '')
	else
		history.pushState({ view: view_name, data: data }, '')
}

window.onpopstate = function()
{
	var state = history.state

	if(state == null) return
		
	switch_state(state.view, state.data)
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

window.emails = emails






window.onload = function() {



	var body = document.getElementsByTagName('body')[0]
	tags.append(body, view.navbar(), tags.div({}))

	switch_state('emails')

}

