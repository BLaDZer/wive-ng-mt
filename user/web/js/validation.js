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

function validateHex(str) {
	var re = /^[0-9a-fA-F]+$/;
	return re.test(str);
}

function validateASCII(str, printable, extended) {
    var re;

    if (extended)
        re = printable ? /^[\x20-\xFF]*$/ : /^[\x00-\xFF]*$/;
    else
        re = printable ? /^[\x20-\x7F]*$/ : /^[\x00-\x7F]*$/;

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

function validateUnicastMAC(mac, info)
{
	if (validateMAC(mac, info) == false)
		return false;

	var octets = mac.toLowerCase().split(":");
	if (octets.length != 6) return false;

	// IPv4 Multicast
	if ((octets[0] == "00" || octets[0] == "01" || octets[0] == "02") && octets[1] == "00" && octets[2] == "5e")
		return false;

	// IEEE (standard protocols)
	if (octets[0] == "01" && octets[1] == "80" && octets[2] == "c2")
		return false;

	// IPv6 Multicast
	if (octets[0] == "33" && octets[1] == "33")
		return false;

	// Cisco Protocols
	if (octets[0] == "01" && octets[1] == "00" && octets[2] == "0c" && octets[3] == "cc" && octets[4] == "cc" && (octets[5] == "cc" || octets[5] == "cd"))
		return false;

	// Broadcast
	if (octets[0] == "ff" && octets[1] == "ff" && octets[2] == "ff" && octets[3] == "ff" && octets[4] == "ff" && octets[5] == "ff")
		return false;

	return true;
}

function validateMACList(array, info)
{
	try
	{
		for (var i=0; i<array.length; i++)
		{
			if (!validateMAC(array[i], info))
				return false;
		}
		
		return true;
	}
	catch (e)
	{
		alert(e);
		return false;
	}
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

// A library for manipulating IPv4 and IPv6 addresses in JavaScript.
(function() {
    var expandIPv6, ipaddr, ipv4Part, ipv4Regexes, ipv6Part, ipv6Regexes, matchCIDR, root;

    ipaddr = {};

    root = this;

    if ((typeof module !== "undefined" && module !== null) && module.exports) {
        module.exports = ipaddr;
    } else {
        root['ipaddr'] = ipaddr;
    }

    matchCIDR = function(first, second, partSize, cidrBits) {
        var part, shift;
        if (first.length !== second.length) {
            throw new Error("ipaddr: cannot match CIDR for objects with different lengths");
        }
        part = 0;
        while (cidrBits > 0) {
            shift = partSize - cidrBits;
            if (shift < 0) {
                shift = 0;
            }
            if (first[part] >> shift !== second[part] >> shift) {
                return false;
            }
            cidrBits -= partSize;
            part += 1;
        }
        return true;
    };

    ipaddr.subnetMatch = function(address, rangeList, defaultName) {
        var rangeName, rangeSubnets, subnet, _i, _len;
        if (defaultName == null) {
            defaultName = 'unicast';
        }
        for (rangeName in rangeList) {
            rangeSubnets = rangeList[rangeName];
            if (rangeSubnets[0] && !(rangeSubnets[0] instanceof Array)) {
                rangeSubnets = [rangeSubnets];
            }
            for (_i = 0, _len = rangeSubnets.length; _i < _len; _i++) {
                subnet = rangeSubnets[_i];
                if (address.match.apply(address, subnet)) {
                    return rangeName;
                }
            }
        }
        return defaultName;
    };

    ipaddr.IPv4 = (function() {
        function IPv4(octets) {
            var octet, _i, _len;
            if (octets.length !== 4) {
                throw new Error("ipaddr: ipv4 octet count should be 4");
            }
            for (_i = 0, _len = octets.length; _i < _len; _i++) {
                octet = octets[_i];
                if (!((0 <= octet && octet <= 255))) {
                    throw new Error("ipaddr: ipv4 octet should fit in 8 bits");
                }
            }
            this.octets = octets;
        }

        IPv4.prototype.kind = function() {
            return 'ipv4';
        };

        IPv4.prototype.toString = function() {
            return this.octets.join(".");
        };

        IPv4.prototype.toByteArray = function() {
            return this.octets.slice(0);
        };

        IPv4.prototype.match = function(other, cidrRange) {
            var _ref;
            if (cidrRange === void 0) {
                _ref = other, other = _ref[0], cidrRange = _ref[1];
            }
            if (other.kind() !== 'ipv4') {
                throw new Error("ipaddr: cannot match ipv4 address with non-ipv4 one");
            }
            return matchCIDR(this.octets, other.octets, 8, cidrRange);
        };

        IPv4.prototype.toNumber = function() {
            var res = 0;
            var arr = this.octets.reverse();
            for (ind in arr) {
                res = res + Number(arr[ind]) * Math.pow(256, ind);
            }
            return res;
        };

        IPv4.prototype.SpecialRanges = {
            unspecified: [[new IPv4([0, 0, 0, 0]), 8]],
            broadcast: [[new IPv4([255, 255, 255, 255]), 32]],
            multicast: [[new IPv4([224, 0, 0, 0]), 4]],
            linkLocal: [[new IPv4([169, 254, 0, 0]), 16]],
            loopback: [[new IPv4([127, 0, 0, 0]), 8]],
            "private": [[new IPv4([10, 0, 0, 0]), 8], [new IPv4([172, 16, 0, 0]), 12], [new IPv4([192, 168, 0, 0]), 16]],
            reserved: [[new IPv4([192, 0, 0, 0]), 24], [new IPv4([192, 0, 2, 0]), 24], [new IPv4([192, 88, 99, 0]), 24], [new IPv4([198, 51, 100, 0]), 24], [new IPv4([203, 0, 113, 0]), 24], [new IPv4([240, 0, 0, 0]), 4]]
        };

        IPv4.prototype.range = function() {
            return ipaddr.subnetMatch(this, this.SpecialRanges);
        };

        IPv4.prototype.toIPv4MappedAddress = function() {
            return ipaddr.IPv6.parse("::ffff:" + (this.toString()));
        };

        IPv4.prototype.prefixLengthFromSubnetMask = function() {
            var cidr, i, octet, stop, zeros, zerotable, _i;
            zerotable = {
                0: 8,
                128: 7,
                192: 6,
                224: 5,
                240: 4,
                248: 3,
                252: 2,
                254: 1,
                255: 0
            };
            cidr = 0;
            stop = false;
            for (i = _i = 3; _i >= 0; i = _i += -1) {
                octet = this.octets[i];
                if (octet in zerotable) {
                    zeros = zerotable[octet];
                    if (stop && zeros !== 0) {
                        return null;
                    }
                    if (zeros !== 8) {
                        stop = true;
                    }
                    cidr += zeros;
                } else {
                    return null;
                }
            }
            return 32 - cidr;
        };

        return IPv4;

    })();

    ipv4Part = "(0?\\d+|0x[a-f0-9]+)";

    ipv4Regexes = {
        fourOctet: new RegExp("^" + ipv4Part + "\\." + ipv4Part + "\\." + ipv4Part + "\\." + ipv4Part + "$", 'i'),
        longValue: new RegExp("^" + ipv4Part + "$", 'i')
    };

    ipaddr.IPv4.parser = function(string) {
        var match, parseIntAuto, part, shift, value;
        parseIntAuto = function(string) {
            if (string[0] === "0" && string[1] !== "x") {
                return parseInt(string, 8);
            } else {
                return parseInt(string);
            }
        };
        if (match = string.match(ipv4Regexes.fourOctet)) {
            return (function() {
                var _i, _len, _ref, _results;
                _ref = match.slice(1, 6);
                _results = [];
                for (_i = 0, _len = _ref.length; _i < _len; _i++) {
                    part = _ref[_i];
                    _results.push(parseIntAuto(part));
                }
                return _results;
            })();
        } else if (match = string.match(ipv4Regexes.longValue)) {
            value = parseIntAuto(match[1]);
            if (value > 0xffffffff || value < 0) {
                throw new Error("ipaddr: address outside defined range");
            }
            return ((function() {
                var _i, _results;
                _results = [];
                for (shift = _i = 0; _i <= 24; shift = _i += 8) {
                    _results.push((value >> shift) & 0xff);
                }
                return _results;
            })()).reverse();
        } else {
            return null;
        }
    };

    ipaddr.IPv6 = (function() {
        function IPv6(parts) {
            var i, part, _i, _j, _len, _ref;
            if (parts.length === 16) {
                this.parts = [];
                for (i = _i = 0; _i <= 14; i = _i += 2) {
                    this.parts.push((parts[i] << 8) | parts[i + 1]);
                }
            } else if (parts.length === 8) {
                this.parts = parts;
            } else {
                throw new Error("ipaddr: ipv6 part count should be 8 or 16");
            }
            _ref = this.parts;
            for (_j = 0, _len = _ref.length; _j < _len; _j++) {
                part = _ref[_j];
                if (!((0 <= part && part <= 0xffff))) {
                    throw new Error("ipaddr: ipv6 part should fit in 16 bits");
                }
            }
        }

        IPv6.prototype.kind = function() {
            return 'ipv6';
        };

        IPv6.prototype.toString = function() {
            var compactStringParts, part, pushPart, state, stringParts, _i, _len;
            stringParts = (function() {
                var _i, _len, _ref, _results;
                _ref = this.parts;
                _results = [];
                for (_i = 0, _len = _ref.length; _i < _len; _i++) {
                    part = _ref[_i];
                    _results.push(part.toString(16));
                }
                return _results;
            }).call(this);
            compactStringParts = [];
            pushPart = function(part) {
                return compactStringParts.push(part);
            };
            state = 0;
            for (_i = 0, _len = stringParts.length; _i < _len; _i++) {
                part = stringParts[_i];
                switch (state) {
                    case 0:
                        if (part === '0') {
                            pushPart('');
                        } else {
                            pushPart(part);
                        }
                        state = 1;
                        break;
                    case 1:
                        if (part === '0') {
                            state = 2;
                        } else {
                            pushPart(part);
                        }
                        break;
                    case 2:
                        if (part !== '0') {
                            pushPart('');
                            pushPart(part);
                            state = 3;
                        }
                        break;
                    case 3:
                        pushPart(part);
                }
            }
            if (state === 2) {
                pushPart('');
                pushPart('');
            }
            return compactStringParts.join(":");
        };

        IPv6.prototype.toByteArray = function() {
            var bytes, part, _i, _len, _ref;
            bytes = [];
            _ref = this.parts;
            for (_i = 0, _len = _ref.length; _i < _len; _i++) {
                part = _ref[_i];
                bytes.push(part >> 8);
                bytes.push(part & 0xff);
            }
            return bytes;
        };

        IPv6.prototype.toNormalizedString = function() {
            var part;
            return ((function() {
                var _i, _len, _ref, _results;
                _ref = this.parts;
                _results = [];
                for (_i = 0, _len = _ref.length; _i < _len; _i++) {
                    part = _ref[_i];
                    _results.push(part.toString(16));
                }
                return _results;
            }).call(this)).join(":");
        };

        IPv6.prototype.match = function(other, cidrRange) {
            var _ref;
            if (cidrRange === void 0) {
                _ref = other, other = _ref[0], cidrRange = _ref[1];
            }
            if (other.kind() !== 'ipv6') {
                throw new Error("ipaddr: cannot match ipv6 address with non-ipv6 one");
            }
            return matchCIDR(this.parts, other.parts, 16, cidrRange);
        };

        IPv6.prototype.SpecialRanges = {
            unspecified: [new IPv6([0, 0, 0, 0, 0, 0, 0, 0]), 128],
            linkLocal: [new IPv6([0xfe80, 0, 0, 0, 0, 0, 0, 0]), 10],
            multicast: [new IPv6([0xff00, 0, 0, 0, 0, 0, 0, 0]), 8],
            loopback: [new IPv6([0, 0, 0, 0, 0, 0, 0, 1]), 128],
            uniqueLocal: [new IPv6([0xfc00, 0, 0, 0, 0, 0, 0, 0]), 7],
            ipv4Mapped: [new IPv6([0, 0, 0, 0, 0, 0xffff, 0, 0]), 96],
            rfc6145: [new IPv6([0, 0, 0, 0, 0xffff, 0, 0, 0]), 96],
            rfc6052: [new IPv6([0x64, 0xff9b, 0, 0, 0, 0, 0, 0]), 96],
            '6to4': [new IPv6([0x2002, 0, 0, 0, 0, 0, 0, 0]), 16],
            teredo: [new IPv6([0x2001, 0, 0, 0, 0, 0, 0, 0]), 32],
            reserved: [[new IPv6([0x2001, 0xdb8, 0, 0, 0, 0, 0, 0]), 32]]
        };

        IPv6.prototype.range = function() {
            return ipaddr.subnetMatch(this, this.SpecialRanges);
        };

        IPv6.prototype.isIPv4MappedAddress = function() {
            return this.range() === 'ipv4Mapped';
        };

        IPv6.prototype.toIPv4Address = function() {
            var high, low, _ref;
            if (!this.isIPv4MappedAddress()) {
                throw new Error("ipaddr: trying to convert a generic ipv6 address to ipv4");
            }
            _ref = this.parts.slice(-2), high = _ref[0], low = _ref[1];
            return new ipaddr.IPv4([high >> 8, high & 0xff, low >> 8, low & 0xff]);
        };

        return IPv6;

    })();

    ipv6Part = "(?:[0-9a-f]+::?)+";

    ipv6Regexes = {
        "native": new RegExp("^(::)?(" + ipv6Part + ")?([0-9a-f]+)?(::)?$", 'i'),
        transitional: new RegExp(("^((?:" + ipv6Part + ")|(?:::)(?:" + ipv6Part + ")?)") + ("" + ipv4Part + "\\." + ipv4Part + "\\." + ipv4Part + "\\." + ipv4Part + "$"), 'i')
    };

    expandIPv6 = function(string, parts) {
        var colonCount, lastColon, part, replacement, replacementCount;
        if (string.indexOf('::') !== string.lastIndexOf('::')) {
            return null;
        }
        colonCount = 0;
        lastColon = -1;
        while ((lastColon = string.indexOf(':', lastColon + 1)) >= 0) {
            colonCount++;
        }
        if (string.substr(0, 2) === '::') {
            colonCount--;
        }
        if (string.substr(-2, 2) === '::') {
            colonCount--;
        }
        if (colonCount > parts) {
            return null;
        }
        replacementCount = parts - colonCount;
        replacement = ':';
        while (replacementCount--) {
            replacement += '0:';
        }
        string = string.replace('::', replacement);
        if (string[0] === ':') {
            string = string.slice(1);
        }
        if (string[string.length - 1] === ':') {
            string = string.slice(0, -1);
        }
        return (function() {
            var _i, _len, _ref, _results;
            _ref = string.split(":");
            _results = [];
            for (_i = 0, _len = _ref.length; _i < _len; _i++) {
                part = _ref[_i];
                _results.push(parseInt(part, 16));
            }
            return _results;
        })();
    };

    ipaddr.IPv6.parser = function(string) {
        var match, octet, octets, parts, _i, _len;
        if (string.match(ipv6Regexes['native'])) {
            return expandIPv6(string, 8);
        } else if (match = string.match(ipv6Regexes['transitional'])) {
            parts = expandIPv6(match[1].slice(0, -1), 6);
            if (parts) {
                octets = [parseInt(match[2]), parseInt(match[3]), parseInt(match[4]), parseInt(match[5])];
                for (_i = 0, _len = octets.length; _i < _len; _i++) {
                    octet = octets[_i];
                    if (!((0 <= octet && octet <= 255))) {
                        return null;
                    }
                }
                parts.push(octets[0] << 8 | octets[1]);
                parts.push(octets[2] << 8 | octets[3]);
                return parts;
            }
        }
        return null;
    };

    ipaddr.IPv4.isIPv4 = ipaddr.IPv6.isIPv6 = function(string) {
        return this.parser(string) !== null;
    };

    ipaddr.IPv4.isValid = function(string) {
        var e;
        try {
            new this(this.parser(string));
            return true;
        } catch (_error) {
            e = _error;
            return false;
        }
    };

    ipaddr.IPv6.isValid = function(string) {
        var e;
        if (typeof string === "string" && string.indexOf(":") === -1) {
            return false;
        }
        try {
            new this(this.parser(string));
            return true;
        } catch (_error) {
            e = _error;
            return false;
        }
    };

    ipaddr.IPv4.parse = ipaddr.IPv6.parse = function(string) {
        var parts;
        parts = this.parser(string);
        if (parts === null) {
            throw new Error("ipaddr: string is not formatted like ip address");
        }
        return new this(parts);
    };

    ipaddr.IPv4.parseCIDR = function(string) {
        var maskLength, match;
        if (match = string.match(/^(.+)\/(\d+)$/)) {
            maskLength = parseInt(match[2]);
            if (maskLength >= 0 && maskLength <= 32) {
                return [this.parse(match[1]), maskLength];
            }
        }
        throw new Error("ipaddr: string is not formatted like an IPv4 CIDR range");
    };

    ipaddr.IPv6.parseCIDR = function(string) {
        var maskLength, match;
        if (match = string.match(/^(.+)\/(\d+)$/)) {
            maskLength = parseInt(match[2]);
            if (maskLength >= 0 && maskLength <= 128) {
                return [this.parse(match[1]), maskLength];
            }
        }
        throw new Error("ipaddr: string is not formatted like an IPv6 CIDR range");
    };

    ipaddr.isValid = function(string) {
        return ipaddr.IPv6.isValid(string) || ipaddr.IPv4.isValid(string);
    };

    ipaddr.parse = function(string) {
        if (ipaddr.IPv6.isValid(string)) {
            return ipaddr.IPv6.parse(string);
        } else if (ipaddr.IPv4.isValid(string)) {
            return ipaddr.IPv4.parse(string);
        } else {
            throw new Error("ipaddr: the address has neither IPv6 nor IPv4 format");
        }
    };

    ipaddr.parseCIDR = function(string) {
        var e;
        try {
            return ipaddr.IPv6.parseCIDR(string);
        } catch (_error) {
            e = _error;
            try {
                return ipaddr.IPv4.parseCIDR(string);
            } catch (_error) {
                e = _error;
                throw new Error("ipaddr: the address has neither IPv6 nor IPv4 CIDR format");
            }
        }
    };

    ipaddr.fromByteArray = function(bytes) {
        var length;
        length = bytes.length;
        if (length === 4) {
            return new ipaddr.IPv4(bytes);
        } else if (length === 16) {
            return new ipaddr.IPv6(bytes);
        } else {
            throw new Error("ipaddr: the binary input is neither an IPv6 nor IPv4 address");
        }
    };

    ipaddr.process = function(string) {
        var addr;
        addr = this.parse(string);
        if (addr.kind() === 'ipv6' && addr.isIPv4MappedAddress()) {
            return addr.toIPv4Address();
        } else {
            return addr;
        }
    };

}).call(this);
