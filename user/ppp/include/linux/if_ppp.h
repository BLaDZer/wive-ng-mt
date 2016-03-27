#include <linux/ppp-ioctl.h>

#define ifr__name       b.ifr_ifrn.ifrn_name
#define stats_ptr       b.ifr_ifru.ifru_data

struct ifpppstatsreq {
	struct ifreq b;
	struct ppp_stats stats;	/* statistic information */
};
