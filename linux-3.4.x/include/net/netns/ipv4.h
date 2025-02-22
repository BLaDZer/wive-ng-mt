/*
 * ipv4 in net namespaces
 */

#ifndef __NETNS_IPV4_H__
#define __NETNS_IPV4_H__

#include <net/inet_frag.h>
#include <linux/rcupdate.h>
#include <linux/mutex.h>
#include <linux/types.h>

struct ctl_table_header;
struct ipv4_devconf;
struct fib_rules_ops;
struct hlist_head;
struct sock;

struct netns_ipv4 {
#ifdef CONFIG_SYSCTL
	struct ctl_table_header	*forw_hdr;
	struct ctl_table_header	*frags_hdr;
	struct ctl_table_header	*ipv4_hdr;
	struct ctl_table_header *route_hdr;
#endif
	struct ipv4_devconf	*devconf_all;
	struct ipv4_devconf	*devconf_dflt;
	struct ip_ra_chain __rcu *ra_chain;
	struct mutex		ra_mutex;
#ifdef CONFIG_IP_MULTIPLE_TABLES
	struct fib_rules_ops	*rules_ops;
#endif
	struct hlist_head	*fib_table_hash;
	struct sock		*fibnl;

	struct sock		**icmp_sk;
	struct sock		*tcp_sock;

	struct netns_frags	frags;
#ifdef CONFIG_NETFILTER
	struct xt_table		*iptable_filter;
	struct xt_table		*iptable_mangle;
	struct xt_table		*iptable_raw;
	struct xt_table		*arptable_filter;
#ifdef CONFIG_SECURITY
	struct xt_table		*iptable_security;
#endif
	struct xt_table		*nat_table;
	struct hlist_head	*nat_bysource;
	unsigned int		nat_htable_size;
#endif

	int sysctl_icmp_echo_ignore_all;
	int sysctl_icmp_echo_ignore_broadcasts;
	int sysctl_icmp_ignore_bogus_error_responses;
	int sysctl_icmp_ratelimit;
	int sysctl_icmp_ratemask;
	int sysctl_icmp_errors_use_inbound_ifaddr;
	int sysctl_rt_cache_rebuild_count;
	int current_rt_cache_rebuild_count;

	unsigned int sysctl_ping_group_range[2];
	long sysctl_tcp_mem[3];
	int sysctl_tcp_min_snd_mss;

	atomic_t rt_genid;
	atomic_t dev_addr_genid;

#ifdef CONFIG_IP_MROUTE
#ifndef CONFIG_IP_MROUTE_MULTIPLE_TABLES
	struct mr_table		*mrt;
#else
	struct list_head	mr_tables;
	struct fib_rules_ops	*mr_rules_ops;
#endif
#endif
};
#endif
