var internet_dictionary = {
	'inet apply'			: 'Apply',
	'inet cancel'			: 'Cancel',
	'inet enable'			: 'Enable',
	'inet disable'			: 'Disable',
	'inet mac'			: 'MAC Address',
	'inet ip'			: 'IP Address',
	'inet domain'			: 'Domain Name',
	'inet server'			: 'Server IP',
	'inet netmask'			: 'Subnet Mask',
	'inet gateway'			: 'Default Gateway',
	'inet pri dns'			: 'Primary DNS Server',
	'inet sec dns'			: 'Secondary DNS Server',
	'inet user'			: 'User Name',
	'inet password'			: 'Password',
	'inet pass2'			: 'Verify Password',
	'inet hostname'			: 'Hostname',
	'inet invalid ip'		: 'You have entered invalid IP address',

	'wan title'			: 'Wide Area Network (WAN) Settings',
	'wan introduction'		: 'You can choose connection type suitable for your environment. Also, you can configure parameters according to the selected connection type.',
	'wan connection type'		: 'WAN Connection Type:',
	'wan connection type static'	: 'STATIC (Fixed IP)',
	'wan connection type dhcp'	: 'DHCP (Auto Config)',
	'wan connection type pppoe'	: 'PPPoE (ADSL)',
	'wan connection type l2tp'	: 'L2TP',
	'wan connection type pptp'	: 'PPTP',
	'wan protocol opmode'		: 'Operation Mode',
	'wan protocol opmode keepalive'	: 'Keep-Alive',
	'wan protocol opmode ondemand'	: 'On-Demand',
	'wan protocol opmode manual'	: 'Manual',
	'wan static mode'		: 'Static Mode',
	'wan dhcp mode'			: 'DHCP Mode',
	'wan pppoe mode'		: 'PPPoE Mode',
	'wan l2tp mode'			: 'L2TP Mode',
	'wan pptp mode'			: 'PPTP Mode',
	'wan address mode'		: 'Address Mode',
	'wan address mode static'	: 'Static',
	'wan address mode dynamic'	: 'Dynamic',
	'wan mac clone'			: 'MAC Clone',

	'lan dhcp type'			: 'DHCP Type',
	'lan dhcp type server'		: 'Server',
	'lan dhcp start'		: 'Start IP Address',
	'lan dhcp end'			: 'End IP Address',
	'lan dhcp lease'		: 'Lease Time',
	'lan dhcp static'		: 'Statically Assigned',
	'lan stp'			: '802.1d Spanning Tree',
	'lan arpp'			: 'ARP Proxy (parprouted)',
	'lan cdp'			: 'Link Layer Cisco Discovery (cdp)',
	'lan lltd'			: 'Link Layer Topology Discovery (lltd)',
	'lan lldpd'			: 'Link Layer Discovery Daemon (lldpd)',
	'lan snmp'			: 'SNMP daemon (snmpd)',
	'lan igmpp'			: 'Multicast route (igmpproxy)',
	'lan upnp'			: 'Auto portmap service (miniupnpd)',

	'dhcp title'			: 'DHCP Client List',
	'dhcp introduction'		: 'You can monitor DHCP clients here.',
	'dhcp clients'			: 'DHCP Clients',
	'dhcp expire'			: 'Expires in',

	'vpn title'			: 'VPN Passthrough',
	'vpn introduction'		: 'VPN passthrough configurations including: L2TP, IPSec, and PPTP passthrough.',
	'vpn pass thru'			: 'VPN Pass Through',
	'vpn l2tp pass thru'		: 'L2TP Passthrough',
	'vpn ipsec pass thru'		: 'IPSec Passthrough',
	'vpn pptp pass thru'		: 'PPTP Passthrough',

	'l2 tunnels title'		: 'L2 Tunnels Setup',
	'l2 tunnels introduction'	: 'Here you can configure L2 tunnels settings.',
	'l2 tunnels config'		: 'L2 Tunnels Setup',
	'l2 tunnels custom brifs'	: 'Custom:',
	'l2 tunnels all ifaces'		: 'All interfaces',

	'eoip config'			: 'EOIP Setup',
	'eoip enable'			: 'EOIP',
	'eoip local'			: 'Local IP to use',
	'eoip remote'			: 'Remote IP',
	'eoip tid'			: 'Tunnel ID',
	'eoip brifs'			: 'Tunneling bridged interfaces',
	'eoip invalid tid'		: 'Invalid Tunnel ID!',
	'eoip invalid brifs'		: 'Invalid Bridge Interfaces!',
	'eoip brifs duplicate'		: 'Bridged Interfaces have duplicates!',
	'eoip brifs in use'		: 'Bridged Interfaces are already in use in another tunnel!',

	'l2tpv3 config'			: 'L2TPv3 Setup',
	'l2tpv3 enable'			: 'L2TPv3',
	'l2tpv3 tid'			: 'Tunnel ID',
	'l2tpv3 ptid'			: 'Peer Tunnel ID',
	'l2tpv3 sid'			: 'Session ID',
	'l2tpv3 psid'			: 'Peer Session ID',
	'l2tpv3 source port'		: 'Source Port',
	'l2tpv3 dest port'		: 'Destination Port',
	'l2tpv3 brifs'			: 'Tunneling bridged interfaces',
	'l2tpv3 invalid id'		: 'Invalid identifier!',
	'l2tpv3 invalid port'		: 'Invalid port specified!',
	'l2tpv3 invalid brifs'		: 'Invalid Bridged Interfaces!',
	'l2tpv3 brifs duplicate'	: 'Bridged Interfaces have duplicates!',
	'l2tpv3 brifs in use'		: 'Bridged Interfaces are already in use in another tunnel!',

	'vlan vlanid exist'		: 'VLAN ID already exist',
	'vlan invalid vlanid'		: 'Entered invalid VLAN ID',
	'vlan invalid vlan range'	: 'Invalid VLAN range',
	'vlan reboot confirm'		: 'For apply settings needs to reboot you router. Do you want to proceed?',
	'vlan reset confirm'		: 'Settings will be reset to factory defaults and router will be rebooted. Continue?',
	'vlan tvsip title'		: 'TV/SIP Port Settings',
	'vlan tvsip introduction'	: 'Section to configure VLAN on TV/STB/SIP port',
	'vlan tvsip tv stb'		: 'TV/STB/SIP Port 1',
	'vlan tvsip tv stb mcast'	: 'Mcast proxy',
	'vlan tvsip sip stb'		: 'TV/STB/SIP Port 2',
	'vlan tvsip sip stb mcast'	: 'Mcast proxy',
	'vlan tvsip status title'	: 'TV/STB/SIP VLAN Status',
	'vlan tvsip status port'	: 'Port',
	'vlan tvsip vlanid prio'	: 'PRIO:',
	'vlan tvsip need tv vlan'	: 'To work correctly, you must create one or more VLAN on the TV/STB/SIP Port 1',
	'vlan tvsip need sip vlan'	: 'To work correctly, you must create one or more VLAN on the TV/STB/SIP Port 2',
	'vlan edit record'		: 'Edit',
	'vlan delete record'		: 'Delete',
	'vlan action'			: 'Action',
	'vlan vlan'			: 'VLAN',
	'vlan vlanid'			: 'VLAN ID:',
	'vlan add button'		: 'Add / Edit',
	'vlan mode title'		: 'WLAN VLAN / LAN VLAN',
	'vlan mode introduction'	: 'This section is used to configure the WLAN VLAN / LAN VLAN on Router',
	'vlan mode settings'		: 'WLAN/LAN VLAN Settings',
	'vlan mode select'		: 'Mode',
	'vlan mode select disable'	: 'Disable',
	'vlan mode select wlan'		: 'WLAN VLAN',
	'vlan mode select lan'		: 'LAN VLAN',
	'vlan mode wlan ap'		: 'AP',
	'vlan mode wlan iface'		: 'Interface',
	'vlan mode lan isolated'	: 'Isolated',
	'vlan mode lan isolate untagged': 'Isolate untagged traffic',
	'vlan mode lan add'		: 'Add / Edit',
	'vlan mode lan table'		: 'LAN VLAN Status',
	'vlan mode lan table isolated'	: 'Isolated',
	'vlan mode wlan table'		: 'WLAN VLAN Status',
	'vlan mode wlan table ap'	: 'AP',
	'vlan mode wlan table iface'	: 'Interface',

	'lan title'			: 'Local Area Network (LAN) Settings',
	'lan introduction'		: 'You can configure different network services here.',
	'lan setup'			: 'LAN Setup',
	'lan accept dhcp opts'		: 'Apply change network settings to dhcpd?',

	'inet hostname'			: 'Hostname',
	'inet mac'			: 'MAC Address',
	'inet ip'			: 'IP Address',
	'inet netmask'			: 'Subnet Mask',
	'inet gateway'			: 'Default Gateway',
	'inet pri dns'			: 'Primary DNS Server',
	'inet sec dns'			: 'Secondary DNS Server',
	'inet invalid mtu'		: 'Invalid MTU value',
	'inet invalid mac'		: 'Invalid MAC address',
	'inet auto'			: 'Auto',
	'inet custom'			: 'Custom',
	'inet hostname wrong'		: 'Enter valid Hostname',
	'inet lan wan same'		: 'LAN IP address is identical to WAN',
	'inet lan gw same'		: 'LAN IP address is identical to Gateway',
	'inet wan gw same'		: 'WAN IP address is identical to Gateway',
	'inet dns profile'		: 'DNS profile',
	'inet dns profile manual'	: 'Manual',
	'inet dns profile google'	: 'Google DNS',
	'inet dns profile yandex'	: 'Yandex DNS',
	'inet dns profile sky'		: 'Sky DNS',
	'inet dns profile open'		: 'Open DNS',
	'inet dns profile adguard'	: 'AdGuard DNS',
	'inet dns profile dhcp'		: 'Auto (DHCP)',
	'inet dns profile yandex title'	: 'Yandex DNS profile',
	'inet dns profile yandex basic'	: 'Basic',
	'inet dns profile yandex safe'	: 'Safe',
	'inet dns profile yandex family': 'Family',
	'inet dns profile adguard title': 'AdGuard DNS profile',
	'inet dns profile adguard default'	: 'Default',
	'inet dns profile adguard family'	: 'Family',

	'wan title'			: 'Wide Area Network (WAN) Settings',
	'wan introduction'		: 'You can choose connection type suitable for your environment. Also, you can configure parameters according to the selected connection type.',
	'wan connection'		: 'WAN Connection',
	'wan connection type'		: 'WAN Connection Type:',
	'wan connection type static'	: 'STATIC (Fixed IP)',
	'wan connection type dhcp'	: 'DHCP (Auto Config)',
	'wan connection type zero'	: 'Zeroconf',
	'wan static mode'		: 'Static Mode',
	'wan dhcp mode'			: 'DHCP Mode',
	'wan additional options'	: 'Additional Options',
	'wan request from dhcp'		: 'Request IP from DHCP (optional)',
	'wan dhcp vendor class'		: 'Vendor class identifier (optional)',
	'wan mtu'			: 'WAN MTU',
	'wan nat enabled'		: 'Enable NAT',
	'wan mac'			: 'WAN MAC address',
	'wan reboot confirm'		: 'Changing new MAC needs to reboot you router. Do you want to proceed?',
	'wan warning same lan'		: 'WAN IP address is identical to LAN',

	'vpn title'			: 'Virtual Private Network setup',
	'vpn introduction'		: 'This page is used to configure the VPN(Virtual Private Network) tunnel on your Router. ',
	'vpn config'			: 'VPN configuration',
	'vpn enable'			: 'Enable VPN',
	'vpn mode'			: 'VPN Mode:',
	'vpn pppoe'			: 'PPPoE client',
	'vpn pptp'			: 'PPTP client',
	'vpn l2tp'			: 'L2TP client',
	'vpn pppoe iface'		: 'PPPoE interface:',
	'vpn server col'		: 'Host, IP, AC or APN name:',
	'vpn server col ac'		: 'AC name:',
	'vpn server col host'		: 'Host, IP or DNS name:',
	'vpn server col apn'		: 'APN:',
	'vpn service name'		: 'Service name:',
	'vpn auth type'			: 'Authentication method:',
	'vpn lanauth lvl'		: 'KABINET access level:',
	'vpn kabinet offline'		: 'offline',
	'vpn kabinet'			: 'kabinet',
	'vpn kabinet full'		: 'full',
	'vpn username'			: 'Username',
	'vpn password'			: 'Password',
	'vpn mtu mru'			: 'MTU/MRU',
	'vpn enabled'			: 'Enabled',
	'vpn disabled'			: 'Disabled',
	'vpn lcp interval'		: 'LCP echo interval:',
	'vpn lcp failure'		: 'LCP echo failure:',
	'vpn allow mppe'		: 'Allow MPPE',
	'vpn peer dns'			: 'Peer DNS',
	'vpn allow debug'		: 'Allow debug',
	'vpn enable nat'		: 'Enable NAT',
	'vpn adaptive lcp'		: 'Adaptive LCP',
	'vpn pure pppoe'		: 'Pure PPPoE',
	'vpn test server'		: 'Test server reachable',
	'vpn alert password'		: 'The password you entered is empty or contains invalid characters! Non-ASCII characters, spaces and certain special characters are prohibited.',
	'vpn alert username'		: 'The username you entered is empty or contains invalid characters! Username may only contain alphanumeric characters and underscores.',
	'vpn alert invalid ip'		: 'Invalid IP address or domain name!',
	'vpn kabinet auth'		: 'KABINET Authorization',
	'vpn status disabled'		: 'disabled',
	'vpn status not started'	: 'not started',
	'vpn status offline'		: 'offline',
	'vpn status connecting'		: 'connecting',
	'vpn status online'		: 'online',
	'vpn status kabinet networks'	: 'kabinet networks',
	'vpn status full access'	: 'full access',

	'ipv6 title'			: 'IPv6 Setup',
	'ipv6 introduction'		: '',
	'ipv6 connection type'		: 'IPv6 Connection Type',
	'ipv6 operation mode'		: 'IPv6 Operation Mode',
	'ipv6 static'			: 'Native dynamic/static IP Connection',
	'ipv6 dhcp6c'			: 'IPv6 autoconfigure by DHCP/RA',
	'ipv6 6rd'			: 'Tunneling Connection (6RD)',
	'ipv6 6to4'			: 'Tunneling Connection (6to4/6in4)',
	'ipv6 allow forward'		: 'Allow access to LAN from internet',
	'ipv6 static ip'		: 'IPv6 Static IP Setup',
	'ipv6 static lan address'	: 'LAN IPv6 Address / Subnet Prefix Length',
	'ipv6 static wan address'	: 'WAN IPv6 Address / Subnet Prefix Length',
	'ipv6 static gateway'		: 'Default Gateway',
	'ipv6 static dns primary'	: 'Primary DNS Address',
	'ipv6 static dns secondary'	: 'Secondary DNS Address',
	'ipv6 6rd enabled'		: 'Tunneling Connection (6RD) Setup',
	'ipv6 6rd prefix'		: 'ISP 6rd Prefix / Prefix Length',
	'ipv6 6rd relay address'	: 'ISP Border Relay IPv4 Address',
	'ipv6 6to4 isp prefix'		: 'Use ISP IPv6 Prefix',
	'ipv6 6to4 prefix'		: 'ISP 6to4 Prefix / Prefix Length',
	'ipv6 6to4 enabled'		: 'Tunneling Connection (6to4) Setup',
	'ipv6 6to4 server address'	: 'IPv4 to IPv6 server address',
	'ipv6 Ipv6InVPN'		: 'IPv6 uplink over VPN',
	'ipv6 iana disable'		: 'Do not use ia-na for WAN configuration',
	'ipv6 services'			: 'IPv6 Services',
	'ipv6 service name'		: 'Service name',
	'ipv6 service value'		: 'Value',
	'ipv6 service status'		: 'Status',
	'ipv6 radvd'			: 'Router Advertisement(radvd)',
	'ipv6 dhcp6s'			: 'Dynamic IPv6 configuration(dhcp6s)',
	'ipv6 ip empty'			: 'IP address is empty.',
	'ipv6 number'			: 'It should be a [0-9] number.',
	'ipv6 ip format error'		: 'IP address format error.',
	'ipv6 hex'			: 'It should be a [0-F] number.',
	'ipv6 invalid addr'		: 'Invalid IPv6 address!',
	'ipv6 invalid prefix'		: 'Invalid prefix length!',
	'ipv6 fill all'			: 'Please fill all fields!',
	'ipv6 invalid ipv4'		: 'Invalid IPv4 address!',
	'ipv6 mtu'			: 'MTU',

	'qos title'			: 'Quality of Service Settings',
	'qos introduction'		: 'Here you can setup rules to provide desired Quality of Service for specific applications.',
	'qos warning'			: 'For correct operation of QoS need turning off NAT offload mode. Please note that turning off NAT offload mode will very increase CPU usage.',
	'qos setup'			: 'QoS Setup',
	'qos type'			: 'Type of QoS',
	'qos bandwidth settings'	: 'Bandwidth settings',
	'qos upload rate'		: 'Upload rate:',
	'qos upload limit'		: 'Upload rate limit:',
	'qos download rate'		: 'Download rate:',
	'qos download limit'		: 'Download rate limit:',
	'qos upload vpn rate'		: 'Upload VPN rate:',
	'qos upload vpn limit'		: 'Upload VPN rate limit:',
	'qos upload mdm rate'		: 'Upload modem rate:',
	'qos upload mdm limit'		: 'Upload modem rate limit:',
	'qos priority port'		: 'Priority port settings',
	'qos high priority ports'	: 'User High Priority Ports',
	'qos low priority ports'	: 'User Low Priority Ports',
	'qos priority dscp'		: 'Priority DSCP settings',
	'qos high priority dscps'	: 'User High Priority DSCPs',
	'qos low priority dscps'	: 'User Low Priority DSCPs',
	'qos simple'			: 'Simple priority based QoS',
	'qos shaper'			: 'Complex shaper (ports and DSCP based mark + HTB limit)',
	'qos codel'			: 'Automatic Fair Queue Controlled Delay AQM (CODEL)',
	'qos mode'			: 'Shape from:',
	'qos mode all'			: 'All',
	'qos mode wan'			: 'Only WAN',
	'qos mode vpn'			: 'Only VPN',
	'qos invalid bw'		: 'Entered wrong value.',

	'routing title'			: 'Routing Settings',
	'routing introduction'		: 'You can add and remove custom Internet routing rules and/or enable dynamic routing exchange protocol here.',
	'routing add rule'		: 'Add a static routing rule',
	'routing dest'			: 'Destination',
	'routing range'			: 'Range',
	'routing host'			: 'Host',
	'routing net'			: 'Net',
	'routing next hop'		: 'Next Hop',
	'routing netmask'		: 'Netmask',
	'routing gateway'		: 'Gateway',
	'routing interface'		: 'Interface',
	'routing interface name'	: 'Interface Name',
	'routing custom'		: 'Custom',
	'routing comment'		: 'Comment',
	'routing dynamic title'		: 'Dynamic Routing Support',
	'routing rip'			: 'Support RIP protocol',
	'routing del title'		: 'Current Routing table:',
	'routing del title ipv6'	: 'Current IPv6 Routing table:',
	'routing number'		: 'No.',
	'routing del flags'		: 'Flags',
	'routing del metric'		: 'Metric',
	'routing del ref'		: 'Ref',
	'routing del use'		: 'Use',
	'routing action'		: 'Action',
	'routing unsup chars'		: 'Unsupported character in comment',

	'hotspot title'			: 'Hotspot settings',
	'hotspot introduction'		: '',
	'hotspot profile'		: 'Profile name',
	'hotspot manual'		: 'Manual',
	'hotspot setup'			: 'Hotspot Setup',
	'hotspot type'			: 'Hotspot type',
	'hotspot domain'		: 'Domain name',
	'hotspot https redirect'	: 'HTTPS Redirect',
	'hotspot drop dns'		: 'DNS paranoia',
	'hotspot drop dns full'		: 'Drop DNS responses (pre-authentication) containing any non- A, CNAME, SOA, or MX records',
	'hotspot clisolate'		: 'Use /32 mask for clients',
	'hotspot clisolate full'	: 'Use /32 mask for clients (prevent client to client direct connect)',
	'hotspot radius auth'		: 'Radius server auth address',
	'hotspot radius acct'		: 'Radius server acct address',
	'hotspot radius secret'		: 'Radius server secret',
	'hotspot radius nas'		: 'Radius NAS ID',
	'hotspot radius location id'	: 'Radius Location ID',
	'hotspot radius location name'	: 'Radius Location Name',
	'hotspot radius coa port'	: 'Radius CoA/DM port',
	'hotspot radius no check ip'	: 'No IP check for CoA/DM request',
	'hotspot uam server'		: 'UAM server URL',
	'hotspot uam homepage'		: 'UAM homepage URL',
	'hotspot uam secret'		: 'UAM secret',
	'hotspot uam allowed'		: 'UAM allowed hosts',
	'hotspot uam domain'		: 'UAM allowed domains',
	'hotspot uam any dns'		: 'Allow all DNS requests',
	'hotspot uam mac'		: 'Allowed MAC',
	'hotspot dont space'		: 'Space in field is not allowed!',
	'hotspot expects number'	: 'This field expects number!',
	'hotspot endip low'		: 'End IP must be greater or equal to Start IP',
	'hotspot mac list'		: 'Please correctly fill out the list of MAC addresses!',
	'hotspot max clients'		: 'Maximum clients',
	'hotspot max clients range'	: 'Allowed range is 50-2000',
	'hotspot nodog ip range'	: 'Range of IP addresses',
	'hotspot nodog url'		: 'Redirected to URL after authentication',
	'hotspot nodog max clients'	: 'Maximum number of users',
	'hotspot nodog idle timeout'	: 'Inactivity before a user is \'deauthenticated\'',
	'hotspot nodog force timeout'	: 'Minutes before a user is \'deauthenticated\'',
	'hotspot nodog auth'		: 'Authenticate Immediately',
	'hotspot nodog mechanism'	: 'MAC Mechanism',
	'hotspot nodog blocked list'	: 'List Blocked MAC Addresses',
	'hotspot nodog allowed list'	: 'List Allowed MAC Addresses',
	'hotspot nodog trusted list'	: 'List Trusted MAC Addresses',
	'hotspot nodog pass'		: 'Password Authentication',
	'hotspot nodog user'		: 'Username Authentication',
	'hotspot nodog pass attempts'	: 'Password Attempts',
	'hotspot nodog block'		: 'Block',
	'hotspot nodog allow'		: 'Allow',
	'hotspot profile change'	: 'When changing profile recommended to reset the hotspot settings. Reset settings?',

	'ethernet title'		: 'Ethernet Port Settings',
	'ethernet introduction'		: 'You can set up ethernet port',
	'ethernet reboot confirm'	: 'For apply settings needs to reboot you router. Reboot router?',
	'ethernet reset confirm'	: 'Settings will be reset to factory defaults and router will be rebooted. Continue?',
	'ethernet port management'	: 'Ethernet Port Settings',
	'ethernet wan port'		: 'WAN port',
	'ethernet lan port'		: 'First LAN port',
	'ethernet lan port near'	: 'Near to the WAN port',
	'ethernet lan port distant'	: 'Distant from the WAN port',
	'ethernet port 1 mode'		: 'Port 1 mode',
	'ethernet port 2 mode'		: 'Port 2 mode',
	'ethernet port 3 mode'		: 'Port 3 mode',
	'ethernet port 4 mode'		: 'Port 4 mode',
	'ethernet port 5 mode'		: 'Port 5 mode',
	'ethernet port mode auto'	: 'Auto-Negotiation',
	'ethernet port mode 10h'	: '10Mbit/s Half-Duplex',
	'ethernet port mode 10f'	: '10Mbit/s Full-Duplex',
	'ethernet port mode 100h'	: '100Mbit/s Half-Duplex',
	'ethernet port mode 100f'	: '100Mbit/s Full-Duplex',
	'ethernet port mode 1000h'	: '1Gbit/s Half-Duplex',
	'ethernet port mode 1000f'	: '1Gbit/s Full-Duplex',
	'ethernet port status head'	: 'Ethernet Ports Status',
	'ethernet port status'		: 'Ports Status',
	'ethernet port rx bytes'	: 'RX Bytes',
	'ethernet port tx bytes'	: 'TX Bytes',
	'ethernet gibits'		: ' GiB',
	'ethernet mibits'		: ' MiB',
	'ethernet kibits'		: ' KiB',
	'ethernet port fcmode name'	: 'Flow Control: ',
	'ethernet port fcmode auto'	: 'Auto',
	'ethernet port fcmode tx'	: 'TX',
	'ethernet port fcmode rx'	: 'RX',
	'ethernet port fcmode txrx'	: 'TX+RX',
	'ethernet port fcmode off'	: 'Off',
	'ethernet port fcmode label auto'	: 'Full Flow Control (RX, TX and Auto-Negotiation)',
	'ethernet port fcmode label tx'		: 'TX only Flow Control',
	'ethernet port fcmode label rx'		: 'RX only Flow Control',
	'ethernet port fcmode label txrx'	: 'TX and RX Flow Control',
	'ethernet port fcmode label off'	: 'All Flow Control Off'
}

lang_dictionary = Object.assign(lang_dictionary, internet_dictionary);

delete internet_dictionary;