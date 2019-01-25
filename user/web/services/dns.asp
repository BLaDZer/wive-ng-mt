<!DOCTYPE html>
<html>
	<head>
		<title>DNS Services</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
				init_translation_model();

				if (document.getElementById('DDNSProvider').value != "freedns.afraid.org")
					_TR("manDdnsPasswd", "services l2tp password");
				else
					_TR("manDdnsPasswd", "services ddns key");
			}
			
			function initValues() {
				var form = document.DNSForm;

				form.dnsPEnabled.options.selectedIndex = NVRAM_dnsPEnabled;
				form.dnsToLocalRedir.value = NVRAM_dnsToLocalRedir;

				form.dns_adblock.selectedIndex = NVRAM_dns_adblock;

				var dns_userblock = NVRAM_dns_userblock.split(" ");
				if (dns_userblock.length == 1 && dns_userblock[0] == "") {
					dns_userblock.length = 0;
				}

				for (var i=0;i<dns_userblock.length;i++) {
					addAdblockEntry(dns_userblock[i], 0);
				}

				var adblock_skip_domains = NVRAM_dns_adblock_skip.split("|");
				if (adblock_skip_domains.length == 1 && adblock_skip_domains[0] == "") {
					adblock_skip_domains.length = 0;
				}

				for (var i=0;i<adblock_skip_domains.length;i++) {
					addAdblockEntry(adblock_skip_domains[i], 1);
				}

				form.dns_sec.selectedIndex = NVRAM_dns_sec;

				switch (NVRAM_DDNSProvider) {
					case 'none':			document.getElementById('DDNSProvider').options.selectedIndex = 0;	break;
					case 'dyndns.org':		document.getElementById('DDNSProvider').options.selectedIndex = 1;	break;
					case 'freedns.afraid.org':	document.getElementById('DDNSProvider').options.selectedIndex = 2;	break;
					case 'zoneedit.com':		document.getElementById('DDNSProvider').options.selectedIndex = 3;	break;
					case 'no-ip.com':		document.getElementById('DDNSProvider').options.selectedIndex = 4;	break;
					default:			document.getElementById('DDNSProvider').options.selectedIndex = 0;
				}
				document.getElementById('Account').value	= NVRAM_DDNSAccount;
				document.getElementById('Password').value	= NVRAM_DDNSPassword;
				document.getElementById('DDNS').value		= NVRAM_DDNS;

				DDNSupdateState();
				showWarning();
				displayServiceStatus([[ NVRAM_DDNSProvider, 'inadyn', 'inadyn', null, 'wikipedia.org/wiki/Dynamic_DNS' ]]);
				displayServiceStatus([[ NVRAM_dnsPEnabled, 'dnsproxy', 'dnsmasq', null, 'thekelleys.org.uk/dnsmasq/doc.html' ]]);

				displayAdblockStatus(form);

				setInterval(function() {
					displayAdblockStatus(form);
				}, 5000);

				updateVisibility(form);
				initHosts();
			}

			function validateValue(val) {
				if (val == '')
					return false;

				if (validateASCII(val, true, false) == false)
					return false;

				if (/[;"]/.test(val))
					return false;

				return true;
			}

			function DNSFormCheck() {
				var form = document.DNSForm;

				if (form.DDNSProvider.value != 'none')
				{
					if (form.DDNSProvider.value != 'freedns.afraid.org' && !validateValue(form.Account.value)) {
						alert(_("services ddns invalid login"));
						form.Account.focus();
						form.Account.select();
						return false;
					}

					if (!validateValue(form.Password.value)) {
						alert(_("services ddns invalid password"));
						form.Password.focus();
						form.Password.select();
						return false;
					}

					if (!validateValue(form.DDNS.value)) {
						alert(_("services ddns invalid domain"));
						form.DDNS.focus();
						form.DDNS.select();
						return false;
					}
				}

				var elems = document.getElementsByClassName("adblock_entry");
				var skip_str = "";
				var block_str = "";
				for (var i=0;i<elems.length;i++) {
					var arr = adblockEntryToArray(elems[i]);
					var name = arr[0];
					var action = arr[1];

					if (action == "1") {
						if (skip_str != "") skip_str += "|";
						skip_str += name;
					} else {
						if (block_str != "") block_str += " ";
						block_str += name;
					}
				}

				form.dns_adblock_skip.value = skip_str;
				form.dns_userblock.value = block_str;

				var elems = document.getElementsByClassName("hosts_entry_user_defined");
				var hosts_str = "";
				for (var i=0;i<elems.length;i++) {
					var hostarr = hostsEntryToArray(elems[i]);
					var hosts_ip = hostarr[0];
					var hosts_name = hostarr[1];
					if (i>0) hosts_str += ";";
					hosts_str += hosts_ip;
					hosts_str += ",";
					hosts_str += hosts_name;
				}
				form.dns_local_hosts.value = hosts_str;

				ajaxShowTimer(document.DNSForm, 'timerReloader', _('message apply'), 7);
				return true;
			}

			function DDNSupdateState() {
				enableElements([ document.getElementById('Password'), document.getElementById('DDNS') ], document.getElementById('DDNSProvider').options.selectedIndex != 0);
				displayElement([ 'div_password', 'div_dynname' ], document.getElementById('DDNSProvider').options.selectedIndex != 0 );
				enableElements(document.getElementById('Account'), document.getElementById('DDNSProvider').options.selectedIndex != 0 && document.getElementById('DDNSProvider').value != 'freedns.afraid.org');
				displayElement('div_login', document.getElementById('DDNSProvider').options.selectedIndex != 0 && document.getElementById('DDNSProvider').value != 'freedns.afraid.org');
				initTranslation();
			}

			function updateVisibility(form) {
				displayElement('dnsproxy',	BUILD_DNSMASQ);
				displayElement(['div_hosts', 'div_hosts_add', 'div_dnsblock', 'dnsToLocalRedirRow', 'div_misc'], form.dnsPEnabled.value != '0');
				displayElement(['div_dnsblock_except', 'div_dnsblock_except_add'], form.dnsPEnabled.value != '0' && form.dns_adblock.value != '0');
				displayElement('div_dnssec',	BUILD_DNSMASQSEC && BUILD_NETTLE);
			}

			function deleteDnsEntry(elem) {
				var entries_container = document.getElementById("localDnsEntries");
				entries_container.removeChild(elem);
			}

			function deleteAdblockEntry(elem) {
				var entries_container = document.getElementById("dnsFilterExceptions");
				entries_container.removeChild(elem);
			}

			function addAdblockEntry(name, action) {
				var entries_container = document.getElementById("dnsFilterExceptions");
				var elems = document.getElementsByClassName("adblock_entry");
				if (name == "") return false;

				for (var i=0;i<elems.length;i++) {

					var adblock_name = adblockEntryToArray(elems[i])[0];
					if (adblock_name == name) {
						return false;
					}
				}

				entries_container.appendChild(generateAdblockEntry(name, action));
				return true;
			}

			function addAdblockEntryClick() {
				var name_elem = document.getElementById("dnsblock_except_add_domain");
				var action_elem = document.getElementById("dnsblock_except_add_action");

				if (!/^[A-Za-z0-9.-]+$/.test(name_elem.value)) {
					alert(_('services dns wrong domain'));
					name_elem.focus();
					return false;
				}

				if (!addAdblockEntry(name_elem.value, action_elem.value)) {
					alert(_('services dns domain in use'));
					name_elem.focus();
					return false;
				}

				return true;
			}

			function adblockEntryToArray(entry) {
				var name = "";
				var action = 0;
				if (entry.className.indexOf(" adblock_entry_allow") != -1) {
					action = 1;
				}

				for (var i=0;i<entry.childNodes.length;i++) {
					var cnode = entry.childNodes[i];
					if (cnode.className == "adblock_name") name = cnode.innerHTML;
				}
				return [name,action];
			}

			function generateAdblockEntry(name, action) {
				var tr = document.createElement("tr");
				if (action == "1") {
					tr.className = "adblock_entry adblock_entry_allow";
				} else {
					tr.className = "adblock_entry adblock_entry_block";
				}

				var td = document.createElement("td");
				td.className = "adblock_name";
				td.innerHTML = name;
				tr.appendChild(td);

				var td = document.createElement("td");
				td.className = "adblock_action";
				td.innerHTML = action=="1"?_("services dns filter except allow"):_("services dns filter except block");
				td.style.textAlign = "center";
				tr.appendChild(td);

				var td = document.createElement("td");
				td.style.textAlign = "center";
				td.style.cursor = "pointer";
				td.innerHTML = "<a onClick='deleteAdblockEntry(this.parentElement.parentElement);' style='color: #ff0000;' title='" + _("services dns hosts remove") + "' ><img src='/graphics/cross.png' alt='[x]'></a>";
				tr.appendChild(td);

				return tr;
			}

			function addDnsEntry() {
				var entries_container = document.getElementById("localDnsEntries");
				var ip_elem = document.getElementById("hosts_new_ip");
				var name_elem = document.getElementById("hosts_new_name");

				try {
					ipaddr.IPv6.parse(ip_elem.value);
				} catch (e) {
					if (!validateIP(ip_elem, true)) {
						ip_elem.focus();
						return false;
					}
				}

				if (!/^[A-Za-z0-9.-]+$/.test(name_elem.value)) {
					alert(_('services dns wrong domain'));
					name_elem.focus();
					return false;
				}

				var elems = document.getElementsByClassName("hosts_entry");
				for (var i=0;i<elems.length;i++) {

					var hosts_name = hostsEntryToArray(elems[i])[1];
					if (hosts_name == name_elem.value) {
						alert(_('services dns domain in use'));
						name_elem.focus();
						return false;
					}
				}

				entries_container.appendChild(generateHostsEntry(ip_elem.value, name_elem.value, 1));
				return true;
			}

			function hostsEntryToArray(entry) {
				var user_defined = 0;
				var ip = "";
				var name = "";

				if (entry.className.indexOf(" hosts_entry_user_defined") != -1) user_defined = 1;
				for (var i=0;i<entry.childNodes.length;i++) {
					var cnode = entry.childNodes[i];
					if (cnode.className == "hosts_ip") ip = cnode.innerHTML;
					if (cnode.className == "hosts_name") name = cnode.innerHTML;
				}
				return [ip, name, user_defined];
			}

			function generateHostsEntry(ip, name, user_defined) {
				var tr = document.createElement("tr");
				tr.className = "hosts_entry";

				var td = document.createElement("td");
				td.className = "hosts_ip";
				td.innerHTML = ip;
				tr.appendChild(td);

				var td = document.createElement("td");
				td.className = "hosts_name";
				td.innerHTML = name;
				if (!user_defined) td.colSpan = 2;
				tr.appendChild(td);

				if (user_defined) {
					tr.className = "hosts_entry hosts_entry_user_defined";
					var td = document.createElement("td");
					td.style.textAlign = "center";
					td.style.cursor = "pointer";
					td.innerHTML = "<a onClick='deleteDnsEntry(this.parentElement.parentElement);' style='color: #ff0000;' title='" + _("services dns hosts remove") + "' ><img src='/graphics/cross.png' alt='[x]'></a>";
					tr.appendChild(td);
				}
				return tr;
			}

			function initHosts(form) {
				var hosts_data = <% getHosts(); %>;

				var entries_container = document.getElementById("localDnsEntries");
				while (entries_container.firstChild)
					entries_container.removeChild(entries_container.firstChild);

				var html = '';
				for (var i=0;i<hosts_data.length;i++) {
					var entry = hosts_data[i]; // ip, name, user_defined
					var ip = entry[0];
					var name = entry[1];
					var user_defined = entry[2];

					entries_container.appendChild(generateHostsEntry(ip, name, user_defined));
				}
			}

			function displayAdblockStatus(form) {
				if (form.dns_adblock.value == '0') {
					displayServiceStatusView([ '0', 'dnsfilter', null, null, 'wikipedia.org/wiki/Ad_blocking' ], false);
				} else {
					ajaxPerformRequest("/goform/getAdblockStatus", function(content) {
						displayServiceStatusView([ content!=0?'1':'0', 'dnsfilter', null, null, 'wikipedia.org/wiki/Ad_blocking' ], content == '2');
					});
				}
			}

			function changeDNSSEC(form) {
				if (form.dns_sec.selectedIndex > 0 && !confirm(_("services dns misc dnssec confirm"))) {
					form.dns_sec.selectedIndex = 0;
					return false;
				}
			}


		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 data-tr="services dns title">DNS Settings</h1>
					<p data-tr="services dns introduction">Here you can configure DNS settings.</p>
					<hr>
					<!-- ================= DDNS  ================= -->
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="DNSForm" action="/goform/setDns" OnSubmit="return DNSFormCheck(this)">

					<table id="div_dnsproxy" class="form">
						<col style="width: 40%"/>
						<col style="width: 34%"/>
						<col style="width: 13%"/>
						<col style="width: 13%"/>
						<thead>
							<td class="title" colspan="4" data-tr="services dns setup">DNS Settings</td>
						</thead>
						<tbody>
							<tr id="dnsproxy">
								<td class="head" id="dnsproxy_td">DNS Proxy</td>
								<td>
									<select name="dnsPEnabled" class="mid" onChange="updateVisibility(this.form);">
										<option value="0" data-tr="button disable">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select>
								</td>
								<td style="text-align: center;" >&nbsp;</td>
								<td style="text-align: center;">&nbsp;</td>
							</tr>

							<tr id="dnsToLocalRedirRow">
								<td class="head" data-tr="services dns redirect">Redirect all DNS to Local server</td>
								<td colspan="3">
									<select id="dnsToLocalRedir" name="dnsToLocalRedir" class="mid">
										<option value="0" data-tr="button disable" selected="selected">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select>
								</td>
							</tr>

						</tbody>
					</table>

					<table id="div_dnsblock" class="form">
						<col style="width: 40%"/>
						<col style="width: 34%"/>
						<col style="width: 13%"/>
						<col style="width: 13%"/>
						<thead>
							<td class="title" colspan="4" data-tr="services dns filter">DNS Content Filter</td>
						</thead>
						<tbody>
							<tr id="dnsfilter">
								<td class="head" id="dnsfilter_td" data-tr="services dns filter enable">Enable DNS Filtering</td>
								<td>
									<select name="dns_adblock" class="mid" onChange="updateVisibility(this.form);">
										<option value="0" data-tr="button disable">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select>
								</td>
								<td style="text-align: center;">&nbsp;</td>
								<td style="text-align: center;">&nbsp;</td>
							</tr>
						</tbody>
					</table>

					<input type="hidden" id="dns_adblock_skip" name="dns_adblock_skip"></input>
					<input type="hidden" id="dns_userblock" name="dns_userblock"></input>

					<table id="div_dnsblock_except" class="form">
						<col style="width: 74%" />
						<col style="width: 13%" />
						<col style="width: 13%" />
						<thead>
							<tr>
								<td class="title" data-tr="services dns filter except title" colspan="4">DNS Content Filter Exceptions</td>
							</tr>
							<tr>
								<th data-tr="inet domain">Domain Name</th>
								<th data-tr="services dns filter except action" colspan="3">Action</th>
							</tr>
						</thead>
						<tbody id="dnsFilterExceptions">
						</tbody>
					</table>

					<table id="div_dnsblock_except_add" class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<thead>
							<tr>
								<td class="title" data-tr="services dns filter except add title" colspan="2">Add DNS Content Filter Exception</td>
							</tr>
						</thead>
						<tbody>
							<tr>
								<td class="head" data-tr="inet domain">Domain Name</td>
								<td><input type="text" class="mid" id="dnsblock_except_add_domain"></input></td>
							</tr>
							<tr>
								<td class="head" data-tr="services dns filter except action">Action</td>
								<td>
									<select class="mid" id="dnsblock_except_add_action">
										<option value="0" selected data-tr="services dns filter except block">Block</option>
										<option value="1" data-tr="services dns filter except allow">Allow</option>
									</select>
								</td>
							</tr>
							<tr>
								<td colspan="2"><input type="button" data-tr="services dns filter except add" value="Add"  onClick='addAdblockEntryClick();'></input></td>
							</tr>
						</tbody>
					</table>


					<input type="hidden" id="dns_local_hosts" name="dns_local_hosts"></input>
					<table class="form" id="div_hosts">
						<col style="width: 40%" />
						<col style="width: 47%" />
						<col style="width: 13%" />
						<thead>
							<tr>
								<td class="title" data-tr="services dns hosts title" colspan="3">Local DNS Entries</td>
							</tr>
							<tr>
								<th data-tr="inet ip">IP Address</th>
								<th data-tr="inet domain" colspan="2">Domain Name</th>
							</tr>
						</thead>
						<tbody id="localDnsEntries">
						</tbody>
					</table>

					<table class="form" id="div_hosts_add">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<thead>
							<tr>
								<td class="title" data-tr="services dns hosts add title"  colspan="2">Add Local DNS Entry</td>
							</tr>
						</thead>
						<tbody>
							<tr>
								<td class="head" data-tr="inet ip">IP Address</td>
								<td><input type='text' id='hosts_new_ip'></input></td>
							</tr>
							<tr>
								<td class="head" data-tr="inet domain">Domain Name</td>
								<td><input type='text' id='hosts_new_name'></input></td>
							</tr>
							<tr>
								<td colspan=2><input type='button' data-tr="services dns hosts add button" onClick='addDnsEntry();'></input></td>
							</tr>
						</tbody>
					</table>

					<table id="div_ddns" class="form">
						<col style="width: 40%"/>
						<col style="width: 34%"/>
						<col style="width: 13%"/>
						<col style="width: 13%"/>
						<tbody>
							<tr>
								<td class="title" colspan="4" data-tr="services ddns setup">DDNS Settings</td>
							</tr>

							<tr id="inadyn">
								<td class="head" data-tr="services ddns provider">Dynamic DNS Provider</td>
								<td>
									<select onChange="DDNSupdateState()" name="DDNSProvider" id="DDNSProvider" class="mid">
										<option value="none" data-tr="services ddns none"> None </option>
										<option value="dyndns.org"> dyndns.org </option>
										<option value="freedns.afraid.org"> freedns.afraid.org </option>
										<option value="zoneedit.com"> www.zoneedit.com </option>
										<option value="no-ip.com"> www.no-ip.com </option>
									</select>
								</td>
								<td style="text-align: center;">&nbsp;</td>
								<td style="text-align: center;">&nbsp;</td>
							</tr>
							<tr id="div_login">
								<td class="head" data-tr="services l2tp login">Login</td>
								<td colspan="3"><input class="mid" name="Account" id="Account" type="text"></td>
							</tr>
							<tr id="div_password">
								<td class="head" id="manDdnsPasswd">Password</td>
								<td colspan="3"><input class="mid" name="Password" id="Password" type="password"></td>
							</tr>
							<tr id="div_dynname">
								<td class="head" data-tr="services ddns">Dynamic Name</td>
								<td colspan="3"><input class="mid" name="DDNS" id="DDNS" type="text"></td>
							</tr>
						</tbody>
					</table>

					<table class="form" id="div_misc">
						<col style="width: 40%" />
						<col style="width: 47%" />
						<col style="width: 13%" />
						<thead>
							<tr>
								<td class="title" data-tr="services dns misc title"  colspan="3">Miscellaneous</td>
							</tr>
						</thead>
						<tbody>

							<tr id="div_dnssec">
								<td class="head" data-tr="services dns misc dnssec">Enable DNSSEC</td>
								<td>
									<select name="dns_sec" class="mid" onChange="changeDNSSEC(this.form);">
										<option value="0" data-tr="button disable">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select>
								</td>
								<td style="text-align: center">
									<a data-tr="services status about" href="https://en.wikipedia.org/wiki/DNSSEC" target="_blank">Learn more...</a>
								</td>
							</tr>
						</tbody>
					</table>

					<table class="buttons">
						<tr>
							<td>
								<input type="hidden" name="submit-url" value="/services/dns.asp" >
								<input type="submit" class="normal" value="Apply" data-tr="button apply">
								<input type="reset" class="normal" value="Cancel" data-tr="button cancel" onClick="window.location.reload()">
							</td>
						</tr>
					</table>

					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>