function defaultNumber(value, deflt)
{
	var re = /^\d+$/;
	return (re.test(value)) ? value : deflt;
}

function validateNum(str, floating)
{
	var re = (floating) ? /^\d+(?:\.\d+)$/ : /^\d+$/;
	return re.test(str);
}

function validatePort(field, info)
{
	var re = /^\d+$/;

	if (!re.test(field.value))
	{
		if (info)
		{
			alert("Invalid port value: " + field.value);
			field.focus();
		}
		
		return false;
	}
	
	// Cast to int
	var port = field.value*1;
	
	if ((port<1) || (port>65535))
	{
		if (info)
		{
			alert("Port value to be between 1 and 65535 but is set to: " + field.value);
			field.focus();
		}
		
		return false;
	}
	
	return true;
}

function validateMAC(mac, info)
{
	var re = /^(?:[A-Fa-f0-9]{2}\:){5}[A-Fa-f0-9]{2}$/;
	
	if (!re.test(mac))
	{
		if (info)
			alert("Please fill the MAC Address in correct format! (XX:XX:XX:XX:XX:XX)");
		return false;
	}
	return true;
}

function validateIP(ip, info)
{
	var re = /^(?:(?:0|1\d{0,2}|2([0-4]\d?|5[0-5]?|[6-9]?)|[3-9]\d?)\.){3}(?:0|1\d{0,2}|2([0-4]\d?|5[0-5]?|[6-9]?)|[3-9]\d?)$/;
	var is_string = typeof(ip) == 'string';
	var value = (is_string) ? ip : ip.value;
	
	if (value == "")
	{
		if (info)
		{
			alert("Error. IP address is empty.");
			if (!is_string)
				ip.focus();
		}
		return false;
	}
	if (!re.test(value))
	{
		if (info)
		{
			alert("Error. Invalid IP address format! (0-255.0-255.0-255.0-255)");
			if (!is_string)
				ip.focus();
		}
		return false;
	}
	
	return true;
}

function validateIPList(array)
{
	try
	{
		for (var i=0; i<array.length; i++)
		{
			if (!validateIP(array[i], true))
			{
				array[i].focus();
				return false;
			}
		}
		
		return true;
	}
	catch (e)
	{
		alert(e);
		return false;
	}
}

function validateIPMask(ip, info)
{
	var re = /^(?:(?:0|128|192|224|240|248|252|254)\.0+\.0+\.0+|255\.(?:(?:0|128|192|224|240|248|252|254)\.0+\.0+|255\.(?:(?:0|128|192|224|240|248|252|254)\.0+|255\.(?:0|128|192|224|240|248|252|254|255))))$/;
	var is_string = typeof(ip) == 'string';
	var value = (is_string) ? ip : ip.value;
	
	if (value == "")
	{
		if (info)
		{
			alert("Error. Mask is empty.");
			if (!is_string)
				ip.focus();
		}
		return false;
	}
	if (!re.test(value))
	{
		if (info)
		{
			alert("Error. Invalid IP mask format!");
			if (!is_string)
				ip.focus();
		}
		return false;
	}
	
	return true;
}

function validateIPMaskList(array)
{
	try
	{
		for (var i=0; i<array.length; i++)
		{
			if (!validateIPMask(array[i], true))
			{
				array[i].focus();
				return false;
			}
		}
		
		return true;
	}
	catch (e)
	{
		alert(e);
		return false;
	}
}

function validateRegexpList(array, regexp, message, match_result)
{
	try
	{
		for (var i=0; i<array.length; i++)
		{
			if (regexp.test(array[i].value) == match_result)
			{
				alert(message);
				array[i].focus();
				return false;
			}
		}
		
		return true;
	}
	catch (e)
	{
		alert(e);
		return false;
	}
}

function validateBlanksList(array)
{
	return validateRegexpList(array, /\s+/, 'Blank characters are not allowed!', true);
}

function validateNumberList(array)
{
	return validateRegexpList(array, /^\d+$/, 'Bad number value specified!', false);
}

function validateDNS(dns, info)
{
	var re = /^(?:([a-zA-Z0-9\-]+)\.){0,}[a-zA-Z0-9\-]+\.?$/;
	
	if (dns.value == "")
	{
		if (info)
			alert("Error. Domain name is invalid.");
		return false;
	}
	if (!re.test(dns.value))
	{
		if (info)
			alert("Error. Invalid domain name format!");
		return false;
	}
	
	return true;
}

function checkDigitRange(value, start, stop)
{
	var re = /^\d+$/;

	if (re.test(value))
	{
		var val = parseInt(value);
		return (val>=start) && (val<=stop);
	}
	
	return false;
}

function checkFilePresent(filename)
{
	if ((filename.value == null) || (filename.value == ''))
	{
		alert('Please specify file to upload!');
		return false;
	}
	
	return true;
}
