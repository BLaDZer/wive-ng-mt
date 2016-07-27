var Profiles = [

	// MANUAL
	[ "manual",					// Profile 
		_("hotspot manual"),		// Provider name
		false,						// IP
		false,						// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		false,						// Primary DNS
		false,						// Secondary DNS
		false,						// Domain name
		false,						// Lease time
		false,						// Radius server auth address
		false,						// Radius server acct address
		false,						// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		false,						// Radius Location Name
		false,						// Radius CoA/DM port
		false,						// No IP check for CoA/DM request
		false,						// UAM server URL
		false,						// UAM homepage URL
		false,						// UAM secret
		false,						// UAM allowed hosts
		false,						// UAM allowed domains
		false,						// Allow all DNS requests
		false,						// Allowed MAC
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true
	],

	// HotSpotSystem
	[   "hotspotsystem",			// Profile 
		"HotSpotSystem.com",		// Provider name
		"192.168.182.0",			// IP
		"255.255.255.0",			// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		"192.168.182.1",			// Primary DNS
		"192.168.182.1",			// Secondary DNS
		"key.chillispot.info",		// Domain name
		false,						// Lease time
		"radius.hotspotsystem.com",	// Radius server auth address
		"radius2.hotspotsystem.com",// Radius server acct address
		false,						// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		false,						// Radius Location Name
		"3779",						// Radius CoA/DM port
		"off",						// No IP check for CoA/DM request
		"https://customer.hotspotsystem.com/customer/hotspotlogin.php",	// UAM server URL
		"",							// UAM homepage URL
		false,						// UAM secret
		"194.149.46.0/24,198.241.128.0/17,66.211.128.0/17,216.113.128.0/17,70.42.128.0/17,128.242.125.0/24,216.52.17.0/24,62.249.232.74,155.136.68.77,155.136.66.34,66.4.128.0/17,66.211.128.0/17,66.235.128.0/17,88.221.136.146,195.228.254.149,195.228.254.152,203.211.140.157,203.211.150.204,www.paypal.com,mobile.paypal.com,www.paypalobjects.com,sstats.paypal-metrics.com,altfarm.mediaplex.com,live.adyen.com,www.worldpay.com,secure.worldpay.com,www.directebanking.com,betalen.rabobank.nl,ideal.ing.nl,ideal.abnamro.nl,www.ing.nl,www.hotspotsystem.com,customer.hotspotsystem.com,tech.hotspotsystem.com,a1.hotspotsystem.com,a2.hotspotsystem.com,a3.hotspotsystem.com,a4.hotspotsystem.com,a5.hotspotsystem.com,a6.hotspotsystem.com,a7.hotspotsystem.com,a8.hotspotsystem.com,a9.hotspotsystem.com,a10.hotspotsystem.com,a11.hotspotsystem.com,a12.hotspotsystem.com,a13.hotspotsystem.com,a14.hotspotsystem.com,a15.hotspotsystem.com,a16.hotspotsystem.com,a17.hotspotsystem.com,a18.hotspotsystem.com,a19.hotspotsystem.com,a20.hotspotsystem.com",  // UAM allowed hosts
		"paypal.com,paypalobjects.com,paypal-metrics.com,mediaplex.com,worldpay.com,adyen.com,hotspotsystem.com,geotrust.com", // UAM allowed domains
		"off",						// Allow all DNS requests
		false,						// Allowed MAC
		//display Elements
		true,
		true,
		false,
		false,
		true,
		true,
		false,
		false,
		false,
		true,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		true,
		true,
		false,
		false,
		false,
		true
	],

	// MyWiFi
	[   "mywifi",					// Profile 
		"MyWifi",					// Provider name
		"192.168.182.0",			// IP
		"255.255.255.0",			// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		"192.168.182.1",			// Primary DNS
		"192.168.182.1",			// Secondary DNS
		"key.chillispot.info",		// Domain name
		false,						// Lease time
		"radiusm.mywifi.com",		// Radius server auth address
		"radiuss.mywifi.com",		// Radius server acct address
		false,						// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		false,						// Radius Location Name
		"3779",						// Radius CoA/DM port
		false,						// No IP check for CoA/DM request
		"http://access.mywifi.com", // UAM server URL
		"",							// UAM homepage URL
		false,						// UAM secret
		"212.118.48.0/24,212.158.173.0/24,91.227.52.0/24,91.227.53.0/24,91.200.28.0/24,91.200.30.0/24,91.232.115.0/26,webmoney.ru,wmtransfer.com,webmoney.com.mx", // UAM allowed hosts
		"login.wmtransfer.com,security.webmoney.ru,access.mywifi.com,webmoney.ru,wmtransfer.com,webmoney.com.mx", // UAM allowed domains
		false,						// Allow all DNS requests
		false,						// Allowed MAC
		//display Elements
		true,
		true,
		false,
		false,
		true,
		true,
		false,
		false,
		false,
		true,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		true,
		false,
		true,
		true,
		false,
		false,
		true,
		true
	],

	// SAIWifi
	[   "saiwifi",					// Profile
		"SAIWifi", 					// Provider name
		"192.168.182.0",			// IP
		"255.255.255.0",			// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		"192.168.182.1",			// Primary DNS
		"192.168.182.1",			// Secondary DNS
		"key.chillispot.info",		// Domain name
		false,						// Lease time
		"176.111.96.30",			// Radius server auth address
		"92.63.105.213",			// Radius server acct address
		false,						// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		false,						// Radius Location Name
		"3779",						// Radius CoA/DM port
		false,						// No IP check for CoA/DM request
		"http://hotspot.saiwifi.ru",// UAM server URL
		"",							// UAM homepage URL
		false,						// UAM secret
		"77.88.8.2,176.111.96.30,176.111.96.13", // UAM allowed hosts
		"stat.saiwifi.ru,hotspot.saiwifi.ru",	 // UAM allowed domains
		false,						// Allow all DNS requests
		false,						// Allowed MAC
		//display Elements
		true,
		true,
		false,
		false,
		true,
		true,
		false,
		false,
		false,
		true,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		true,
		false,
		true,
		true,
		false,
		false,
		true,
		true
	],
	
	// WiFi System
	[	"wifisystem",				// Profile
		"Wi-Fi System",				// Provider name
		"192.168.182.0",			// IP
		"255.255.255.0",			// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		"192.168.182.1",			// Primary DNS
		"192.168.182.1",			// Secondary DNS
		"key.chillispot.info",		// Domain name
		false,						// Lease time
		"radius.wifisystem.ru",		// Radius server auth address
		"radius2.wifisystem.ru",	// Radius server acct address
		"wfs123",					// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		false,						// Radius Location Name
		"3779",						// Radius CoA/DM port
		false,						// No IP check for CoA/DM request
		"https://auth.wifisystem.ru/hotspotlogin", // UAM server URL
		"",							// UAM homepage URL
		false,						// UAM secret
		"95.167.165.64/27,195.14.118.0/23,91.229.116.0/22,213.59.200.64/28,91.200.28.0/24,91.227.52.0/24,198.241.128.0/17,66.211.128.0/17,216.113.128.0/17,paypal.com,paypalobjects.com,paymaster.ru,wifisystem.ru,www.paymaster.ru,www.paypal.com,www.paypalobjects.com,www.wifisystem.ru,auth.wifisystem.ru,support.wifisystem.ru,lk.wifisystem.ru,account.wifisystem.ru,s1.wifisystem.ru,s2.wifisystem.ru,s3.wifisystem.ru,s4.wifisystem.ru,s5.wifisystem.ru,s6.wifisystem.ru,s7.wifisystem.ru,s8.wifisystem.ru,s9.wifisystem.ru,s10.wifisystem.ru,s11.wifisystem.ru,s12.wifisystem.ru,s13.wifisystem.ru,s14.wifisystem.ru,s15.wifisystem.ru,s16.wifisystem.ru,s17.wifisystem.ru,s18.wifisystem.ru,s19.wifisystem.ru,s20.wifisystem.ru", // UAM allowed hosts
		"wifisystem.ru,paymaster.ru,paypal.com,paypalobjects.com", // UAM allowed domains
		false,						// Allow all DNS requests
		false,						// Allowed MAC
		//display Elements
		true,
		true,
		false,
		false,
		true,
		true,
		false,
		false,
		false,
		true,
		false,
		false,
		false,
		true,
		true,
		true,
		false,
		true,
		false,
		true,
		true,
		false,
		false,
		true,
		true
	],
	
	// NetByNet
	[	"netbynet",					// Profile
		"NetByNet",					// Provider name
		false,						// IP
		false,						// Netmask
		"10",						// Start DHCP IP
		"200",						// End DHCP IP
		false,						// Primary DNS
		false,						// Secondary DNS
		"NetNyNEt.lo",					// Domain name
		"3000",						// Lease time
		"wi-fi.netbynet.ru",				// Radius server auth address
		"wi-fi.netbynet.ru",				// Radius server acct address
		false,						// Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		"NetByNet.chilli",				// Radius Location Name
		"3779",						// Radius CoA/DM port
		"off",						// No IP check for CoA/DM request
		"https://wi-fi.netbynet.ru",			// UAM server URL
		false,						// UAM homepage URL
		false,						// UAM secret
		"wi-fi.netbynet.ru,192.168.1.1",		// UAM allowed hosts
		false,						// UAM allowed domains
		"off",						// Allow all DNS requests
		false,						// Allowed MAC
		//display Elements
		true,
		true,
		true,
		true,
		false,
		false,
		true,
		true,
		true,
		false,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		true,
		true,
		false,
		true,
		false,
		true
	],
	
/*	
	//Enforta
	[	"enforta",					// Profile
		"Enforta",					// Provider name
		"192.168.182.0",			// IP
		"255.255.255.0",			// Netmask
		false,						// Start DHCP IP
		false,						// End DHCP IP
		"87.241.223.68",			// Primary DNS
		"192.168.182.1",			// Secondary DNS
		"key.chillispot.info",		// Domain name
		false,						// Lease time
		"185.12.28.167",			// Radius server auth address
		"0.0.0.0",					// Radius server acct address
		"enforta1242-8201-service_36Cloud", // Radius server secret
		false,						// Radius NAS ID
		false,						// Radius Location ID
		"3779",						// Radius CoA/DM port
		false,						// No IP check for CoA/DM request
		"http://hs.enforta.ru",		// UAM server URL
		"",							// UAM homepage URL
		false,						// UAM secret
		"185.12.28.167,87.241.223.68,192.168.182.1", // UAM allowed hosts
		"",							// UAM allowed domains
		false,						// Allow all DNS requests
		false						// Allowed MAC
	] 
*/
];
