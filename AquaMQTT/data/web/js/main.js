const URI = '/api/';

async function getData(endpoint) {
    return (await fetch(URI + endpoint)).json();
}


async function processElement(x) {
    for (var key in x) {
        var el = $(`#${key}`);
        switch (el.attr('type')) {
            case 'text':
            case 'select':
            case 'password':
            case 'number':
                el.val(x[key]);
                break;
            case 'checkbox':
                x[key] == 1 ? el.prop('checked', true) : el.prop('checked', false);
                break;
            default:
                break;
        }
    }
}
// Load data from eache API endpoint
(async function () {
    await processElement(await getData('wifi'));
    await processElement(await getData('mqtt'));
    await processElement(await getData('aqua'));
})();



async function sendData(reboot) {
	{
		// endpoind based on visible form id
		let endpoint = $('form:visible').attr('id');
		const data = {}
		$("form:visible .form-control").each(function (idx) {
			data[$(this).attr('id')] = $(this).val();
		});
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
}

$(async function () {
	$('#save').on('click', function () { sendData(false) })

})

$(async function () {
	$('#savereboot').on('click', function () { sendData(true) })
})


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
