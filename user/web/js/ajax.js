/*******************************************
 Copyright (C) Vladimir Sadovnikov
 This is free software distributed under
 terms of GNU Public License v2.

 If you have ideas or recommendations:
 Visit my blog: http://sadko.xskernel.org/
 Or contact me: sadko<at>xskernel.org

*******************************************/

function createXMLHttp() {
	var result = false;
	var actions = [
		function() {return new XMLHttpRequest()},
		function() {return new ActiveXObject('Msxml2.XMLHTTP')},
		function() {return new ActiveXObject('Microsoft.XMLHTTP')}
	];
	for(var i = 0; i < actions.length; i++) {
		try {
			result = actions[i]();
			break;
		}
		catch (e) {}
	}
	return result;
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
	if (!xmlHttp)
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
	if (!xmlHttp)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			// Free resources
			if (handler != null)
			{
				if (xmlHttp.status == 200) {
					handler(xmlHttp.responseText);
					handler = null;
					xmlHttp.onreadystatechange = null;
					xmlHttp = null;
				}
			}

			if (refresh)
				window.location.reload();

			// Free resources
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
	if (!xmlHttp)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			var text = xmlHttp.responseText;

			if (xmlHttp.status == 200)
				ajaxModifyElementHTML(elementID, text);
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
	if (!xmlHttp)
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

function ajaxPopupWindow(popupID, message, onLoadAction) {
	var doc = document;

	// Produce window popup
	var background = doc.createElement('div');
	background.id = popupID + "Background";
	background.className = 'popup_window';
	background.style.display = 'none';
	doc.body.appendChild(background);

	var message_div = doc.createElement('div');
	message_div.className = 'message';
	message_div.innerHTML = message;

	var count = doc.createElement('div');
	count.id = 'ajxCounterIndicator';
	count.className = 'message red';

	var border = doc.createElement('div');
	border.className = 'popup_message';
	border.appendChild(message_div);
	border.appendChild(count);

	var popup = doc.createElement('div');
	popup.appendChild(border);
	popup.id = popupID + "Border";
	popup.className = 'popup_window_border';
	popup.style.display = 'none';
	popup.windowLoaded = false;
	doc.body.appendChild(popup);

	// Show background
	var b_height =	(document.body.innerHeight != null) ? document.body.innerHeight : document.body.clientHeight;
	var c_width  =	(window.innerWidth != null) ? window.innerWidth :
					(document.body.innerWidth != null) ? document.body.innerWidth :
					(window.clientWidth != null) ? window.clientWidth : document.body.clientWidth;
	var c_height =	(window.innerHeight != null) ? window.innerHeight :
					(document.body.innerHeight != null) ? document.body.innerHeight :
					(window.clientHeight != null) ? window.clientHeight : document.body.clientHeight;
	background.style.height = Math.max(b_height, c_height) + 'px';
	background.style.display = '';

	// Show popup
	popup.style.display = '';

	var d_width = popup.offsetWidth;
	var d_height = popup.offsetHeight;
	var x = Math.round((c_width - d_width - top.frames["menu"].document.documentElement.clientWidth)/2.0);
	var y = Math.round((c_height - d_height - top.frames["title"].document.documentElement.clientHeight)/2.0);
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;

	popup.style.left = x + 'px';
	popup.style.top  = y + 'px';
	popup.windowLoaded = true;
				
	if (onLoadAction != null)
		onLoadAction();

	popupID = null;
	popup   = null;
}

function ajaxCloseWindow(popupID)
{
	var popup = document.getElementById(popupID + "Border");
	if (popup!=null)
		document.body.removeChild(popup);

	popup = document.getElementById(popupID + "Background");
	if (popup!=null)
		document.body.removeChild(popup);

	var browser = getBrowser();
	if (!(browser.browser == 'ie' && browser.versionShort <= 8))
		document.body.onscroll = document.body.oldOnScroll;
}

function ajaxPostForm(question, form, reloader, message, handler)
{
	if (question != null) {
		if (!confirm(question))
			return false;
	}

	if (parent!=null) {
		// Set shadow on title & menu
		var frame_title				= top.frames["title"].document
		var background_title			= frame_title.createElement('div');
		var frame_menu				= top.frames["menu"].document;
		var background_menu			= frame_menu.createElement('div');
		background_title.id			= "reload_background";
		background_title.style.backgroundColor	= '#c0c0c0';
		background_title.style.position		= 'absolute';
		background_title.style.opacity		= '0.5';
		background_title.style.left		= '0px';
		background_title.style.top		= '0px';
		background_title.style.height		= frame_title.documentElement.clientHeight + 'px';
		background_title.style.width		= frame_title.documentElement.clientWidth + 'px';
		background_menu.id			= "reload_background";
		background_menu.style.backgroundColor	= '#c0c0c0';
		background_menu.style.position		= 'absolute';
		background_menu.style.opacity		= '0.5';
		background_menu.style.left		= '0px';
		background_menu.style.top		= '0px';
		background_menu.style.height		= frame_menu.documentElement.clientHeight+ 'px';
		background_menu.style.width		= frame_menu.documentElement.clientWidth + 'px';

		frame_title.body.appendChild(background_title);
		frame_menu.body.appendChild(background_menu);
	}
	window.scrollTo(0, 0);
	
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

function ajaxShowTimer(form, reloader, message, delay, href)
{
	// Set shadow on title & menu
	var frame_title				= top.frames["title"].document
	var background_title			= frame_title.createElement('div');
	var frame_menu				= top.frames["menu"].document;
	var background_menu			= frame_menu.createElement('div');
	background_title.id			= "reload_background";
	background_title.style.backgroundColor	= '#c0c0c0';
	background_title.style.position		= 'absolute';
	background_title.style.opacity		= '0.5';
	background_title.style.left		= '0px';
	background_title.style.top		= '0px';
	background_title.style.height		= frame_title.documentElement.clientHeight + 'px';
	background_title.style.width		= frame_title.documentElement.clientWidth + 'px';
	background_menu.id			= "reload_background";
	background_menu.style.backgroundColor	= '#c0c0c0';
	background_menu.style.position		= 'absolute';
	background_menu.style.opacity		= '0.5';
	background_menu.style.left		= '0px';
	background_menu.style.top		= '0px';
	background_menu.style.height		= frame_menu.documentElement.clientHeight+ 'px';
	background_menu.style.width		= frame_menu.documentElement.clientWidth + 'px';

	frame_title.body.appendChild(background_title);
	frame_menu.body.appendChild(background_menu);

	window.scrollTo(0, 0);
	var submitForm = function() {
		form.submit();
	};

	if (reloader != null)
		form.target = reloader;

	ajaxPopupWindow('ajxLoadParams', message, submitForm, ajaxShowProgress);

	if (delay == null)
		delay = 15;

	var elem = ajaxSearchElementById("ajxCounterIndicator");
	var pw = ajaxGetRootWindow();

	var reloader = function() {
		if (pw.currentProgressHandler == reloader) {
			if (elem != null)
				elem.innerHTML = delay;
		}
		if (delay > 0) { // check counter
			delay--;
			pw.setTimeout(reloader, 1000);
		}
		else {	// Reload page
			ajaxCloseWindow('ajxLoadParams');

			// Remove shadow on title & menu
			if (frame_title.getElementById('reload_background'))
				frame_title.body.removeChild(frame_title.getElementById('reload_background'));
			if (frame_menu.getElementById('reload_background'))
				frame_menu.body.removeChild(frame_menu.getElementById('reload_background'));

			if (href === undefined)
				window.location.href = window.location.href;
			else
				pw.location.href = href;
		}
	}
	pw.currentProgressHandler = reloader;
	reloader();
	clearInterval(displayServiceStatus_interval);
}

function getBrowser() {
    var ua = navigator.userAgent;
    var bName = function () {
        if (ua.search(/Edge/) > -1) return "edge";
        if (ua.search(/MSIE/) > -1) return "ie";
        if (ua.search(/Trident/) > -1) return "ie11";
        if (ua.search(/Firefox/) > -1) return "firefox";
        if (ua.search(/Opera/) > -1) return "opera";
        if (ua.search(/OPR/) > -1) return "operaWebkit";
        if (ua.search(/YaBrowser/) > -1) return "yabrowser";
        if (ua.search(/Chrome/) > -1) return "chrome";
        if (ua.search(/Safari/) > -1) return "safari";
        if (ua.search(/Maxthon/) > -1) return "maxthon";
    }();

    var version;
    switch (bName) {
        case "edge":
            version = (ua.split("Edge")[1]).split("/")[1];
            break;
        case "ie":
            version = (ua.split("MSIE ")[1]).split(";")[0];
            break;
        case "ie11":
            bName = "ie";
            version = (ua.split("; rv:")[1]).split(")")[0];
            break;
        case "firefox":
            version = ua.split("Firefox/")[1];
            break;
        case "opera":
            version = ua.split("Version/")[1];
            break;
        case "operaWebkit":
            bName = "opera";
            version = ua.split("OPR/")[1];
            break;
        case "yabrowser":
            version = (ua.split("YaBrowser/")[1]).split(" ")[0];
            break;
        case "chrome":
            version = (ua.split("Chrome/")[1]).split(" ")[0];
            break;
        case "safari":
            version = (ua.split("Version/")[1]).split(" ")[0];
            break;
        case "maxthon":
            version = ua.split("Maxthon/")[1];
            break;
    }

    var platform = 'desktop';
    if (/iphone|ipad|ipod|android|blackberry|mini|windows\sce|palm/i.test(navigator.userAgent.toLowerCase())) platform = 'mobile';

    var browsrObj;

    try {
        browsrObj = {
            platform: platform,
            browser: bName,
            versionFull: version,
            versionShort: version.split(".")[0]
        };
    } catch (err) {
        browsrObj = {
            platform: platform,
            browser: 'unknown',
            versionFull: 'unknown',
            versionShort: 'unknown'
        };
    }
 
    return browsrObj;
}

function enableCWMPAutoClick() {
	if (confirm(_("warning cwmpd auto alert"))) {
		ajaxPostRequest('/goform/cwmpEnableAuto', '', true, null);
	}
}

function showWarning() {

	getRunningServices(function(services) {

		var warning_access_password		= NVRAM_Password == "Admin";
		var warning_wireless_security		= NVRAM_AuthMode == "OPEN";
		var warning_wireless_key		= NVRAM_WPAPSK1 == "1234567890";
		var warning_cwmpd			= services.indexOf("cwmpd") != -1;
		var warning_cwmpd_auto_available	= BUILD_CWMP == '1' && ('<% getCWMPAutoAvailable(); %>' == '1') && NVRAM_cwmpdEnabled == '0';
		var warningHTML				= '';

		if (warning_access_password || warning_wireless_security || warning_wireless_key || warning_cwmpd || warning_cwmpd_auto_available) {
			warningHTML += '<br>';
			warningHTML += '<table class="warning">';
			warningHTML += '<tr><th class="warning" align="center" colspan="2">' + _("warning header") + '</th></tr>';
			if  (warning_access_password) {
				warningHTML += '<tr>';
				warningHTML += '<td class="warning">' + _("warning access password") + '</td>';
				warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button warning") + '" onClick=\'window.location.assign("/adm/management.asp");\'></td>';
				warningHTML += '</tr>';
			}
			if (warning_access_password && (warning_wireless_security || warning_wireless_key)) {
				warningHTML += '<tr><td colspan="2"><hr class="warning"></td></tr>';
			}
			if  (warning_wireless_security || warning_wireless_key) {
				warningHTML += '<tr>';
				warningHTML += '<td class="warning">' + _("warning wireless security") + '</td>';
				warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button warning") + '" onClick=\'window.location.assign("/wireless/main.asp");\'></td>';
				warningHTML += '</tr>';
			}

			if  (warning_cwmpd) {
				warningHTML += '<tr>';
				warningHTML += '<td class="warning">' + _("warning cwmpd") + '</td>';
				warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button warning") + '" onClick=\'window.location.assign("/services/cwmp.asp");\'></td>';
				warningHTML += '</tr>';
			}

			if  (warning_cwmpd_auto_available) {
				warningHTML += '<tr>';
				warningHTML += '<td class="warning">' + _("warning cwmpd auto available") + '</td>';
				warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button enable") + '" onClick="enableCWMPAutoClick();"></td>';
				warningHTML += '</tr>';
			}

			warningHTML += '</table>';
			warningHTML += '<br>';
			ajaxModifyElementHTML('warning', warningHTML);
		}
	});
}

function resetValues(form, time, callback) {
	if (confirm(_('message reset confirm'))) {
		if (callback !== undefined)
			callback();

		var br = getBrowser();
		form.reset.value = "1";

		if (time === undefined) 
			time = 15;

		ajaxShowTimer(form, 'timerReloader', _('message apply'), time);
                return true;
	}

	return false;
}

// indexOf for IE8
if (!Array.prototype.indexOf) {
	Array.prototype.indexOf = function(elt /*, from*/) {
		var len = this.length >>> 0;
		var from = Number(arguments[1]) || 0;
		from = (from < 0) ? Math.ceil(from) : Math.floor(from);
		if (from < 0)
			from += len;
		for (; from < len; from++) {
			if (from in this && this[from] === elt)
				return from;
		}
		return -1;
	};
}

// Display service status

var displayServiceStatus_interval;

function getRunningServices(done) {
	var xmlHttp = createXMLHttp();
	var LAN_IP = '<% getLanIp(); %>';

	if (xmlHttp) {
		xmlHttp.onreadystatechange = function() {
			if (xmlHttp.readyState == 4) {
				var services = xmlHttp.responseText.split(',');
				done(services);

				// Free resources
				xmlHttp.onreadystatechange = null;
				xmlHttp = null;
			}
		}
		xmlHttp.open("GET", genRandomParam('/services/misc-stat.asp'), true);
		xmlHttp.send(null);
	}

}

function displayServiceStatus(services) {
	var LAN_IP = '<% getLanIp(); %>';

	getRunningServices(function(daemons) {
		// Now display all services
		for (var i = 0; i < services.length; i++) {
			var service = services[i];
			var is_running = daemons.indexOf(service[2]) != -1;
			var row = document.getElementById(service[1]);
			var tds = [];
			for (var j = 0; j < row.childNodes.length; j++)
				if (row.childNodes[j].nodeName == 'TD')
					tds.push(row.childNodes[j]);
			if (row != null) {
				// Fill-up status
				if (+service[0] == 0 || service[0] == 'none') {
					if (service[4] != null)
						tds[3].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
					else
						tds[2].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
				}
				else {
					if (service[4] != null)
						tds[3].innerHTML = is_running ?
							'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
							'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
					else
						tds[2].innerHTML = is_running ?
							'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
							'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
				}

				if (service[4] != null) {
					tds[2].innerHTML = '<a href="http://' + service[4] + '" target="_blank">' + _("services status about") + '</a>';
					if (tds[4] != null)
						tds[4].innerHTML = ((service[0] > '0') && is_running && (service[3] != null)) ?
							'<a href="http://' + LAN_IP +':' + service[3] + '">' + _("services status configure") + '</a>' : '&nbsp;';
				}
			}
		}
		displayServiceStatus_interval = setTimeout(function () { displayServiceStatus(services); }, 5000);
	});
}

// Translation
window._ = function(key) { return lang_gettext(key); };
window._TR = function(elementID, key) {
	var e = elementID;

	if (typeof e === 'string')
		e = document.getElementById(e);

	if (e != null)
		if (e.nodeName == 'INPUT')
			e.value = lang_gettext(key);
		else
			e.innerHTML = lang_gettext(key);
}

function lang_gettext(key) {
	try {
		if (key in lang_dictionary)
			return lang_dictionary[key];
		else
			console.log('Warning! Translation key not found: \'' + key + '\'');
	}
	catch (e) {
		console.log('Warning! Translation dictionary not loaded!');
	}
}

function init_translation_model()
{
	var trlist = document.getElementsByAttribute("data-tr");
	for (var i=0; i<trlist.length; i++) {
		var elem = trlist[i];
		_TR(elem, elem.getAttribute("data-tr"));
	}
}

function ajaxPing(on_response)
{
	var xmlHttp = createXMLHttp();
	if (!xmlHttp)
		return;

	xmlHttp.onreadystatechange = function()
	{
		if (xmlHttp.readyState == 4)
		{
			if (on_response != null) {
				on_response(xmlHttp.status);
				on_response = null;
			}

			xmlHttp.onreadystatechange = null;
			xmlHttp = null;
		}
	}

	xmlHttp.open("POST", "/goform/ping", true);
	xmlHttp.send();
}
