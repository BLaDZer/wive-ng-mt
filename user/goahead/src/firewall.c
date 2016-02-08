/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.c,v 1.29.2.1 2009-03-24 08:56:16 yy Exp $
 */

/*
 *	if  WAN or LAN ip changed, we must restart firewall.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#include "utils.h"
#include "firewall.h"
#include "internet.h"
#include "helpers.h"

static int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

static int isIpValid(char *str)
{
	struct in_addr addr;	// for examination
	if( (! strcmp(T("any"), str)) || (! strcmp(T("any/0"), str)))
		return 1;

	if(! (inet_aton(str, &addr))){
		syslog(LOG_ERR, "isIpValid(): %s is not a valid IP address, %s\n", str, __FUNCTION__);
		return 0;
	}
	return 1;
}

static int isNumOnly(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

static int isAllNumAndSlash(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

static int isOnlyOneSlash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}

static int isIpNetmaskValid(char *s)
{
	char str[32];
	char *slash;
	struct in_addr addr;    // for examination

	if (!s || !strlen(s))
		return 0;

	strncpy(str, s, sizeof(str));

	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
		return 1;

	if (!isAllNumAndSlash(str))
		return 0;

	if (!isOnlyOneSlash(str))
		return 0;

	slash = strchr(str, '/');
	if (slash)
	{
		int mask;

		*slash = '\0';
		slash++;
		if(!strlen(slash)){
			return 0;
		}

		if(!isNumOnly(slash)){
			return 0;
		}

		mask = atoi(slash);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if (! (inet_aton(str, &addr)))
	{
		syslog(LOG_ERR, "isIpNetmaskValid(): %s is not a valid IP address, %s\n", str, __FUNCTION__);
		return 0;
	}

	return 1;
}

static int getDMZEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *dmze = nvram_get(RT2860_NVRAM, "DMZEnable");

	if (dmze)
		value = atoi(dmze);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

/*
 *
 */
static void makeIPPortFilterRule(char *buf, int len, char *iface, char *mac_address,
	char *sip, char *sim, int sprf_int, int sprt_int,
	char *dip, char *dim, int dprf_int, int dprt_int,
	int proto, int action, const char *chain)
{
	int rc = 0;
	char *pos = buf;

	rc = snprintf(pos, len, "iptables -A %s ", chain);

	pos += rc;
	len -= rc;

	if (iface != NULL)
	{
		rc = snprintf(pos, len, "-i %s ", iface);
		pos += rc;
		len -= rc;
	}

	// write mac address
	if ((mac_address!=NULL) && (strlen(mac_address)>0))
	{
		rc = snprintf(pos, len, "-m mac --mac-source %s ", mac_address);
		pos += rc;
		len -= rc;
	}

	// write source ip
	if ((sip == NULL) || (strlen(sip)<=0))
		sip = "any/0";

	if ((sim==NULL) || (strlen(sim)==0) || (strcmp(sim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-s %s ", sip);
	else
		rc = snprintf(pos, len, "-s %s/%s ", sip, sim);

	pos += rc;
	len -= rc;

	// write destination ip
	if ((dip == NULL) || (strlen(dip)<=0))
		dip = "any/0";

	if ((dim==NULL) || (strlen(dim)==0) || (strcmp(dim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-d %s ", dip);
	else
		rc = snprintf(pos, len, "-d %s/%s ", dip, dim);

	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_ICMP)
	{
		rc = snprintf(pos, len, "-p icmp ");
		pos += rc;
		len -= rc;
	}
	else if ((proto == PROTO_TCP) || (proto == PROTO_UDP))
	{
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else // UDP
			rc = snprintf(pos, len, "-p udp ");
		pos += rc;
		len -= rc;

		// write source port
		if (sprf_int>0)
		{
			if (sprt_int>0)
				rc = snprintf(pos, len, "--sport %d:%d ", sprf_int, sprt_int);
			else
				rc = snprintf(pos, len, "--sport %d ", sprf_int);
			pos += rc;
			len -= rc;
		}

		// write dest port
		if (dprf_int>0)
		{
			if (dprt_int>0)
				rc = snprintf(pos, len, "--dport %d:%d ", dprf_int, dprt_int);
			else
				rc = snprintf(pos, len, "--dport %d ", dprf_int);
			pos += rc;
			len -= rc;
		}
	}

	switch(action)
	{
		case ACTION_DROP:			// 1 == ENABLE--DROP mode
			rc = snprintf(pos, len, "-j DROP");
			break;
		case ACTION_ACCEPT:			// 2 == ENABLE--ACCEPT mode
			rc = snprintf(pos, len, "-j ACCEPT");
			break;
	}

	pos += rc;
	len -= rc;
	rc = snprintf(pos, len, "\n");
}

static void makePortForwardRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;

	char *lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char *lan_nm = nvram_get(RT2860_NVRAM, "lan_netmask");
	char *lan_if = getLanIfName();

	// Add nat loopback
	if (inat_loopback)
	{
		//DNAT
		rc = snprintf(pos, len, "iptables -t nat -A %s -i %s -d $1 ", PORT_FORWARD_PRE_CHAIN, lan_if);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (prf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
		pos += rc;
		len -= rc;
		// write dst port
		if (rprf_int != 0)
		{
			rc = (rprt_int != 0) ?
				snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
				snprintf(pos, len, ":%d ", rprf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "\n");
		pos += rc;
		len -= rc;

		//MASQ
		rc = snprintf(pos, len, "iptables -t nat -A %s -s %s/%s -d %s ", PORT_FORWARD_POST_CHAIN, lan_ip, lan_nm, ip_address);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (rprf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", rprf_int, rprt_int) :
			snprintf(pos, len, "--dport %d ", rprf_int);
			pos += rc;
			len -= rc;
		} else {
			if (prf_int != 0)
			{
				rc = (prt_int != 0) ?
				snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
				snprintf(pos, len, "--dport %d ", prf_int);
				pos += rc;
				len -= rc;
			}
		}
		rc = snprintf(pos, len, "-j MASQUERADE\n");
		pos += rc;
		len -= rc;
	}

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -t nat -A %s -i %s ! -d %s/%s ", PORT_FORWARD_PRE_CHAIN, wan_name, lan_ip, lan_nm);
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write src port
	if (prf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
	pos += rc;
	len -= rc;

	// write dst port
	if (rprf_int != 0)
	{
		rc = (rprt_int != 0) ?
			snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
			snprintf(pos, len, ":%d ", rprf_int);
		pos += rc;
		len -= rc;
	}

	rc = snprintf(pos, len, "\n");
}

static void makePortForwardRuleVPN(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;

	char *lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char *lan_nm = nvram_get(RT2860_NVRAM, "lan_netmask");
	char *lan_if = getLanIfName();

	// Add nat loopback
	if (inat_loopback)
	{
		//DNAT
		rc = snprintf(pos, len, "iptables -t nat -$1 %s -i %s -d $3 ", PORT_FORWARD_PRE_CHAIN_VPN, lan_if);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (prf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
		pos += rc;
		len -= rc;
		// write dst port
		if (rprf_int != 0)
		{
			rc = (rprt_int != 0) ?
				snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
				snprintf(pos, len, ":%d ", rprf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "\n");
		pos += rc;
		len -= rc;



		//MASQ
		rc = snprintf(pos, len, "iptables -t nat -$1 %s -s %s/%s -d %s ", PORT_FORWARD_POST_CHAIN_VPN, lan_ip, lan_nm, ip_address);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (rprf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", rprf_int, rprt_int) :
			snprintf(pos, len, "--dport %d ", rprf_int);
			pos += rc;
			len -= rc;
		} else {
			if (prf_int != 0)
			{
				rc = (prt_int != 0) ?
				snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
				snprintf(pos, len, "--dport %d ", prf_int);
				pos += rc;
				len -= rc;
			}
		}
		rc = snprintf(pos, len, "-j MASQUERADE\n");
		pos += rc;
		len -= rc;
	}

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -t nat -$1 %s -i %s ! -d %s/%s ", PORT_FORWARD_PRE_CHAIN_VPN, wan_name, lan_ip, lan_nm);
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write src port
	if (prf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
	pos += rc;
	len -= rc;

	// write dst port
	if (rprf_int != 0)
	{
		rc = (rprt_int != 0) ?
			snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
			snprintf(pos, len, ":%d ", rprf_int);
		pos += rc;
		len -= rc;
	}

	rc = snprintf(pos, len, "\n");
}

static void iptablesIPPortFilterBuildScript(void)
{
	int i = 0, mode, sprf_int, sprt_int, proto, action, dprf_int, dprt_int;
	char rec[256];
	char cmd[1024];
	char sprf[8], sprt[8], protocol[8], iface[8];
	char dprf[8], dprt[8], wan_name[16];
	char mac_address[32], action_str[4];
	char sip[32], dip[32], sim[32], dim[32];
	char *c_if, *firewall_enable, *default_policy;
	char_t *rule;
	int default_drop_flag;

	// Check that IP/port filter is enabled
	firewall_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	if (firewall_enable == NULL)
	{
		syslog(LOG_ERR, "Warning: can't find \"IPPortFilterEnable\" in nvram, %s\n", __FUNCTION__);
		return;
	}
	mode = atoi(firewall_enable);
	if (mode == 0)
		return;

	rule = nvram_get(RT2860_NVRAM, "IPPortFilterRules");
	if (rule == NULL)
	{
		syslog(LOG_ERR, "Warning: can't find \"IPPortFilterRules\" in nvram, %s\n", __FUNCTION__);
		return;
	}

	// add the default policy to the end of FORWARD chain
	default_policy = nvram_get(RT2860_NVRAM, "DefaultFirewallPolicy");
	if (default_policy == NULL)
		default_policy = "0";
	default_drop_flag = atoi(default_policy);

	// get wan name
	strncpy(wan_name, getWanIfName(), sizeof(wan_name)-1);

	//Generate portforward script file
	FILE *fd = fopen(_PATH_MACIP_FILE, "w");
	if (fd != NULL)
	{
		fputs("#!/bin/sh\n\n", fd);
		// make chain and jump to from forward
		fprintf(fd, "iptables -t filter -N %s\n", IPPORT_FILTER_CHAIN);
		fprintf(fd, "iptables -t filter -I FORWARD -j %s\n\n", IPPORT_FILTER_CHAIN);
		// accept related/established sessions by conntrack
		fprintf(fd, "iptables -t filter -A %s -m state --state ESTABLISHED,RELATED -j ACCEPT\n\n", IPPORT_FILTER_CHAIN);

		while ((getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1))
		{
			// Get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
				continue;

			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;

			// get protocol
			if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
				case PROTO_NONE:
				case PROTO_ICMP:
					break;
				default:
					continue;
			}

			// get mac address
			if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
				continue;

			if (strlen(mac_address) > 0)
			{
				if (!isMacValid(mac_address))
					continue;
			}

			// get source ip
			if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
				continue;
			if (!isIpNetmaskValid(sip))
				sip[0] = '\0';

			// get source ip mask
			if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
				continue;
			if (!isIpNetmaskValid(sim))
				sim[0] = '\0';

			// get source port range "from"
			if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
				continue;
			if ((sprf_int = atoi(sprf)) > 65535)
				continue;

			// get source port range "to"
			if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
				continue;
			if ((sprt_int = atoi(sprt)) > 65535)
				continue;

			// get destination ip
			if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
				continue;
			if (!isIpNetmaskValid(dip))
				dip[0] = '\0';

			// get destination ip mask
			if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
				continue;
			if (!isIpNetmaskValid(dim))
				dim[0] = '\0';

			// get destination port range "from"
			if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
				continue;
			if ((dprf_int = atoi(dprf)) > 65535)
				continue;

			// get destination port range "to"
			if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
				continue;
			if ((dprt_int = atoi(dprt)) > 65535)
				continue;

			// get action / policy
			if ((getNthValueSafe(11, rec, ',', action_str, sizeof(action_str)) == -1))
				continue;

			action = atoi(action_str);

			// build rules
			makeIPPortFilterRule(
				cmd, sizeof(cmd), c_if, mac_address, sip, sim, sprf_int,
				sprt_int, dip, dim, dprf_int, dprt_int, proto, action, IPPORT_FILTER_CHAIN);
			// write to file
			fputs(cmd, fd);
		}

		// write default policy
		if (default_drop_flag)
		    snprintf(cmd, 1024, "iptables -t filter -A %s -j DROP\n\n", IPPORT_FILTER_CHAIN);
		else
		    snprintf(cmd, 1024, "iptables -t filter -A %s -j ACCEPT\n\n", IPPORT_FILTER_CHAIN);

		// write to file
		fputs(cmd, fd);

		//close file
		fclose(fd);
		chmod(_PATH_MACIP_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
	}
}

static int checkNatLoopback(char *rule)
{
	char nat_loopback[8];
	char rec[256];
	int inat_loopback, i=0;

	while ( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
	{
		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;

		if (inat_loopback)
			return 1;
	}

	return 0;
}

static void iptablesPortForwardBuildScript(void)
{
	char rec[256];
	char cmd[1024];
	char wan_name[16];
	char ip_address[32], prf[8], prt[8], rprf[9], rprt[8], protocol[8], interface[8], nat_loopback[8];
	char *c_if, *firewall_enable;
	char_t *rule;
	int i=0, prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback;

	//Remove portforward script
	firewall_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (!firewall_enable)
	{
		syslog(LOG_ERR, "Warning: can't find \"PortForwardEnable\" in flash, %s\n", __FUNCTION__);
		return;
	}

	if (atoi(firewall_enable))
	{
		rule = nvram_get(RT2860_NVRAM, "PortForwardRules");
		if(!rule)
		{
			syslog(LOG_ERR, "Warning: can't find \"PortForwardRules\" in flash, %s\n", __FUNCTION__);
			return;
		}
	}
	else
		return;

	int nat_loopback_on = checkNatLoopback(rule);

	// get wan name
	strncpy(wan_name, getWanIfName(), sizeof(wan_name)-1);

	// Generate portforward script file
	FILE *fd = fopen(_PATH_PFW_FILE, "w");
	if (fd == NULL)
		return;

	// Print header for WAN/LAN
	fputs("#!/bin/sh\n\n", fd);
	fprintf(fd,
		"iptables -t nat -N %s\n"
		"iptables -t nat -A PREROUTING -j %s\n\n",
		PORT_FORWARD_PRE_CHAIN, PORT_FORWARD_PRE_CHAIN);

	// Additional rules if port forwarding enabled
	if (nat_loopback_on)
		fprintf(fd,
			"iptables -t nat -N %s\n"
			"iptables -t nat -A POSTROUTING -j %s\n\n",
			PORT_FORWARD_POST_CHAIN, PORT_FORWARD_POST_CHAIN);

	chmod(_PATH_PFW_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);

	// Open file for VPN
	FILE *fd_vpn = fopen(_PATH_PFW_FILE_VPN, "w");
	if (fd_vpn == NULL)
	{
		fclose(fd);
		return;
	}

	// Print header for VPN
	fputs("#!/bin/sh\n\n", fd_vpn);
	fprintf(fd_vpn,
		"iptables -t nat -N %s\n"
		"iptables -t nat -A PREROUTING -j %s\n\n",
		PORT_FORWARD_PRE_CHAIN_VPN, PORT_FORWARD_PRE_CHAIN_VPN);

	// Additional rules if port forwarding enabled
	if (nat_loopback_on)
		fprintf(fd_vpn,
			"iptables -t nat -N %s\n"
			"iptables -t nat -A POSTROUTING -j %s\n\n",
			PORT_FORWARD_POST_CHAIN_VPN, PORT_FORWARD_POST_CHAIN_VPN);

	chmod(_PATH_PFW_FILE_VPN, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);

	// Now write all rules
	while((getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1))
	{
		// get interface
		if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
			continue;

		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;

		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_TCP_UDP:
				break;
			default:
				continue;
		}

		// get port range "from"
		if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			continue;

		if (strlen(prf) > 0)
		{
			if ((prf_int = atoi(prf)) == 0 || prf_int > 65535)
				continue;
		}
		else
			prf_int = 0;

		// get port range "to"
		if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if (prt > 0)
		{
			if ((prt_int = atoi(prt)) > 65535)
				continue;
		}
		else
			prt_int = 0;

		// get ip address
		if ((getNthValueSafe(4, rec, ',', ip_address, sizeof(ip_address)) == -1))
			continue;

		if (!isIpValid(ip_address))
			continue;

		// get forward port range "from"
		if ((getNthValueSafe(5, rec, ',', rprf, sizeof(rprf)) == -1))
			continue;

		if (strlen(rprf) > 0)
		{
			rprf_int = atoi(rprf);
			if (rprf_int > 65535)
				continue;
		}
		else
			rprf_int = 0;

		// get port range "to"
		if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
			continue;

		if (strlen(rprt) > 0)
		{
			if ((rprt_int = atoi(rprt)) > 65535)
				continue;
		}
		else
			rprt_int = 0;

		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;

		int is_vpn = 0;
		// Patch interface
		if (strcmp(interface, "LAN")==0)
			c_if = "br0";
		else if (strcmp(interface, "VPN")==0)
		{
			c_if = "$2";
			is_vpn = 1;
		}
		else
			c_if = wan_name;

		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
				if (is_vpn)
				{
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
				}
				else
				{
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
				}
				break;

			case PROTO_TCP_UDP:
				if (is_vpn)
				{
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
				}
				else
				{
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
				}
				break;
		}
	}

	// Close files
	fclose(fd_vpn);
	fclose(fd);
}

static int getRuleNums(char *rules)
{
	return getNums(rules, ';');
}

/*
 * ASP function
 */
static int getPortForwardRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int first=1, i=0;
	int prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback;
	char ip_address[32], prf[8], prt[8], rprf[8], rprt[8], comment[64], protocol[8], interface[8], nat_loopback[8];
	char rec[128];
	char_t *rules = nvram_get(RT2860_NVRAM, "PortForwardRules");

	if (rules == NULL)
		return 0;
	if (strlen(rules) == 0)
		return 0;

	/* format is :
	 * [interface],[protocol],[src_port],[dst_port],[ip_address],[redirect_src_port],[redirect_dst_port],[nat_loopback],[comment];
	 */
	while(getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1)
	{
		// get interface
		if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
			continue;

		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;

		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_TCP_UDP:
				break;
			default:
				continue;
		}

		// get port range "from"
		if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			continue;

		if (strlen(prf) > 0)
		{
			if ((prf_int = atoi(prf)) == 0 || prf_int > 65535)
				continue;
		}

		// get port range "to"
		if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if (prt > 0)
		{
			if ((prt_int = atoi(prt)) > 65535)
				continue;
		}

		// get ip address
		if ((getNthValueSafe(4, rec, ',', ip_address, sizeof(ip_address)) == -1))
			continue;

		if (!isIpValid(ip_address))
			continue;

		// get forward port range "from"
		if ((getNthValueSafe(5, rec, ',', rprf, sizeof(rprf)) == -1))
			continue;

		if (strlen(rprf) > 0)
		{
			if ((rprf_int = atoi(rprf)) == 0 || rprf_int > 65535)
				continue;
		}

		// get port range "to"
		if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
			continue;

		if (rprt > 0)
		{
			if ((rprt_int = atoi(rprt)) > 65535)
				continue;
		}

		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;


		// Get comment
		if ((getNthValueSafe(8, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s' ]"),
				(first) ? "" : ",\n\t",
				interface,
				proto,
				prf, prt,
				ip_address,
				rprf, rprt,
				inat_loopback,
				comment
			);

		first = 0;
	}

	return 0;
}

/*
 * ASP function
 */
static int getPortFilteringRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int i = 0, first = 1;
	int sprf_int, sprt_int, proto, dprf_int, dprt_int;
	char mac_address[32];
	char sip[32], sprf[8], sprt[8], comment[16], protocol[8], action[4];
	char dip[32], dprf[8], dprt[8], iface[8], sim[32], dim[32];
	char rec[256];
	char_t *rules = nvram_get(RT2860_NVRAM, "IPPortFilterRules");

	if (rules == NULL)
		return 0;

	while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1)
	{
		// Get interface
		if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
			continue;

		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;

		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_NONE:
			case PROTO_ICMP:
				break;
			default:
				continue;
		}

		// get mac address
		if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
			continue;

		// get source ip
		if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
			continue;
		if (!isIpNetmaskValid(sip))
			sip[0] = '\0';

		// get source ip mask
		if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
			continue;
		if (!isIpNetmaskValid(sim))
			sim[0] = '\0';

		// get source port range "from"
		if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
			continue;
		if ((sprf_int = atoi(sprf)) > 65535)
			continue;

		// get source port range "to"
		if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
			continue;
		if ((sprt_int = atoi(sprt)) > 65535)
			continue;

		// get destination ip
		if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
			continue;
		if (!isIpNetmaskValid(dip))
			dip[0] = '\0';

		// get destination ip mask
		if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
			continue;
		if (!isIpNetmaskValid(dim))
			dim[0] = '\0';

		// get destination port range "from"
		if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
			continue;
		if ((dprf_int = atoi(dprf)) > 65535)
			continue;

		// get destination port range "to"
		if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
			continue;
		if ((dprt_int = atoi(dprt)) > 65535)
			continue;

		// get action / policy
		if ((getNthValueSafe(11, rec, ',', action, sizeof(action)) == -1))
			continue;

		// get comment
		if ((getNthValueSafe(12, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' ]"),
				(first) ? "" : ",\n\t",
				iface,
				proto,
				mac_address,
				sip, sim,
				sprf, sprt,
				dip, dim,
				dprf, dprt,
				action,
				comment
			);

		first = 0;
	}

	return 0;
}

/* Same as the file "linux/netfilter_ipv4/ipt_webstr.h" */
#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08
static void iptablesWebsFilterRun(void)
{
	int i, content_filter = 0;
	char entry[256]; //need long buffer for utf domain name encoding support
	char *proxy		= nvram_get(RT2860_NVRAM, "websFilterProxy");
	char *java		= nvram_get(RT2860_NVRAM, "websFilterJava");
	char *activex		= nvram_get(RT2860_NVRAM, "websFilterActivex");
	char *cookies		= nvram_get(RT2860_NVRAM, "websFilterCookies");
	char *url_filter	= nvram_get(RT2860_NVRAM, "websURLFilters");
	char *host_filter	= nvram_get(RT2860_NVRAM, "websHostFilters");

	if ((url_filter && strlen(url_filter) && getRuleNums(url_filter)) ||
		(host_filter && strlen(host_filter) && getRuleNums(host_filter)) ||
			atoi(proxy) || atoi(java) || atoi(activex) || atoi(cookies))
	{
		// Content filter
		if(atoi(java))
			content_filter += BLK_JAVA;
		if(atoi(activex))
			content_filter += BLK_ACTIVE;
		if(atoi(cookies))
			content_filter += BLK_COOKIE;
		if(atoi(proxy))
			content_filter += BLK_PROXY;

		//Generate portforward script file
		FILE *fd = fopen(_PATH_WEBS_FILE, "w");

		if (fd != NULL)
		{
			fputs("#!/bin/sh\n\n", fd);
			fprintf(fd, "iptables -t filter -N %s\n", WEB_FILTER_CHAIN);
			fprintf(fd, "iptables -t filter -A FORWARD -j %s\n", WEB_FILTER_CHAIN);

			if (content_filter)
			{
				// Why only 3 ports are inspected?(This idea is from CyberTAN source code)
				// TODO: use layer7 to inspect HTTP
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 80   -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 3128 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 8080 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
			}

			// URL filter
			i=0;
			while ((i < getRuleNums(url_filter)) && (getNthValueSafe(i, url_filter, ';', entry, sizeof(entry)) != -1))
			{
				if (strlen(entry))
				{
					if (!strncasecmp(entry, "http://", strlen("http://")))
						strcpy(entry, entry + strlen("http://"));

					fprintf(fd, "iptables -A %s -p tcp -m tcp -m webstr --url  %s -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, entry);
				}
				i++;
			}

			// HOST(Keyword) filter
			i=0;
			while ((i < getRuleNums(host_filter)) && (getNthValueSafe(i, host_filter, ';', entry, sizeof(entry)) != -1))
			{
				if (strlen(entry))
				{
					fprintf(fd, "iptables -A %s -p tcp -m tcp -m webstr --host %s -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, entry);
				}
				i++;
			}

			//closefile
			fclose(fd);
			chmod(_PATH_WEBS_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
		}
	}
}

void firewall_rebuild_etc(void)
{
	// Rebuild firewall scripts in etc

	// Port forwarding
	char *pfw_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (pfw_enable == NULL)
		pfw_enable = "0";

	// Remove portforwards scripts
	doSystem("rm -f " _PATH_PFW_FILE);
	doSystem("rm -f " _PATH_PFW_FILE_VPN);
	if (strcmp(pfw_enable, "1") == 0) // Turned on?
		iptablesPortForwardBuildScript();

	// IP/Port/MAC filtering
	char *ipf_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	if (ipf_enable == NULL)
		ipf_enable = "0";

	// Remove mac filter script
	doSystem("rm -f " _PATH_MACIP_FILE);
	if (strcmp(ipf_enable, "1") == 0) // Turned on?
		iptablesIPPortFilterBuildScript();

	// Web filtering
	doSystem("rm -f " _PATH_WEBS_FILE);
	iptablesWebsFilterRun();
}

void firewall_rebuild(void)
{
	//rebuild firewall scripts in etc
	firewall_rebuild_etc();
	doSystem("service iptables restart");
}

static int showDMZIPAddressASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *DMZIPAddress = nvram_get(RT2860_NVRAM, "DMZIPAddress");

	if(!DMZIPAddress)
		return 0;
	if(!strlen(DMZIPAddress))
		return 0;

	websWrite(wp, T("%s"), DMZIPAddress);
	return 0;
}

static void portForward(webs_t wp, char_t *path, char_t *query)
{
	char_t *pfe = websGetVar(wp, T("portForwardEnabled"), T("0"));
	char_t *PortForwardRules = websGetVar(wp, T("portForwardRules"), T(""));

	pfe = (CHK_IF_DIGIT(pfe, 1)) ? "1" : "0";

	// Commit
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "PortForwardEnable", pfe);
	if (CHK_IF_DIGIT(pfe, 1))
		nvram_bufset(RT2860_NVRAM, "PortForwardRules", PortForwardRules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("portForwardEnabled: %s<br>\n"), pfe);
	websFooter(wp);
	websDone(wp, 200);
#else
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// call iptables
	firewall_rebuild();
}

static void portFiltering(webs_t wp, char_t *path, char_t *query)
{
	char_t *firewall_enable = websGetVar(wp, T("portFilterEnabled"), T("0"));
	char_t *default_policy = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));
	char_t *firewall_rules = websGetVar(wp, T("portFilteringRules"), T(""));

	firewall_enable = (CHK_IF_DIGIT(firewall_enable, 1)) ? "1" : "0";
	default_policy = (CHK_IF_DIGIT(default_policy, 1)) ? "1" : "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "IPPortFilterEnable", firewall_enable);
	if (CHK_IF_DIGIT(firewall_enable, 1))
	{
		// Store default firewall policy & rules
		nvram_bufset(RT2860_NVRAM, "DefaultFirewallPolicy", default_policy);
		nvram_bufset(RT2860_NVRAM, "IPPortFilterRules", firewall_rules);
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("portFilteringEnabled: %s<br>\n"), firewall_enable);
	websWrite(wp, T("default_policy: %s<br>\n"), default_policy);
	websFooter(wp);
	websDone(wp, 200);
#else
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// Call iptables
	firewall_rebuild();
	// flush conntrack for apply changes immediately
	doSystem("echo f > /proc/net/nf_conntrack");
}

static void DMZ(webs_t wp, char_t *path, char_t *query)
{
	char *dmzE, *ip_address, *dmzLoopback;

	char_t *reset = websGetVar(wp, T("reset"), T("0"));
	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 3, "DMZEnable", "DMZIPAddress", "DMZNATLoopback");
		goto out;
	}

	dmzE = websGetVar(wp, T("DMZEnabled"), T(""));
	ip_address = websGetVar(wp, T("DMZIPAddress"), T(""));
	dmzLoopback = websGetVar(wp, T("dmzLoopback"), T("off"));
	char_t *submitUrl;

	if (CHK_IF_DIGIT(dmzE, 0)) // disable
	{
		nvram_set(RT2860_NVRAM, "DMZEnable", "0");
	}
	else if (CHK_IF_DIGIT(dmzE, 1)) // enable
	{
		if (!isIpValid(ip_address))
			return;

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "DMZEnable", "1");
		nvram_bufset(RT2860_NVRAM, "DMZIPAddress", ip_address);
		nvram_bufset(RT2860_NVRAM, "DMZNATLoopback", CHK_IF_DIGIT(dmzLoopback, 1) ? "1" : "0");
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else
		return;
out:
#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("DMZEnabled: %s<br>\n"), dmzE);
	websWrite(wp, T("ip_address: %s<br>\n"), ip_address);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// Call iptables
	firewall_rebuild();
}

static void websSysFirewall(webs_t wp, char_t *path, char_t *query)
{
	char *wpfE = websGetVar(wp, T("pingFrmWANFilterEnabled"), T(""));

	// someone use malform page.....
	if(!wpfE || !strlen(wpfE))
		return;


	websHeader(wp);
	websWrite(wp, T("WANPingFilter: %s<br>\n"), wpfE);
	websFooter(wp);
	websDone(wp, 200);

	// call iptables
	firewall_rebuild();
}


const parameter_fetch_t content_filtering_args[] =
{
	{ T("urlFiltering"),           "websURLFilters",       0,   T("") },
	{ T("hostFiltering"),          "websHostFilters",      0,   T("") },
	{ T("websFilterProxy"),        "websFilterProxy",      2,   T("") },
	{ T("websFilterJava"),         "websFilterJava",       2,   T("") },
	{ T("websFilterActivex"),      "websFilterActivex",    2,   T("") },
	{ T("websFilterCookies"),      "websFilterCookies",    2,   T("") },

	{ NULL, NULL, 0, NULL } // Terminator
};

static void webContentFilterSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	// Store firewall parameters
	setupParameters(wp, content_filtering_args, 1);

	//call iptables
	firewall_rebuild();

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);

}

/* goform/setFirewallAlg */
const parameter_fetch_t alg_params[] =
{
	{ "alg_ftp",		T("fwAlgFTP"),			2,   T("") },
	{ "alg_gre",		T("fwAlgGRE"),			2,   T("") },
	{ "alg_h323",		T("fwAlgH323"),			2,   T("") },
	{ "alg_pptp",		T("fwAlgPPTP"),			2,   T("") },
	{ "alg_sip",		T("fwAlgSIP"),			2,   T("") },
	{ "alg_rtsp",		T("fwAlgRTSP"),			2,   T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setFirewallAlg(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	// Store firewall parameters
	setupParameters(wp, alg_params, 1);

	//call iptables
	firewall_rebuild();

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);

}

/* goform/setFirewall */
const parameter_fetch_t firewall_params[] =
{
	{ "ForwardSesLimit",		T("ForwardSesLimit"),			0,   T("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setFirewall(webs_t wp, char_t *path, char_t *query)
{
	// Store firewall parameters
	setupParameters(wp, firewall_params, 1);

	//call iptables
	firewall_rebuild();

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

void formDefineFirewall(void)
{
	websFormDefine(T("portFiltering"), portFiltering);

	websAspDefine(T("getPortFilteringRules"), getPortFilteringRules);

	websFormDefine(T("DMZ"), DMZ);
	websAspDefine(T("getDMZEnableASP"), getDMZEnableASP);
	websAspDefine(T("showDMZIPAddressASP"), showDMZIPAddressASP);

	websFormDefine(T("setFirewallAlg"), setFirewallAlg);

	websAspDefine(T("getPortForwardRules"), getPortForwardRules);
	websFormDefine(T("portForward"), portForward);

	websFormDefine(T("websSysFirewall"), websSysFirewall);
	websFormDefine(T("webContentFilterSetup"), webContentFilterSetup);

	websFormDefine(T("setFirewall"), setFirewall);
}
