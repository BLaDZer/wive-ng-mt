<!DOCTYPE html>
<html>
	<head>
		<title>Wive-NG-MT</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_login.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>

			function inFrame () {
			    try {
			        return window.self !== window.top;
			    } catch (e) {
			        return true;
			    }
			}

			function initTranslation() {
				_TR("loTitle",			"login title");
				_TR("loUsername",		"login username");
				_TR("loPassword",		"login password");
				_TR("loEnter",			"login enter");
				_TR("loIncorrect",		"login incorrect");
			}

			function initValues() {
				if (inFrame())
				{
					window.parent.location.replace("/login.asp");
				}
				
				initTranslation();
				
				if (getCookie("Wrong-Pass") == "1")
				{
					deleteCookie("Wrong-Pass");
					var elem = document.getElementById("loIncorrect");
					elem.style.display = "inherit";
				}
			}


		</script>
                <style>
body {
    padding: 0;
    margin: 0;
    width: 100%;
}

.title {
    font-size: large;
}

div.container {
    padding-top: 50px;
}

#submit-td {
    text-align: center;
}

table {
    border: 1px solid #aaa;
    width: 320px;
}

table td {
    padding: 10px;
}

td.col1 {
    width: 20%;
}

td.col2 {
    width: 80%;
}

input {
    padding: 5px 2px;
}

#username,#password {
    width: 100%;
    border: 1px solid #aaa;
}

#loEnter {
    width: 100%;
}

#loIncorrect {
    display: none;
    width: 100%;
    color: #d11;
    font-weight: bolder;
}

#loIncorrectTd {
      padding-top: 0;
      padding-bottom: 5px;
}

</style>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
        <div class="container" align=center>
					<!-- ----------------- Login Form ----------------- -->
					<form method="post" name="Lang" action="/goform/auth">
						<table>
							<tr>
								<td class="title" id="loTitle" colspan=2>Sign In</td>
							<tr>
								<td class="col1"><legend id="loUsername">Username</legend></td>
								<td class="col2"><input type="text" name="username" id="username"></td>
							</tr>
							<tr>
								<td class="col1"><legend id="loPassword">Password</legend></td>
								<td class="col2"><input type="password" name="password" id="password"></td>
							</tr>
							<tr>
								<input type="hidden" name="submit-url" value="/" >
								<td colspan=2 id="submit-td"><input type="submit" value="Log In" id="loEnter"></td>
							</tr>
							<tr>
                                                            <td colspan=2 id="loIncorrectTd"><span id="loIncorrect">You have entered incorrect username or password. Please verify your credentials and try again.</span></td>
                                                        </tr>
						</table>
					</form>
	</div>
	</body>
</html>