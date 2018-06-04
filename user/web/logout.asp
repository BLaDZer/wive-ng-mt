<!DOCTYPE html>
<html>
	<head>
		<title>Logout</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
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


			function deleteCookie(name) {
				setCookie(name, "", { expires: -1 });
			}

			function initValues() {
				deleteCookie("sessionid");
				window.location.replace("/");
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<table class="body">
                    <% webLogout(); %>
		</table>
	</body>
</html>
