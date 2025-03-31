const URI = '/api/';

async function getData(endpoint) {
	return (await fetch(URI + endpoint)).json();
}


async function processInputElement(x) {
	for (var key in x) {
		var el = document.getElementById(`${key}`)
		switch (el.getAttribute('type')) {
			case 'text':
			case 'select':
			case 'password':
			case 'number':
				el.value = x[key];
				break;
			case 'checkbox':
				x[key] == 1 ? el.checked = true : el.checked = false;
				break;
			default:
				break;
		}
	}
}
// Load data from eache API endpoint
(async function () {
	await processInputElement(await getData('wifi'));
	await processInputElement(await getData('mqtt'));
	await processInputElement(await getData('aqua'));
})();

function processOutputElement(formId) {
	const outData = {}
	const elements = document.querySelector(`form#${formId}`).elements;
	Array.from(elements).forEach((el) => {
		switch (el.getAttribute('type')) {
			case 'text':
			case 'select':
			case 'password':
				outData[el.id] = el.value;
				break;
			case 'number':
				outData[el.id] = parseInt(el.value);
				break;
			case 'checkbox':
				outData[el.id] = el.checked ? 1 : 0
				break;
			default:
				break;
		}
	})
	return outData;
}

async function sendData(endpoint, reboot) {

	let data = {}
	data = processOutputElement(endpoint);

	const resp = await fetch(
		`/api/${endpoint}`,
		{
			method: "POST",
			headers: {
				"Content-Type": "application/json",
				"Accept": "application/json",
			},
			body: JSON.stringify({ 'reboot': reboot, data })
		})
	if (resp.ok) {
		appendAlert('Save OK', 'success')
	} else {
		appendAlert(`Error while saving data.Please retry`, 'danger')
	}
}

function endpoint() {
	let endpoints = document.querySelectorAll('form')
	let endpoint = "";
	endpoints.forEach((form) => {
		//assume only one form visible at one moment
		if (form.checkVisibility()) { endpoint = form.id }
	})
	return endpoint;
}

document.getElementById("save").onclick = function () { sendData(endpoint(), false) };
document.getElementById("savereboot").onclick = function () { sendData(endpoint(), true) };


const alertPlaceholder = document.getElementById('liveAlertPlaceholder')
const appendAlert = (message, type) => {
	const wrapper = document.createElement('div')
	wrapper.innerHTML = [
		`<div class="alert alert-${type} alert-dismissible" role="alert">`,
		`   <div>${message}</div>`,
		'   <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>',
		'</div>'
	].join('')

	alertPlaceholder.append(wrapper)
}
