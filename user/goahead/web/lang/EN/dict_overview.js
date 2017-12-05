var overview_dictionary = {
	'overview select language'		: 'Select Language',
	'overview status'			: 'Status',
	'overview statistics'			: 'Statistics',
	'overview management'			: 'Management',

	'opmode title'				: 'Operation Mode Configuration',
	'opmode firmware version'		: 'Current Firmware Version: ',
	'opmode introduction'			: 'You can configure the operation mode suitable for your environment.',
	'opmode mode bridge'			: 'AP-Bridge',
	'opmode mode bridge intro'		: 'All Ethernet and Wireless interfaces are combined into a single bridge interface.',
	'opmode mode gateway'			: 'AP-Gateway',
	'opmode mode gateway intro'		: 'The first Ethernet port is treated as WAN port. All other Ethernet ports and the Wireless interface are bridged together and treated as LAN ports.',
	'opmode mode e'				: 'Client-Gateway',
	'opmode mode e intro'			: 'The Wireless interface is treated as WAN port, and Ethernet ports are used as LAN ports.',
	'opmode mode a'				: 'Client-AP-Gateway(WISP)/Client-AP-Bridge(Repeater)',
	'opmode mode a intro'			: 'The Wireless APCli interface is treated as WAN port others ports as LAN (Client-AP-Gateway). Or all interfaces in bridge + NAT2.5 for wireless client connection (Client-AP-Bridge).',
	'opmode mode unknown'			: 'Unknown',
	'opmode confirm'			: 'Changing operation mode needs to reboot you router. Do you want to proceed?'
}

lang_dictionary = Object.assign(lang_dictionary, overview_dictionary);

delete overview_dictionary;
