<!DOCTYPE html>
<html>
<head>
<title>Wireless Station About</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script language="JavaScript" type="text/javascript">

</script>
</head>
<body bgcolor="#FFFFFF">
<table class="body">
  <tr>
    <td><h1 id="aboutTitle">Station About</h1>
      <p id="aboutIntroduction">The Status page shows the settings and current operation status of the Station.</p>
      <hr />
      <table width="90%" border="1" cellpadding="2" cellspacing="1">
        <tr>
          <td class="title" colspan="2" id="aboutAbout">About</td>
        </tr>
        <tr>
          <td class="head" id="aboutDriverVersion">Driver Version</td>
          <td><% getStaDriverVer(); %></td>
        </tr>
        <tr>
          <td class="head" id="aboutMacAddr">Mac Address</td>
          <td><% getStaMacAddrw(); %></td>
        </tr>
      </table></td>
  </tr>
</table>
<div class="whitespace">&nbsp;</div>
</body>
</html>
