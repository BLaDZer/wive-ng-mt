/*
 * IpV6 passthrough
 * This module is used to pass through the IPv6 packets
 */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/proc_fs.h>

//#define PTHROUGH_DEBUG
#define PTHROUGH_LOG		"ipv6-pthrough:"

int ipv6_pt_enable = 0;

static char ipv6_pt_landev[IFNAMSIZ];
static char ipv6_pt_wandev[IFNAMSIZ];

static struct net_device *ipv6_dev_lan = NULL;
static struct net_device *ipv6_dev_wan = NULL;

static inline
void passthrough_skb(struct sk_buff *skb, struct net_device *dev_pass)
{
	skb->dev = dev_pass;
	skb_push(skb, ETH_HLEN);
	dev_queue_xmit(skb);
}

/* return 1, if we want to handle this packet, or
 * return 0, let other ones do this */
int ipv6_pthrough(struct sk_buff *skb)
{
	struct net_device *dev_lan, *dev_wan;
	struct ethhdr *eth;

	if (skb->protocol == htons(ETH_P_IPV6)) {
		dev_lan = ipv6_dev_lan;
		dev_wan = ipv6_dev_wan;

		if (!dev_lan || !dev_wan || !skb->dev)
			return 0;

		eth = (struct ethhdr *)skb_mac_header(skb);
		if (skb->dev == dev_lan) {
#ifdef PTHROUGH_DEBUG
			printk("%s IPv6 outgoing packet (%s), "
				"DMAC=%02X:%02X:%02X:%02X:%02X:%02X, "
				"SMAC=%02X:%02X:%02X:%02X:%02X:%02X\n", PTHROUGH_LOG, skb->dev->name, 
				eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
				eth->h_dest[3], eth->h_dest[4], eth->h_dest[5],
				eth->h_source[0], eth->h_source[1], eth->h_source[2],
				eth->h_source[3], eth->h_source[4], eth->h_source[5]);
#endif
			if (!compare_ether_addr(dev_wan->dev_addr, eth->h_source))
				return 0;

			passthrough_skb(skb, dev_wan);
			return 1;
		}
		else
		if (skb->dev == dev_wan) {
#ifdef PTHROUGH_DEBUG
			printk("%s IPv6 incoming packet (%s), "
				"DMAC=%02X:%02X:%02X:%02X:%02X:%02X, "
				"SMAC=%02X:%02X:%02X:%02X:%02X:%02X\n", PTHROUGH_LOG, skb->dev->name,
				eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
				eth->h_dest[3], eth->h_dest[4], eth->h_dest[5],
				eth->h_source[0], eth->h_source[1], eth->h_source[2],
				eth->h_source[3], eth->h_source[4], eth->h_source[5]);
#endif
			if (!compare_ether_addr(dev_lan->dev_addr, eth->h_source))
				return 0;

			passthrough_skb(skb, dev_lan);
			return 1;
		}
	}

	return 0;
}

int proc_ipv6_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int ret=0;
	if (ipv6_pt_enable) {
		ret = sprintf(page, "%s,%s\n", ipv6_pt_landev, ipv6_pt_wandev);
	} else {
		ret = sprintf(page, "null,null\n");
	}

	return ret;
}

#define isCHAR(x) ((x >= 'a') && (x <= 'z')) ? 1:((x >= '0') && (x <= '9')) ? 1:((x >= 'A') && (x <= 'Z')) ? 1:(x == '.') ? 1:0
int proc_ipv6_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char *pt;
	int cur_count=0, pt_enable_last;
	struct net_device *dev_lan, *dev_wan;

	pt_enable_last = ipv6_pt_enable;

	/* we expect that buffer contain format of "landev_name,wandev_name" */
	memset(ipv6_pt_landev, 0x0, sizeof (ipv6_pt_landev));
	for (pt=ipv6_pt_landev; (*buffer && (*buffer != ',') && (*buffer != '\n')) ; buffer++) {
		if ((*buffer != ' ') && isCHAR(*buffer)) {
			*pt = *buffer;
			pt++;
		}
		cur_count++;
		if (cur_count >= count)
			break;
	}

	if (!(*buffer) || (cur_count >= count)){
		printk("%s use <lan>,<wan> device name format!\n", PTHROUGH_LOG);
		goto ppw_failed;
	}

	memset(ipv6_pt_wandev, 0x0, sizeof (ipv6_pt_wandev));
	for (pt=ipv6_pt_wandev, buffer++; (*buffer && (*buffer != ';') && (*buffer != '\n')); buffer++) {
		if ((*buffer != ' ') && isCHAR(*buffer)) {
			*pt = *buffer;
			pt++;
		}
		cur_count++;
		if (cur_count >= count)
			break;
	}

	if (!(dev_lan = dev_get_by_name(&init_net, ipv6_pt_landev))) {
#ifdef PTHROUGH_DEBUG
		printk("%s no %s device (%s)\n", PTHROUGH_LOG, "lan", ipv6_pt_landev);
#endif
		goto ppw_failed;
	}

	if (!(dev_wan = dev_get_by_name(&init_net, ipv6_pt_wandev))) {
#ifdef PTHROUGH_DEBUG
		printk("%s no %s device (%s)\n", PTHROUGH_LOG, "wan", ipv6_pt_wandev);
#endif
		dev_put(dev_lan);
		goto ppw_failed;
	}

	rtnl_lock();
	if (ipv6_dev_lan) {
		if (ipv6_dev_lan != dev_lan)
			dev_set_promiscuity(ipv6_dev_lan, -1);
		dev_put(ipv6_dev_lan);
	}
	if (ipv6_dev_wan) {
		if (ipv6_dev_wan != dev_wan)
			dev_set_promiscuity(ipv6_dev_wan, -1);
		dev_put(ipv6_dev_wan);
	}
	if (ipv6_dev_lan != dev_lan) {
		dev_set_promiscuity(dev_lan, 1);
		ipv6_dev_lan = dev_lan;
	}
	if (ipv6_dev_wan != dev_wan) {
		dev_set_promiscuity(dev_wan, 1);
		ipv6_dev_wan = dev_wan;
	}
	ipv6_pt_enable = 1;
	rtnl_unlock();

	if (pt_enable_last != ipv6_pt_enable)
		printk("%s [%s]<->[%s]\n", PTHROUGH_LOG, ipv6_pt_landev, ipv6_pt_wandev);
	return count;

ppw_failed:

	rtnl_lock();
	ipv6_pt_enable = 0;
	if (ipv6_dev_lan) {
		dev_set_promiscuity(ipv6_dev_lan, -1);
		dev_put(ipv6_dev_lan);
		ipv6_dev_lan = NULL;
	}
	if (ipv6_dev_wan) {
		dev_set_promiscuity(ipv6_dev_wan, -1);
		dev_put(ipv6_dev_wan);
		ipv6_dev_wan = NULL;
	}
	rtnl_unlock();

	memset(ipv6_pt_landev, 0x0, sizeof(ipv6_pt_landev));
	memset(ipv6_pt_wandev, 0x0, sizeof(ipv6_pt_wandev));

	if (pt_enable_last != ipv6_pt_enable)
		printk("%s disabled\n", PTHROUGH_LOG);
	return count;
}
