;(function(e,t,n){function i(n,s){if(!t[n]){if(!e[n]){var o=typeof require=="function"&&require;if(!s&&o)return o(n,!0);if(r)return r(n,!0);throw new Error("Cannot find module '"+n+"'")}var u=t[n]={exports:{}};e[n][0].call(u.exports,function(t){var r=e[n][1][t];return i(r?r:t)},u,u.exports)}return t[n].exports}var r=typeof require=="function"&&require;for(var s=0;s<n.length;s++)i(n[s]);return i})({1:[function(require,module,exports){
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


},{"./view":2,"./tags":3}],3:[function(require,module,exports){

each = function(obj, f)
{
	if(obj instanceof Array)
		for(var i = 0; i < obj.length; i++)
			f(obj[i], i, obj)
	else
		for(var i in obj)
			f(obj[i], i, obj)
}

range = function()
{
	var r = [],
		step = 1,
		start = 0,
		end = 0

	switch(arguments.length)
	{
		case 1:
			end = arguments[0]
			break
		case 2:
			start = arguments[0]
			end = arguments[1]
			break
		case 3:
			start = arguments[0]
			end = arguments[1]
			step = arguments[2]
			break
	}
	
	for(var i = start; i < end; i += step)
		r.push(i)

	return r
}


function bind(element, child)
{
	if(child == undefined) return

	if(typeof child != 'object')
		child = document.createTextNode(child)


	element.appendChild(child)

	if(child.getAttribute)
	{
		var name = child.getAttribute('name')

		if(name != undefined)
			element['$' + name] = child
	}
}

function tags(tag, options, children)
{
	var element = tag == 'fragment' ?
		document.createDocumentFragment() :
		document.createElement(tag)

	if(element.setAttribute)
		for(var i in options)
		{
			if(i == 'style')
			{
				for(var j in options[i])
					element.style[j] = options[i][j]
			}
			else
			{
				element.setAttribute(i, options[i])
			}
		}


	each(children, 
		function(c)
		{
			bind(element, c)
		})

	return element
}

var text_tags = ['html', 'div', 'p', 'input', 'a', 'textarea', 'canvas',
	'td', 'tr', 'table', 'fieldset', 'form', 'legend', 'caption',
	'fragment',	'span', 'ul', 'li', 'br']

each(text_tags, function(tag) 
	{ 
		tags[tag] = function(options) 
		{ 
			var children = Array.prototype.slice.call(arguments)

			if(typeof options != undefined)
				children = children.slice(1)

			return tags(tag, options, children) 
		} 
	})


tags.append = function(parent)
{
	var children = Array.prototype.slice.call(arguments, 1)

	each(children, function(c)
		{
			bind(parent, c)
		})

	return parent
}

module.exports = tags

},{}],2:[function(require,module,exports){
module.exports = {
	'emails': require('./emails.js'),
	'key-icon': require('./key-icon.js')
}


},{"./emails.js":4,"./key-icon.js":5}],4:[function(require,module,exports){
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



},{"./key-icon.js":5,"../tags":3}],5:[function(require,module,exports){
var tags = require('../tags')

module.exports = function() 
{
	var icon = tags.div({ class: 'key icon-key icon-2x' })

	icon.onclick = function()
	{
		state('view_keys')
	}

	return icon

}

},{"../tags":3}]},{},[1])
//@ sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoiZ2VuZXJhdGVkLmpzIiwic291cmNlcyI6WyIvaG9tZS9wYW5jYWtlL2NvZGUvcHVibGlja2V5LmlvL2Zyb250ZW5kL21haW4uanMiLCIvaG9tZS9wYW5jYWtlL2NvZGUvcHVibGlja2V5LmlvL2Zyb250ZW5kL3RhZ3MvdGFncy5qcyIsIi9ob21lL3BhbmNha2UvY29kZS9wdWJsaWNrZXkuaW8vZnJvbnRlbmQvdmlldy9pbmRleC5qcyIsIi9ob21lL3BhbmNha2UvY29kZS9wdWJsaWNrZXkuaW8vZnJvbnRlbmQvdmlldy9lbWFpbHMuanMiLCIvaG9tZS9wYW5jYWtlL2NvZGUvcHVibGlja2V5LmlvL2Zyb250ZW5kL3ZpZXcva2V5LWljb24uanMiXSwibmFtZXMiOltdLCJtYXBwaW5ncyI6IjtBQUFBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTs7QUNoRkE7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTs7QUN6SEE7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBOztBQ0xBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBOztBQ25EQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0EiLCJzb3VyY2VzQ29udGVudCI6WyJ2YXIgdmlldyA9IHJlcXVpcmUoJy4vdmlldycpLFxuXHR0YWdzID0gcmVxdWlyZSgnLi90YWdzJylcblxuZnVuY3Rpb24gc3dpdGNoX3N0YXRlKHZpZXdfbmFtZSlcbntcblx0dmFyIGJvZHkgPSBkb2N1bWVudC5nZXRFbGVtZW50c0J5VGFnTmFtZSgnYm9keScpWzBdXG5cblx0d2hpbGUoYm9keS5maXJzdENoaWxkKVxuXHRcdGJvZHkucmVtb3ZlQ2hpbGQoYm9keS5maXJzdENoaWxkKVxuXG5cdHRhZ3MuYXBwZW5kKGJvZHksIHZpZXdbdmlld19uYW1lXSgpKVxuXG5cdGlmKGhpc3Rvcnkuc3RhdGUgPT0gbnVsbClcblx0XHRoaXN0b3J5LnJlcGxhY2VTdGF0ZSh2aWV3X25hbWUsICcnKVxuXHRlbHNlXG5cdFx0aGlzdG9yeS5wdXNoU3RhdGUodmlld19uYW1lLCAnJylcbn1cblxud2luZG93Lm9ucG9wc3RhdGUgPSBmdW5jdGlvbigpXG57XG5cdHZhciBzdGF0ZSA9IGhpc3Rvcnkuc3RhdGVcblxuXHRpZihzdGF0ZSA9PSBudWxsKSByZXR1cm5cblx0XHRcblx0c3dpdGNoX3N0YXRlKHN0YXRlKVxufVxuXG5cblxuXG5cblxuXG5cbi8vdmFyIGVtYWlscyA9IEpTT04ucGFyc2UobG9jYWxTdG9yYWdlWydlbWFpbHNfYWxsJ10pXG52YXIgZW1haWxzID0gW11cbnZhciBuYW1lcyA9IFsnU2VhbiBCZWFuJywgJ1RvbnkgU3RhcmsnLCAnVGhlIEluY3JlZGlibGUgSHVsaycsICdBbnRob255IEhvcGtpbnMnXVxuXG5mb3IodmFyIGkgPSAwOyBpIDwgMTAwOyBpKyspIHtcblx0ZW1haWxzLnB1c2goe1xuXHRcdHRvOiBuYW1lc1tNYXRoLmZsb29yKG5hbWVzLmxlbmd0aCAqIE1hdGgucmFuZG9tKCkpXSxcblx0XHRib2R5OiAnTG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdC4nICtcblx0XHQgJ0Z1c2NlIG1vbGxpcywgbWF1cmlzIGV1IHVsbGFtY29ycGVyIHJ1dHJ1bSwgZW5pbSB1cm5hIGFsaXF1YW0gbmlzaSwnICtcblx0XHQgJ2luIHRlbXB1cyBudWxsYSBlcmF0IHV0IGxpYmVyby4gUGVsbGVudGVzcXVlIHRlbXBvciBkb2xvciBuZWMgc2VtIG9ybmFyZScgK1xuXHRcdCAnaW4gdmFyaXVzIHNhcGllbiBncmF2aWRhLiBVdCBncmF2aWRhIGF1Y3RvciBvcmNpIGV0IHB1bHZpbmFyLiBOdWxsYSBmYWNpbGlzaS4nLFxuXHRcdGRhdGU6IG5ldyBEYXRlKCksXG5cdH0pXHRcdCBcbn1cblxubG9jYWxTdG9yYWdlLmVtYWlscyA9IEpTT04uc3RyaW5naWZ5KGVtYWlscylcblxuXG5cblxuXG52YXIgY29sb3JzID0ge1xuXHRkYXJrbWVkaXVtOiAnIzUzM0I1RScsXG5cdGRhcms6ICcjMUUwNzI5Jyxcblx0bWVkaXVtOiAnI0FFNjdDRicsXG5cdGxpZ2h0OiAnI0U4Q0VGNSdcbn1cblxuXG5cbndpbmRvdy5vbmxvYWQgPSBmdW5jdGlvbigpIHtcblxuXHR2YXIgc3R5bGUgPSBkb2N1bWVudC5nZXRFbGVtZW50QnlJZCgnc3R5bGUnKVxuXG5cblx0ZWFjaChjb2xvcnMsIGZ1bmN0aW9uKHYsIGspIHtcblx0XHR2YXIgciA9IG5ldyBSZWdFeHAoaywgJ2cnKVxuXHRcdHN0eWxlLmlubmVySFRNTCA9IHN0eWxlLmlubmVySFRNTC5yZXBsYWNlKHIsIHYpXG5cdH0pXG5cblxuXG5cdC8vc3RhdGUoJ3ZpZXdfYWxsX2VtYWlscycpXG5cdHN3aXRjaF9zdGF0ZSgnZW1haWxzJylcbn1cblxuIiwiXG5lYWNoID0gZnVuY3Rpb24ob2JqLCBmKVxue1xuXHRpZihvYmogaW5zdGFuY2VvZiBBcnJheSlcblx0XHRmb3IodmFyIGkgPSAwOyBpIDwgb2JqLmxlbmd0aDsgaSsrKVxuXHRcdFx0ZihvYmpbaV0sIGksIG9iailcblx0ZWxzZVxuXHRcdGZvcih2YXIgaSBpbiBvYmopXG5cdFx0XHRmKG9ialtpXSwgaSwgb2JqKVxufVxuXG5yYW5nZSA9IGZ1bmN0aW9uKClcbntcblx0dmFyIHIgPSBbXSxcblx0XHRzdGVwID0gMSxcblx0XHRzdGFydCA9IDAsXG5cdFx0ZW5kID0gMFxuXG5cdHN3aXRjaChhcmd1bWVudHMubGVuZ3RoKVxuXHR7XG5cdFx0Y2FzZSAxOlxuXHRcdFx0ZW5kID0gYXJndW1lbnRzWzBdXG5cdFx0XHRicmVha1xuXHRcdGNhc2UgMjpcblx0XHRcdHN0YXJ0ID0gYXJndW1lbnRzWzBdXG5cdFx0XHRlbmQgPSBhcmd1bWVudHNbMV1cblx0XHRcdGJyZWFrXG5cdFx0Y2FzZSAzOlxuXHRcdFx0c3RhcnQgPSBhcmd1bWVudHNbMF1cblx0XHRcdGVuZCA9IGFyZ3VtZW50c1sxXVxuXHRcdFx0c3RlcCA9IGFyZ3VtZW50c1syXVxuXHRcdFx0YnJlYWtcblx0fVxuXHRcblx0Zm9yKHZhciBpID0gc3RhcnQ7IGkgPCBlbmQ7IGkgKz0gc3RlcClcblx0XHRyLnB1c2goaSlcblxuXHRyZXR1cm4gclxufVxuXG5cbmZ1bmN0aW9uIGJpbmQoZWxlbWVudCwgY2hpbGQpXG57XG5cdGlmKGNoaWxkID09IHVuZGVmaW5lZCkgcmV0dXJuXG5cblx0aWYodHlwZW9mIGNoaWxkICE9ICdvYmplY3QnKVxuXHRcdGNoaWxkID0gZG9jdW1lbnQuY3JlYXRlVGV4dE5vZGUoY2hpbGQpXG5cblxuXHRlbGVtZW50LmFwcGVuZENoaWxkKGNoaWxkKVxuXG5cdGlmKGNoaWxkLmdldEF0dHJpYnV0ZSlcblx0e1xuXHRcdHZhciBuYW1lID0gY2hpbGQuZ2V0QXR0cmlidXRlKCduYW1lJylcblxuXHRcdGlmKG5hbWUgIT0gdW5kZWZpbmVkKVxuXHRcdFx0ZWxlbWVudFsnJCcgKyBuYW1lXSA9IGNoaWxkXG5cdH1cbn1cblxuZnVuY3Rpb24gdGFncyh0YWcsIG9wdGlvbnMsIGNoaWxkcmVuKVxue1xuXHR2YXIgZWxlbWVudCA9IHRhZyA9PSAnZnJhZ21lbnQnID9cblx0XHRkb2N1bWVudC5jcmVhdGVEb2N1bWVudEZyYWdtZW50KCkgOlxuXHRcdGRvY3VtZW50LmNyZWF0ZUVsZW1lbnQodGFnKVxuXG5cdGlmKGVsZW1lbnQuc2V0QXR0cmlidXRlKVxuXHRcdGZvcih2YXIgaSBpbiBvcHRpb25zKVxuXHRcdHtcblx0XHRcdGlmKGkgPT0gJ3N0eWxlJylcblx0XHRcdHtcblx0XHRcdFx0Zm9yKHZhciBqIGluIG9wdGlvbnNbaV0pXG5cdFx0XHRcdFx0ZWxlbWVudC5zdHlsZVtqXSA9IG9wdGlvbnNbaV1bal1cblx0XHRcdH1cblx0XHRcdGVsc2Vcblx0XHRcdHtcblx0XHRcdFx0ZWxlbWVudC5zZXRBdHRyaWJ1dGUoaSwgb3B0aW9uc1tpXSlcblx0XHRcdH1cblx0XHR9XG5cblxuXHRlYWNoKGNoaWxkcmVuLCBcblx0XHRmdW5jdGlvbihjKVxuXHRcdHtcblx0XHRcdGJpbmQoZWxlbWVudCwgYylcblx0XHR9KVxuXG5cdHJldHVybiBlbGVtZW50XG59XG5cbnZhciB0ZXh0X3RhZ3MgPSBbJ2h0bWwnLCAnZGl2JywgJ3AnLCAnaW5wdXQnLCAnYScsICd0ZXh0YXJlYScsICdjYW52YXMnLFxuXHQndGQnLCAndHInLCAndGFibGUnLCAnZmllbGRzZXQnLCAnZm9ybScsICdsZWdlbmQnLCAnY2FwdGlvbicsXG5cdCdmcmFnbWVudCcsXHQnc3BhbicsICd1bCcsICdsaScsICdiciddXG5cbmVhY2godGV4dF90YWdzLCBmdW5jdGlvbih0YWcpIFxuXHR7IFxuXHRcdHRhZ3NbdGFnXSA9IGZ1bmN0aW9uKG9wdGlvbnMpIFxuXHRcdHsgXG5cdFx0XHR2YXIgY2hpbGRyZW4gPSBBcnJheS5wcm90b3R5cGUuc2xpY2UuY2FsbChhcmd1bWVudHMpXG5cblx0XHRcdGlmKHR5cGVvZiBvcHRpb25zICE9IHVuZGVmaW5lZClcblx0XHRcdFx0Y2hpbGRyZW4gPSBjaGlsZHJlbi5zbGljZSgxKVxuXG5cdFx0XHRyZXR1cm4gdGFncyh0YWcsIG9wdGlvbnMsIGNoaWxkcmVuKSBcblx0XHR9IFxuXHR9KVxuXG5cbnRhZ3MuYXBwZW5kID0gZnVuY3Rpb24ocGFyZW50KVxue1xuXHR2YXIgY2hpbGRyZW4gPSBBcnJheS5wcm90b3R5cGUuc2xpY2UuY2FsbChhcmd1bWVudHMsIDEpXG5cblx0ZWFjaChjaGlsZHJlbiwgZnVuY3Rpb24oYylcblx0XHR7XG5cdFx0XHRiaW5kKHBhcmVudCwgYylcblx0XHR9KVxuXG5cdHJldHVybiBwYXJlbnRcbn1cblxubW9kdWxlLmV4cG9ydHMgPSB0YWdzXG4iLCJtb2R1bGUuZXhwb3J0cyA9IHtcblx0J2VtYWlscyc6IHJlcXVpcmUoJy4vZW1haWxzLmpzJyksXG5cdCdrZXktaWNvbic6IHJlcXVpcmUoJy4va2V5LWljb24uanMnKVxufVxuXG4iLCJ2YXIgdGFncyA9IHJlcXVpcmUoJy4uL3RhZ3MnKSxcblx0a2V5X2ljb24gPSByZXF1aXJlKCcuL2tleS1pY29uLmpzJylcblxubW9kdWxlLmV4cG9ydHMgPSBmdW5jdGlvbigpXG57XG5cblx0dmFyIGxpc3QgPSBKU09OLnBhcnNlKGxvY2FsU3RvcmFnZVsnZW1haWxzJ10pXG5cdHZhciB2aWV3ID0gdGFncy50YWJsZSh7fSlcblxuXHR2YXIgcGFnZSA9IHRhZ3MuZnJhZ21lbnQoe30sIFxuXHRcdGtleV9pY29uKCksXG5cdFx0dmlldylcblxuXHRlYWNoKGxpc3QsIGZ1bmN0aW9uKGVtYWlsKVxuXHRcdHtcblx0XHRcdGVtYWlsLmRhdGUgPSBuZXcgRGF0ZShlbWFpbC5kYXRlKVxuXG5cdFx0XHR2YXIgZGlyLCBwZWVyO1xuXHRcdFx0aWYoZW1haWwuZnJvbSAhPSB1bmRlZmluZWQpIHtcblxuXHRcdFx0XHRkaXIgPSAnZnJvbSAnXG5cdFx0XHRcdHBlZXIgPSBlbWFpbC5mcm9tXG5cdFx0XHR9XG5cdFx0XHRlbHNlIHtcblx0XHRcdFx0ZGlyID0gJ3RvJ1xuXHRcdFx0XHRwZWVyID0gZW1haWwudG9cblx0XHRcdH1cblxuXHRcdFx0dmFyIGF0dGFjaG1lbnQgPSB1bmRlZmluZWQ7XG5cdFx0XHRcblx0XHRcdGlmKE1hdGgucmFuZG9tKCkgPCAwLjIpXG5cdFx0XHRcdGF0dGFjaG1lbnQgPSB0YWdzLmRpdih7IFxuXHRcdFx0XHRcdHRpdGxlOiAnVGhpcyBtZXNzYWdlIGhhcyBhdHRhY2htZW50cycsXG5cdFx0XHRcdFx0Y2xhc3M6ICdpY29uLXBhcGVyLWNsaXAgaWNvbi1sYXJnZScsIFxuXHRcdFx0XHRcdHN0eWxlOiB7IGRpc3BsYXk6ICdpbmxpbmUtYmxvY2snLCBwYWRkaW5nOiAnNXB4JyB9XG5cdFx0XHRcdH0pXG5cblx0XHRcdHRhZ3MuYXBwZW5kKHZpZXcsIHRhZ3MudHIoeyBzdHlsZTogeyBoZWlnaHQ6ICcxMzBweCcsIH19LFxuXHRcdFx0XHR0YWdzLnRkKHsgc3R5bGU6IHsgd2lkdGg6ICcxNTBweCcgfSB9LCBcblx0XHRcdFx0XHR0YWdzLmRpdih7IHN0eWxlOiB7IGZvbnRTaXplOiAnMC44ZW0nIH0gfSwgZGlyKSxcblx0XHRcdFx0XHR0YWdzLmRpdih7IHN0eWxlOiB7IGZvbnRTaXplOiAnMS41ZW0nIH0gfSwgcGVlciksXG5cdFx0XHRcdFx0dGFncy5kaXYoeyBzdHlsZTogeyBmb250U2l6ZTogJzAuNWVtJyB9LCBcblx0XHRcdFx0XHRcdHRpdGxlOiBlbWFpbC5kYXRlLnRvU3RyaW5nKCkgfSwgZW1haWwuZGF0ZS50b0RhdGVTdHJpbmcoKSksXG5cdFx0XHRcdFx0YXR0YWNobWVudCksXG5cdFx0XHRcdHRhZ3MudGQoeyBzdHlsZTogeyBjb2xvcjogJyM4Mjc4ODcnIH0gfSwgZW1haWwuYm9keSkpKVxuXHRcdH0pXG5cblx0cmV0dXJuIHBhZ2Vcbn1cblxuXG4iLCJ2YXIgdGFncyA9IHJlcXVpcmUoJy4uL3RhZ3MnKVxuXG5tb2R1bGUuZXhwb3J0cyA9IGZ1bmN0aW9uKCkgXG57XG5cdHZhciBpY29uID0gdGFncy5kaXYoeyBjbGFzczogJ2tleSBpY29uLWtleSBpY29uLTJ4JyB9KVxuXG5cdGljb24ub25jbGljayA9IGZ1bmN0aW9uKClcblx0e1xuXHRcdHN0YXRlKCd2aWV3X2tleXMnKVxuXHR9XG5cblx0cmV0dXJuIGljb25cblxufVxuIl19
;