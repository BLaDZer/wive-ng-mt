var tree_dictionary = {
	'treeapp router'		: 'Router',
	'treeapp operation mode'	: 'Operation Mode',
	'treeapp network settings'	: 'Network Settings',
	'treeapp wireless settings'	: 'Wireless Settings',
	'treeapp firewall'		: 'Firewall',
	'treeapp services'		: 'Services',
	'treeapp usb'			: 'USB services',
	'treeapp storage'		: 'Storage services',
	'treeapp administration'	: 'Administration',

	'treeapp lan'			: 'LAN setup',
	'treeapp wan'			: 'WAN setup',
	'treeapp ipv6'			: 'IPv6 setup',
	'treeapp vpn'			: 'VPN setup',
	'treeapp l2tunnels'		: 'L2 Tunnels',
	'treeapp vlan'			: 'VLAN setup',
	'treeapp routing'		: 'Routing',
	'treeapp hotspot'		: 'Hotspot',
	'treeapp qos'			: 'QoS',
	'treeapp ethernet'		: 'Switch setup',

	'treeapp profile'		: 'Profile',
	'treeapp link status'		: 'Link Status',
	'treeapp statistics'		: 'Statistics',
	'treeapp advance'		: 'Advance',
	'treeapp main'			: 'Main',
	'treeapp security'		: 'Security',
	'treeapp wds'			: 'WDS',
	'treeapp ap client'		: 'Client/Repeater',
	'treeapp station list'		: 'Station List',

	'treeapp firewall'		: 'Firewall',
	'treeapp alg'			: 'Application Level Gateway',
	'treeapp dmz'			: 'Demilitarized Zone',
	'treeapp content filtering'	: 'Content Filtering',

	'treeapp dhcp server'		: 'DHCP Server',
	'treeapp l2tp server'		: 'L2TP Server',
	'treeapp ntp settings'		: 'Network Time',
	'treeapp ddns settings'		: 'Dynamic DNS',
	'treeapp samba'			: 'Samba/WINS',
	'treeapp accounting'		: 'IP Accounting',
	'treeapp radius'		: 'RADIUS Server',
	'treeapp cwmp'			: 'CWMP (TR-069)',
	'treeapp miscellaneous'		: 'Miscellaneous',

	'treeapp printersrv'		: 'Printer Server',
	'treeapp usbmodem'		: 'USB Modem',

	'treeapp disk'			: 'Disk Management',
	'treeapp ftpsrv'		: 'FTP Server',
	'treeapp transmission'		: 'Torrent client',
	'treeapp sambasrv'		: 'SAMBA Server',

	'treeapp management'		: 'Management',
	'treeapp status'		: 'Status',
	'treeapp system log'		: 'System Log',
	'treeapp sdk history'		: 'Version History',
	'treeapp reboot'		: 'Save and reboot',
	'treeapp reboot confirm'	: 'Do you really want to reboot your router?',

	'treeapp logout'		: 'Logout'
}

lang_dictionary = Object.assign(lang_dictionary, tree_dictionary);

delete tree_dictionary;