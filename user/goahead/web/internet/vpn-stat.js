var status = '<% vpnShowVPNStatus(); %>';
var message;
switch(status) {
	case 'disabled':			message = '<b><font color="#808080">' + ((NVRAM_Language == 'ru') ? 'выключен' : 'disabled') + '</font></b>';					break;
	case 'not started':			message = '<b><font color="#808080">' + ((NVRAM_Language == 'ru') ? 'не запущен' : 'not started') + '</font></b>';				break;
	case 'offline':				message = '<b><font color="#FF0000">' + ((NVRAM_Language == 'ru') ? 'не в сети' : 'offline') + '</font></b>';					break;
	case 'connecting':			message = '<b><font color="#FF8000">' + ((NVRAM_Language == 'ru') ? 'соединение' : 'connecting') + '</font></b>';				break;
	case 'online':				message = '<b><font color="#00FF00">' + ((NVRAM_Language == 'ru') ? 'в сети' : 'online') + '</font></b>';						break;
	case 'kabinet networks':	message = '<b><font color="#33BB33">' + ((NVRAM_Language == 'ru') ? 'сеть Кабинет' : 'kabinet networks') + '</font></b>';		break;
	case 'full access':			message = '<b><font color="#00FF00">' + ((NVRAM_Language == 'ru') ? 'полный доступ' : 'full acces') + '</font></b>';			break;
}
ajaxModifyElementHTML('vpn_status_col', message);
