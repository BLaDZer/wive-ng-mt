/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

/* parse_portforward_rule - Parse portforward rule into struct by its number
 *
 * arg: rulestr    - rules string to parse
 * arg: (out) rule - output rule structure
 * arg: rule_num   - index of rule to parse
 * return: 0 = OK
 */
int parse_portforward_rule(char* rulestr, struct PortForwardRule* rule, int rule_num)
{
	int prf_int, prt_int, rprf_int, rprt_int;
	char protostr[8];
	char nat_loopback_str[8];
	char rec[128];

	if (rulestr == NULL)
		return 1;
	if (strlen(rulestr) == 0)
		return 2;

	/* format is :
	 * [interface],[protocol],[src_port],[dst_port],[ip_address],[redirect_src_port],[redirect_dst_port],[nat_loopback],[comment];
	 */
	if (getNthValueSafe(rule_num, rulestr, ';', rec, sizeof(rec)) == -1)
		return 3;

	// get interface
	if ((getNthValueSafe(0, rec, ',', rule->interface, sizeof(rule->interface)) == -1))
		return 4;

	// get protocol
	if ((getNthValueSafe(1, rec, ',', protostr, sizeof(protostr)) == -1))
		return 5;

	rule->protocol = atoi(protostr);
	switch(rule->protocol)
	{
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_TCP_UDP:
			break;
		default:
			return 6;
	}

	// get port range "from"
	if ((getNthValueSafe(2, rec, ',', rule->prf, sizeof(rule->prf)) == -1))
		return 7;

	if (strlen(rule->prf) > 0)
	{
		if ((prf_int = atoi(rule->prf)) == 0 || prf_int > 65535)
			return 8;
	}

	// get port range "to"
	if ((getNthValueSafe(3, rec, ',', rule->prt, sizeof(rule->prt)) == -1))
		return 9;

	if (strlen(rule->prt) > 0)
	{
		if ((prt_int = atoi(rule->prt)) > 65535)
			return 10;
	}

	// get ip address
	if ((getNthValueSafe(4, rec, ',', rule->ip_address, sizeof(rule->ip_address)) == -1))
		return 11;

	if (!isIpValid(rule->ip_address))
		return 12;

	// get forward port range "from"
	if ((getNthValueSafe(5, rec, ',', rule->rprf, sizeof(rule->rprf)) == -1))
		return 13;

	if (strlen(rule->rprf) > 0)
	{
		if ((rprf_int = atoi(rule->rprf)) == 0 || rprf_int > 65535)
			return 14;
	}

	// get port range "to"
	if ((getNthValueSafe(6, rec, ',', rule->rprt, sizeof(rule->rprt)) == -1))
		return 15;

	if (strlen(rule->rprt) > 0)
	{
		if ((rprt_int = atoi(rule->rprt)) > 65535)
			return 16;
	}

	// get Nat Loopback enable flag
	if ((getNthValueSafe(7, rec, ',', nat_loopback_str, sizeof(nat_loopback_str)) == -1))
		return 17;

	if (strlen(nat_loopback_str) > 0)
		rule->nat_loopback = atoi(nat_loopback_str);
	else
		rule->nat_loopback = 0;


	// Get comment
	if ((getNthValueSafe(8, rec, ',', rule->comment, sizeof(rule->comment)) == -1))
		return 18;

	return 0;
}

/* parse_portfiltering_rule - Parse portfiltering rule into struct by its number
 *
 * arg: rulestr    - rules string to parse
 * arg: (out) rule - output rule structure
 * arg: rule_num   - index of rule to parse
 * return: 0 = OK
 */
int parse_portfiltering_rule(char* rulestr, struct PortFilteringRule* rule, int rule_num)
{
	int sprf_int, sprt_int, dprf_int, dprt_int;
	char protostr[8];
	char rec[256];

	if (rulestr == NULL)
		return 1;

	if (getNthValueSafe(rule_num, rulestr, ';', rec, sizeof(rec)) == -1)
		return 2;
	
	// Get interface
	if ((getNthValueSafe(0, rec, ',', rule->interface, sizeof(rule->interface)) == -1))
		return 3;

	// get protocol
	if ((getNthValueSafe(1, rec, ',', protostr, sizeof(protostr)) == -1))
		return 4;

	rule->protocol = atoi(protostr);
	switch (rule->protocol)
	{
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_NONE:
		case PROTO_ICMP:
			break;
		default:
			return 5;
	}

	// get mac address
	if ((getNthValueSafe(2, rec, ',', rule->mac_address, sizeof(rule->mac_address)) == -1))
		return 6;

	// get source ip
	if ((getNthValueSafe(3, rec, ',', rule->sip, sizeof(rule->sip)) == -1))
		return 7;

	if (!isIpNetmaskValid(rule->sip))
		rule->sip[0] = '\0';

	// get source ip mask
	if ((getNthValueSafe(4, rec, ',', rule->sim, sizeof(rule->sim)) == -1))
		return 8;

	if (!isIpNetmaskValid(rule->sim))
		rule->sim[0] = '\0';

	// get source port range "from"
	if ((getNthValueSafe(5, rec, ',', rule->sprf, sizeof(rule->sprf)) == -1))
		return 9;

	if ((sprf_int = atoi(rule->sprf)) > 65535)
		return 10;

	// get source port range "to"
	if ((getNthValueSafe(6, rec, ',', rule->sprt, sizeof(rule->sprt)) == -1))
		return 11;

	if ((sprt_int = atoi(rule->sprt)) > 65535)
		return 12;

	// get destination ip
	if ((getNthValueSafe(7, rec, ',', rule->dip, sizeof(rule->dip)) == -1))
		return 13;

	if (!isIpNetmaskValid(rule->dip))
		rule->dip[0] = '\0';

	// get destination ip mask
	if ((getNthValueSafe(8, rec, ',', rule->dim, sizeof(rule->dim)) == -1))
		return 14;

	if (!isIpNetmaskValid(rule->dim))
		rule->dim[0] = '\0';

	// get destination port range "from"
	if ((getNthValueSafe(9, rec, ',', rule->dprf, sizeof(rule->dprf)) == -1))
		return 15;

	if ((dprf_int = atoi(rule->dprf)) > 65535)
		return 16;

	// get destination port range "to"
	if ((getNthValueSafe(10, rec, ',', rule->dprt, sizeof(rule->dprt)) == -1))
		return 17;

	if ((dprt_int = atoi(rule->dprt)) > 65535)
		return 18;

	// get action / policy
	if ((getNthValueSafe(11, rec, ',', rule->action, sizeof(rule->action)) == -1))
		return 19;

	// get comment
	if ((getNthValueSafe(12, rec, ',', rule->comment, sizeof(rule->comment)) == -1))
		return 20;

	return 0;


}
