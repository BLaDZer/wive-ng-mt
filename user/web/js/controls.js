/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function formatBytes(bytes,decimals) {
   if(bytes == 0) return '0 B';
   var k = 1024,
       dm = decimals <= 0 ? 0 : decimals || 2,
       sizes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'],
       i = Math.floor(Math.log(bytes) / Math.log(k));
   return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];
}

// checks if value is number in range [minval, maxval]
function inRange(value, minval, maxval) {
    if (!isFinite(value)) return false;
    if (minval != null && value < minval) return false;
    if (maxval != null && value > maxval) return false;

    return true;
}


if (!Array.prototype.indexOf) {
    Array.prototype.indexOf = function(obj, start) {
         for (var i = (start || 0), j = this.length; i < j; i++) {
             if (this[i] === obj) { return i; }
         }
         return -1;
    }
}

if (!Array.prototype.filter)
{
  Array.prototype.filter = function(fun /*, thisp */)
  {
    "use strict";

    if (this == null)
      throw new TypeError();

    var t = Object(this);
    var len = t.length >>> 0;
    if (typeof fun != "function")
      throw new TypeError();

    var res = [];
    var thisp = arguments[1];
    for (var i = 0; i < len; i++)
    {
      if (i in t)
      {
        var val = t[i]; // in case fun mutates this
        if (fun.call(thisp, val, i, t))
          res.push(val);
      }
    }

    return res;
  };
}

if (document.getElementsByClassName === undefined)
{
    document.getElementsByClassName = function(className) {
        var found = [];
        var elements = document.getElementsByTagName("*");

        for (var i = 0; i < elements.length; i++)
        {
            var names = elements[i].className.split(' ');
            for (var j = 0; j < names.length; j++)
            {
                if (names[j] == className) found.push(elements[i]);
            }
        }

        return found;
    }
}

document.getElementsByAttribute = function(attribute)
{
    if (document.querySelectorAll !== undefined) return document.querySelectorAll('['+attribute+']');

    var elems = [];
    var allElements = document.getElementsByTagName('*');

    for (var i = 0, n = allElements.length; i < n; i++)
    {
        if (allElements[i].getAttribute(attribute) !== null)
        {
            elems.push(allElements[i]);
        }
    }

    return elems;
}

function disableControlsByAuth()
{
    if (AUTH_ROLE != 2)
    {
        elems = document.getElementsByClassName('auth-disable-user');

        for (i=0;i<elems.length;i++) {
            elems[i].setAttribute("disabled", true) 
        }

        elems = document.getElementsByClassName('auth-hide-user');

        for (i=0;i<elems.length;i++) {
            elems[i].style.display = "none";
        }

        elems = document.getElementsByClassName('auth-readonly-user');

        for (i=0;i<elems.length;i++) {
            elems[i].setAttribute("readonly", true) 
        }
    }
}

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

        if (item.tagName == "OPTION")
        {
            display = "block";
        }
	
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

function setCookie(name, value, options) {
	options = options || {};
	var expires = options.expires;
	if (typeof expires == "number" && expires) {
		var d = new Date();
		d.setTime(d.getTime() + expires * 1000);
		expires = options.expires = d;
	}
	if (expires && expires.toUTCString) {
		options.expires = expires.toUTCString();
	}
	value = encodeURIComponent(value);
	var updatedCookie = name + "=" + value;
	for (var propName in options) {
		updatedCookie += "; " + propName;
		var propValue = options[propName];
		if (propValue !== true) {
			updatedCookie += "=" + propValue;
		}
	}
	document.cookie = updatedCookie;
}

function getCookie(name) {
	var matches = document.cookie.match(new RegExp("(?:^|; )" + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + "=([^;]*)"));
	return matches ? decodeURIComponent(matches[1]) : undefined;
}

function deleteCookie(name) {
	setCookie(name, "", { expires: -1 });
}

function showMenu(elem, show){
	if (typeof elem == "string" && document.getElementById(elem) !== undefined) elem = document.getElementById(elem);
	if (elem.tagName == "THEAD" || elem.tagName == "TBODY") elem = elem.parentNode;
	if (elem.tagName != "TABLE") return false;

	var table_head = elem.tHead;
	var table_body = elem.tBodies[0];

	if (table_head === undefined || table_body === undefined) return false;

	if (show === undefined || show != 0)
	{
		if (table_body.style.removeProperty === undefined)
		{
			table_body.style.removeAttribute('display'); // IE<9
		}
		else
		{
			table_body.style.removeProperty('display');
		}
		elem.setAttribute('minimized', '0');
	}
	else
	{
		table_body.style.display = "none";
		elem.setAttribute('minimized', '1');
	}

	return true;
}

function switchShowMenu(elem){
	if (typeof elem == "string" && document.getElementById(elem) !== undefined) elem = document.getElementById(elem);
	if (elem.tagName == "THEAD" || elem.tagName == "TBODY") elem = elem.parentNode;
	if (elem.tagName != "TABLE") return false;

	var table_head = elem.tHead;
	var table_body = elem.tBodies[0];

	if (table_head === undefined || table_body === undefined) return false;

	if (table_body.style.display == "none")
	{
		showMenu(elem, 1);
	}
	else
	{
		showMenu(elem, 0);
	}

	return true;
}

function hideAllMenus() {
	var menus = document.getElementsByClassName('showHideMenu');
	for (var i=0;i<menus.length;i++) {
		showMenu(menus[i],0);
	}
}
