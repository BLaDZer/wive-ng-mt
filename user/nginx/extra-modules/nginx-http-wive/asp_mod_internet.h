#ifndef __ASP_MOD_INTERNET_H__
#define __ASP_MOD_INTERNET_H__

#include "asp_mod_helpers.h"

static parameter_fetch_t vpn_args[] =
{
	{ ("vpn_server"),             "vpnServer",            0,       ("") },
	{ ("vpn_range"),              "vpnRange",             0,       ("") },
	{ ("vpn_user"),               "vpnUser",              0,       ("") },
	{ ("vpn_pass"),               "vpnPassword",          0,       ("") },
	{ ("vpn_mtu"),                "vpnMTU",               0,       ("") },
	{ ("vpn_type"),               "vpnType",              0,       ("") },
	{ ("vpn_mppe"),               "vpnMPPE",              1,       ("") },
	{ ("vpn_dgw"),                "vpnDGW",               0,       ("") },
	{ ("vpn_peerdns"),            "vpnPeerDNS",           1,       ("") },
	{ ("vpn_debug"),              "vpnDebug",             1,       ("") },
	{ ("vpn_nat"),                "vpnNAT",               1,       ("") },
	{ ("vpn_lcp"),                "vpnEnableLCP",         1,       ("") },
	{ ("vpn_auth_type"),          "vpnAuthProtocol",      0,       ("") },
	{ ("vpn_pppoe_iface"),        "vpnInterface",         0,       ("") },
	{ ("vpn_pppoe_service"),      "vpnService",           0,       ("") },
	{ ("vpn_pure_pppoe"),         "vpnPurePPPOE",         2,       ("") },
	{ ("vpn_lcp_errors"),         "vpnLCPFailure",        0,       ("") },
	{ ("vpn_lcp_interval"),       "vpnLCPInterval",       0,       ("") },
	{ ("vpn_test_reachable"),     "vpnTestReachable",     2,       ("") },
	{ NULL,                        NULL,                   0,       NULL  }  // Terminator
};

static parameter_fetch_t lanauth_args[] =
{
	{ ("vpn_type"),               "vpnType",              0,       ("") },
	{ ("vpn_pass"),               "vpnPassword",          0,       ("") },
	{ ("lanauth_access"),         "LANAUTH_LVL",          0,       ("") },

	{ NULL, NULL, 0, NULL } // Terminator
};


int asp_mod_internet_init();


#endif // __ASP_MOD_INTERNET_H__