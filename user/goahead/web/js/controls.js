/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function showElement(elementID, display)
{
	if (elementID == null)
		return;
	var item = (typeof(elementID)=='string') ? 
		document.getElementById(elementID) : elementID;
	if (item == null)
		return;
	if (display == null)
		display = '';
	
	if (item instanceof Array)
	{
		for (var i=0; i<item.length; i++)
			showElement(item[i], display);
	}
	else
	{
		if (item.style.display != null)
			item.style.display = display;
		if (item.style.visibility)
			item.style.visibility = "visible";
	}
}

function showElementEx(elementID, display)
{
	return showElement(elementID, display);
}

function hideElement(elementID, display)
{
	if (elementID == null)
		return;
	var item = (typeof(elementID)=='string') ? 
		document.getElementById(elementID) : elementID;
	if (item == null)
		return;
	if (display == null)
		display = 'none';
	
	if (item instanceof Array)
	{
		for (var i=0; i<item.length; i++)
			hideElement(item[i], display);
	}
	else
	{
		if (item.style.display != null)
			item.style.display = display;
		if (item.style.visibility)
			item.style.visibility = "hidden";
	}
}

function displayElement(elementID, visible)
{
	if (visible)
		showElement(elementID);
	else
		hideElement(elementID);
}

function disableElement(element, disabled)
{
	if (typeof(element) == 'string')
	{
		element = document.getElementById(element);
		if (element != null)
			element.disabled = disabled;
	}
	else if (element instanceof Array)
	{
		for (var i=0; i<element.length; i++)
			disableElement(element[i], disabled);
	}
	else if (typeof(element) == 'object')
		element.disabled = disabled;
}

function addOption(list, text, value)
{
	var option = new Option(text, value);

	try
	{
		list.add(option, null); // standards compliant
	}
	catch(ex)
	{
		list.add(option); // IE only
	}
}

self.getElementValue = function(element)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.value;
}

self.setElementValue = function(element, value)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.value = value;
}

self.getElementChecked = function(element)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.checked;
}

self.setElementChecked = function(element, checked)
{
	if (typeof(element) == 'string')
		element = document.getElementById(element);
	if (element == null)
		return null;
	return element.checked = checked;
}

self.enableElements = function(elements, enable)
{
	if (enable == null)
		enable = true;
	if (elements == null)
		return;
	var item = (typeof(elements)=='string') ? 
		document.getElementById(elements) : elements;
	if (item == null)
		return;
	
	if (item instanceof Array)
	{
		for (var i=0; i<item.length; i++)
			enableElements(item[i], enable);
	}
	else
		elements.disabled = (enable) ? false : true;
}

self.setInnerHTML = function(elementID, text)
{
	var element = document.getElementById(elementID);
	if (element == null)
		return;

	// Set inner HTML code
	element.innerHTML = text;
}

self.createNode = function(cl, attributes, innerHTML)
{
	var element = document.createElement(cl);
	if (attributes != null)
	{
		for (var key in attributes)
		{
			if (key)
				element.setAttribute(key, attributes[key]);
		}
	}
	if (innerHTML != null)
		element.innerHTML = innerHTML;
	return element;
}


