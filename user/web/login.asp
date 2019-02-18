<!DOCTYPE html>
<html>
	<head>
		<title>Login</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<meta name="viewport" content="width=device-width, initial-scale=1">

		<script>
			var IS_DEMO = '<% isDemoMode(); %>' == '1';
			if (IS_DEMO) {
				(function(m,e,t,r,i,k,a){m[i]=m[i]||function(){(m[i].a=m[i].a||[]).push(arguments)};
				m[i].l=1*new Date();k=e.createElement(t),a=e.getElementsByTagName(t)[0],k.async=1,k.src=r,a.parentNode.insertBefore(k,a)})
				(window, document, "script", "https://mc.yandex.ru/metrika/tag.js", "ym");
			}
		</script>

		<script>
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

			function inFrame () {
			    try {
			        return window.self !== window.top;
			    } catch (e) {
			        return true;
			    }
			}

/*
			function initTranslation() {
				_TR("loTitle",			"login title");
				_TR("loUsername",		"login username");
				_TR("loPassword",		"login password");
				_TR("loEnter",			"login enter");
				_TR("loIncorrect",		"login incorrect");
			}
*/
			function initValues() {
				if (inFrame())
				{
					window.parent.location.replace("/login.asp");
				}
				
//				initTranslation();
				
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
    padding-top: 100px;
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

td {
    font-size: 9pt;
    font-family: Arial, Helvetica, sans-serif;
    color: #333333;
}

td.title {
    margin-left: 5px;
    background-color: #2C5EA4;
    font-weight: bold;
    white-space: nowrap;
    color: #FFFFFF;
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
					<form method="post" name="LoginForm" action="/goform/auth">
						<table>
							<tr>
								<td class="title" id="loTitle" colspan=2>Sign In</td>
							<tr>
								<td class="col1"><legend id="loUsername">Username</legend></td>
								<td class="col2"><input type="text" name="username" id="username" autofocus ></td>
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
	<div id="demoDesc" align="center" style="width: 530px; margin:0 auto; text-align:center; padding-top:10px;"></div>
	<div id="demoMetrics" style="padding-top: 20px;" align="center"></div>

<script>
if (IS_DEMO) {
            document.getElementById("demoDesc").innerHTML = "<b>Демо интерфейс Wive-NG router firmware</b><br>" +
                "Для входа используйте пару логин/пароль Admin/Admin.<br><br>" +
                "Внимание, часть функционала в DEMO UI может работать некорректно или данные могут быть неполными.<br><br>" +
                "Это нормально и вызвано ограничениями самого DEMO UI.";

            var mdiv = document.getElementById("demoMetrics");

            ym(52425745, "init", {
                id:52425745,
                clickmap:true,
                trackLinks:true,
                accurateTrackBounce:true
            });

            var sc = document.createElement('script');
            sc.setAttribute("type", "text/javascript");
            sc.setAttribute("src", "//cdn.clustrmaps.com/map_v2.js?cl=ffffff&w=320&t=tt&d=12kkhpNxOgKWIOeodW82VIL4G8HWYKYchWbiS6x_j0E");
            sc.setAttribute("id", "clustrmaps");
            mdiv.appendChild(sc);

            var div = document.createElement('div');
            div.innerHTML = "<br><a href='//www.liveinternet.ru/click' "+
            "target=_blank><img src='//counter.yadro.ru/hit?t26.11;r"+
            escape(document.referrer)+((typeof(screen)=="undefined")?"":
            ";s"+screen.width+"*"+screen.height+"*"+(screen.colorDepth?
            screen.colorDepth:screen.pixelDepth))+";u"+escape(document.URL)+
            ";h"+escape(document.title.substring(0,150))+";"+Math.random()+
            "' alt='' title='LiveInternet: показано число посетителей за"+
            " сегодня' "+
            "border='0' width='88' height='15'><\/a>";
            mdiv.appendChild(div);
}
</script>

	</body>
</html>