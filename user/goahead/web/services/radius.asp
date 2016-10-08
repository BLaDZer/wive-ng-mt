<!DOCTYPE html>
<html>
	<head>
		<title>Radius Server Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var users = [ <% getRadiusUserList(); %> ];		// Login/sharedsecret list


			// Set translation
			function initTranslation()
			{
				_TR("radiusServerTitle",		"services radius title");
				_TR("radiusServerIntroduction",	"services radius introduction");
				_TR("radiusServerSettings",		"services radius settings");
				_TR("radiusEnabled",			"services radius enable");
				_TR("radiusEnable",				"button enable");
				_TR("radiusDisable",			"button disable");
				
				_TRV("radiusApply",				"button apply");
				_TRV("radiusCancel",			"button cancel");
				_TRV("radiusReset",				"button reset");
			}

			// Set inintal values
			function initValues()
			{
				document.getElementById('radius_srv_enabled').value = ('<% getCfgGeneral(1, "radius_srv_enabled"); %>' == '1') ? 1 : 0;
				radiusEnableSwitch();
				genRadiusTable();
				displayServiceStatus();
				showWarning();
				initTranslation();
			}

			// Check values
			function CheckValues()
			{
				ajaxShowTimer(document.radiusConfig, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function genRadiusTable()
			{
				var table, i;

				table  = '<table class="form">';
				table += '<tr><td class="title" colspan="3">' + _("services radius users") + '</td></tr>';
				table += '<tr><th style="width: 40%;">' + _("services radius login") + '</th><th style="width: 40%;">' + _("services radius sharedsecret") + '</th><th>' + _("services radius action") + '</th></tr>';

				for (i = 0; i < users.length; i++) {
					table += '<tr><td style="padding-left: 20px;">' + users[i][0] + '<input name="radius_srv_user' + i + '" type="hidden" value="' + users[i][0] + '"></td>';
					table += '<td style="padding-left: 20px;">xxxx<input name="radius_srv_pass' + i + '" type="hidden" value="' + users[i][1] + '"></td>';
					table += '<td style="text-align: center;">';
					table += '<img src="/graphics/edit.png" title="' + _("services dhcp edit record") + '" onClick="editUser(' + i + ');"> ';
					table += '<img src="/graphics/cross.png" title="' + _("services dhcp delete record") + '" onClick="deleteUser(' + i + ');">';
					table += '</td></tr>';
				}

				table += '<tr><td><input type="text" id="radiusLogin" maxlength="32" value="" style="width: 99%"></td>';
				table += '<td><input type="password" id="radiusSharedSecret" maxlength="32" value="" style="width: 99%"></td>';
				table += '<td style="text-align: center;">';
				table += '<input type="button" class="normal" title="' + _("services dhcp add record") + '" value="' + _("button add edit") + '" onclick="addUser(this.form);">';
				table += '<input type="hidden" id="radiusEdit" value="-1">';
				table += '<input type="hidden" name="radius_srv_user_num" value="' + users.length + '">';
				table += '</td></tr>';
				table += '</table>';

				document.getElementById("radiusUserList").innerHTML = table;
			}

			// Add user/password
			function addUser()
			{
				var index	= document.getElementById('radiusEdit').value;
				var login	= document.getElementById('radiusLogin').value;
				var pass	= document.getElementById('radiusSharedSecret').value;

				// Check login
				var re_login = /^[a-zA-Z0-9_]+$/;
				if (!re_login.test(login)) {
					alert(_("services radius uncorrect login"));
					document.getElementById('radiusLogin').select();
					document.getElementById('radiusLogin').focus();
					return false;
				}

				// Check login exist
				for (var i = 0; i < users.length; i++) {
					if (login == users[i][0] && index != i) {
						alert(_("services radius exist login"));
						document.getElementById('radiusLogin').select();
						document.getElementById('radiusLogin').focus();
						return false;
					}
				}

				// Check pass
				var re_pass = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
				if (!re_pass.test(pass)) {
					alert(_("services radius uncorrect sharedsecret"));
					document.getElementById('radiusSharedSecret').select();
					document.getElementById('radiusSharedSecret').focus();
					return false;
				}	
				
				if (index == -1)
					users.push([ login, pass ]);
				else {
					var index = document.getElementById('radiusEdit').value;
					users[index][0] = document.getElementById('radiusLogin').value;
					users[index][1] = document.getElementById('radiusSharedSecret').value;
				}
				genRadiusTable();
			}

			// Delete user/password
			function deleteUser(index)
			{
				users.splice(index, 1);
				genRadiusTable();
			}

			// Edit user/password
			function editUser(index)
			{
				document.getElementById('radiusLogin').value		= users[index][0];
				document.getElementById('radiusSharedSecret').value	= users[index][1];
				document.getElementById('radiusEdit').value		= index;
			}

			// Show/hide radius settings
			function radiusEnableSwitch()
			{
				displayElement([ 'radiusUserList' ], document.getElementById('radius_srv_enabled').value == 1);
				genRadiusTable();
			}

			// Display server status
			function displayServiceHandler(response)
			{
				var daemons = response.split(',');

				if ('<% getCfgGeneral(1, "radius_srv_enabled"); %>' == '0' || '<% getCfgGeneral(1, "radius_srv_enabled"); %>' == '')
					document.getElementById('radius_status').innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
				else
					document.getElementById('radius_status').innerHTML = (daemons.indexOf('radiusd') >= 0) ?
						'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
						'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';

				setTimeout('displayServiceStatus();', 5000);
			}

			// Get server status
			function displayServiceStatus()
			{
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"></tr>
			<tr>
				<td><h1 id="radiusServerTitle">RADIUS Server Setup</h1>
					<p id="radiusServerIntroduction">Here you can configure RADIUS server settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="radiusConfig" action="/goform/radiusConfig">
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="radiusServerSettings">RADIUS Server Settings</td>
								</tr>
								<tr>
									<td class="head" id="radiusEnabled">RADIUS server</td>
									<td>
										<select id="radius_srv_enabled" name="radius_srv_enabled" class="mid" onChange="radiusEnableSwitch();">
											<option value="0" id="radiusDisable">Disable</option>
											<option value="1" id="radiusEnable">Enable</option>
										</select>
									</td>
									<td id="radius_status" style="text-align: center;">&nbsp;</td>
								</tr>
							</tbody>
						</table>
						<div id="radiusUserList"> </div>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" id="radiusApply"  value="Apply"  onClick="return CheckValues();">&nbsp;&nbsp;
									<input type="button" class="normal" id="radiusCancel" value="Cancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" id="radiusReset"  value="Reset"  onClick="resetValues(this.form);">
									<input value="0" name="reset" type="hidden">
								</td>
							</tr>
						</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
