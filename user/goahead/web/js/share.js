function disableTextField (field)
{
	if (document.all || document.getElementById)
		field.disabled = true;
	else
	{
		field.oldOnFocus = field.onfocus;
		field.onfocus = skip;
	}
}

function enableTextField(field)
{
	if (document.all || document.getElementById)
		field.disabled = false;
	else
		field.onfocus = field.oldOnFocus;
}

function style_display_on()
{
	if (window.ActiveXObject) // IE
	{
		return "block";
	}
	else if (window.XMLHttpRequest) // Mozilla, Safari...
	{
		return "table-row";
	}
}
