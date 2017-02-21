var status 		= '<% vpnShowVPNStatus(); %>';
var text		= "<b><font color=\"";
switch(status) {
	case 'disabled':			text += "#808080";	break;
	case 'not started':			text += "#808080";	break;
	case 'offline':				text += "#FF0000";	break;
	case 'connecting':			text += "#FF8000";	break;
	case 'online':				text += "#00FF00";	break;
	case 'kabinet networks':	text += "#33BB33";	break;
	case 'full access':			text += "#00FF00";	break;
}
text += "\">" + status + "</font></b>";
ajaxModifyElementHTML('vpn_status_col', text);	
