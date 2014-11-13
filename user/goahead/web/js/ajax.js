/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function createXMLHttp()
{
	var xmlHttp;

	try
	{	
		xmlHttp = new XMLHttpRequest();// Firefox, Opera 8.0+, Safari
	}
	catch (e)
	{
		try
		{
			xmlHttp = new ActiveXObject("Msxml2.XMLHTTP"); // Internet Explorer
		}
		catch (e)
		{
			try
			{
				xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch (e)
			{
				alert("No AJAX support!");
				return null;
			}
		}
	}
	
	return xmlHttp;
}

function genRandomParam(uri)
{
	if (uri == null)
		uri = '';
	if (navigator.userAgent.indexOf('MSIE') < 0)
		return uri;

	var d = new Date();
	var res = (uri.indexOf('?') < 0) ? uri + '?' : uri + '&';

	return res + "msieSucks=" + d.getTime() + "." + Math.round(Math.random()*100000000.0);
}

function ajaxPerformRequest(uri, handler)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;
	
	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (handler != null)
			{
				handler(xmlHttp.responseText);
				handler = null;
			}
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("GET", genRandomParam(uri), true);
	xmlHttp.send(null);
}

function ajaxPostRequest(uri, content, refresh, handler)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;
	
	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			// Free resources
			if (handler != null)
			{
				handler(xmlHttp.responseText);
				handler = null;
			}
			
			if (refresh)
				window.location.reload();
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("POST", genRandomParam(uri), true);
	xmlHttp.send(content);
}

function ajaxLoadElement(elementID, url, onLoadAction)
{
	var element = document.getElementById(elementID);
	if (element == null)
		return;

	// Create XMLHttpRequest
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			var text = xmlHttp.responseText;
			
			if (xmlHttp.status == 200)
			{
				element.innerHTML = '';
				element.innerHTML = text;
			}
			else
				element.innerHTML = '<b>' + xmlHttp.statusText + '</b>';
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;

			if (onLoadAction != null)
				onLoadAction(element);
		}
	}
	
	xmlHttp.open("GET", genRandomParam(url), true);
	xmlHttp.send();
}

function ajaxLoadScript(scriptFile)
{
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (xmlHttp.status == 200)
			{
				try
				{
					eval(xmlHttp.responseText);
				}
				catch (e)
				{
					alert("Error evaluating " + scriptFile + ": " + e);
				}
			}
			
			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}
	
	xmlHttp.open("GET", genRandomParam(scriptFile), true);
	xmlHttp.send(null);
}

function ajaxPopupWindow(popupID, url, onLoadAction)
{
	// Create XMLHttpRequest
	var xmlHttp = createXMLHttp();
	if (xmlHttp == null)
		return;

	var doc = document; //(parent != null) ? parent.document : document;

	// Produce window popup
	var background = document.createElement('div');
	background.id = popupID + "Background";
	background.className = 'popup_window';
	background.style.display = 'none';
	document.body.appendChild(background);

	var popup = doc.createElement('div');
	popup.id = popupID + "Border";
	popup.className = 'popup_window_border';
	popup.style.display = 'none';
	popup.windowLoaded = false;
	doc.body.appendChild(popup);

	// Perform HTTP request
	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (xmlHttp.status == 200)
			{
				// Store innerHTML
				popup.innerHTML = xmlHttp.responseText;
				
				// Scroll
				try
				{
					document.body.scrollTo(0, 0);
				}
				catch (ex)
				{
				}
				
				try
				{
					window.scrollTo(0, 0);
				}
				catch (ex)
				{
				}
				
				// Show background
				var b_height = (document.body.innerHeight != null) ? document.body.innerHeight : document.body.clientHeight;
				var c_width =
					(window.innerWidth != null) ? window.innerWidth :
					(document.body.innerWidth != null) ? document.body.innerWidth :
					(window.clientWidth != null) ? window.clientWidth : document.body.clientWidth;
				var c_height =
					(window.innerHeight != null) ? window.innerHeight :
					(document.body.innerHeight != null) ? document.body.innerHeight :
					(window.clientHeight != null) ? window.clientHeight : document.body.clientHeight;

				background.style.height = Math.max(b_height, c_height) + 'px';
				background.style.display = '';

				// Show popup
				popup.style.display = '';

				var d_width = popup.offsetWidth;
				var d_height = popup.offsetHeight;
				var x = Math.round((c_width - d_width)/2.0);
				var y = Math.round((c_height - d_height)/2.0);
				if (x < 0)
					x = 0;
				if (y < 0)
					y = 0;

				popup.style.left = x + 'px';
				popup.style.top  = y + 'px';
				popup.windowLoaded = true;
				
				if (onLoadAction != null)
					onLoadAction();
			}
			else
			{
				alert(xmlHttp.statusText);
				ajaxCloseWindow(popupID);
			}

			// Free resources
			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
			popupID = null;
			popup   = null;
		}
	};

	xmlHttp.open("GET", genRandomParam(url), true);
	xmlHttp.send(null);
}

function ajaxCloseWindow(popupID)
{
	var doc = document; //(parent != null) ? parent.document : document;

	var popup = doc.getElementById(popupID + "Border");
	if (popup!=null)
		doc.body.removeChild(popup);

	popup = doc.getElementById(popupID + "Background");
	if (popup!=null)
		doc.body.removeChild(popup);

	doc.body.onscroll = doc.body.oldOnScroll;
}

function ajaxPostForm(question, form, reloader, message, handler)
{
	if (question != null)
	{
		if (!confirm(question))
			return false;
	}

	if (parent!=null)
	{
		var obj = parent.document.getElementById("homeFrameset");
		if (obj != null)
			obj.rows = "0,1*"; // Hide top logo
		var obj = parent.document.getElementById("homeMenuFrameset");
		if (obj != null)
			obj.cols = "0,1*"; // Hide menu
	}
	
	var submitForm = function()
	{
		if (handler != null)
			handler();
		form.submit();
	};
	
	if (reloader != null)
		form.target = reloader;
	ajaxPopupWindow('ajxLoadParams', message, submitForm);
	
	return true;
}

function ajaxModifyElementHTML(elementID, value)
{
	var elem = document.getElementById(elementID);
	if (elem != null)
		elem.innerHTML = value;
}

function ajaxSearchElementById(element_id)
{
	var pw = window;
	while (true)
	{
		var elem = window.document.getElementById(element_id);
		if (elem != null)
			return elem;

		for (var i=0; i<pw.frames.length; i++)
		{
			elem = pw.frames[i].document.getElementById(element_id);
			if (elem != null)
				return elem;
		}

		if ((pw.parent != null) && (pw.parent != pw))
			pw = pw.parent;
		else
			return null;
	}
}

function ajaxGetRootWindow()
{
	var pw = window;
	while ((pw.parent != null) && (pw.parent != pw))
		pw = pw.parent;
	return pw;
}

function ajaxShowProgress()
{
	var elem = ajaxSearchElementById("ajxCounterIndicator");
	var pw = ajaxGetRootWindow();
	if (elem == null)
		return;
	
	var progress = [ '/', '-', '\\', '|' ];
	var index = 0;

	var indicator = function()
	{
		if (pw.currentProgressHandler != indicator)
			return;

		elem.innerHTML = progress[index];
		index = (index + 1) % progress.length;
		pw.setTimeout(indicator, 500);
	}
	
	pw.currentProgressHandler = indicator;
	
	indicator();
}

function ajaxReloadDelayedPage(delay, url, local)
{
	var elem = ajaxSearchElementById("ajxCounterIndicator");
	var pw = (local) ? window : ajaxGetRootWindow();

	delay /= 1000;
	
	var reloader = function()
	{
		if (pw.currentProgressHandler == reloader)
		{
			if (elem != null)
				elem.innerHTML = delay;
		}

		if (delay > 0) // check counter
		{
			delay--;
			pw.setTimeout(reloader, 1000);
		}
		else // Reload page
		{

			// Load/reload page
			if (url == null)
				pw.location.href = pw.location.href;
			else
				pw.location.href = url;
		}
	}
	
	pw.currentProgressHandler = reloader;
	
	reloader();
}

