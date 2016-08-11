#ifndef _WIVE_FIREWALL_H
#define _WIVE_FIREWALL_H	1


struct PortFilteringRule  {
    int protocol;
    char mac_address[32];
    char dip[32], dprf[8], dprt[8], interface[8], sim[32], dim[32];
    char sip[32], sprf[8], sprt[8], comment[16], action[4];
};

struct PortForwardRule  {
    int protocol, nat_loopback;
    char ip_address[32], prf[8], prt[8], rprf[8], rprt[8], comment[64], interface[8];
};



int parse_portforward_rule(char* rulestr, struct PortForwardRule* rule, int rule_num);
int parse_portfiltering_rule(char* rulestr, struct PortFilteringRule* rule, int rule_num);

#endif
