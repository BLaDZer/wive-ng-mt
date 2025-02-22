#ifndef __LINUX_NETFILTER_H
#define __LINUX_NETFILTER_H

#include <linux/types.h>

#include <linux/sysctl.h>

/* Nat mode type */
#if defined(CONFIG_NAT_CONE)
#define NAT_MODE_LINUX		0
#define NAT_MODE_FCONE		1
#define NAT_MODE_RCONE		2
#endif

/* used in skip lists in conntrack */
#define SKIP_NO			0
#define SKIP_ALL		1
#define SKIP_SWO		2

/* Responses from hook functions. */
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4

#if !defined(CONFIG_BCM_NAT)
#define NF_STOP 5
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
#define NF_IMQ_QUEUE 6
#define NF_MAX_VERDICT NF_IMQ_QUEUE
#else
#define NF_MAX_VERDICT NF_STOP
#endif
#else
#define NF_FAST_NAT 5
#define NF_STOP 6
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
#define NF_IMQ_QUEUE 7
#define NF_MAX_VERDICT NF_IMQ_QUEUE
#else
#define NF_MAX_VERDICT NF_STOP
#endif
#endif

/* we overload the higher bits for encoding auxiliary data such as the queue
 * number or errno values. Not nice, but better than additional function
 * arguments. */
#define NF_VERDICT_MASK 0x000000ff

/* extra verdict flags have mask 0x0000ff00 */
#define NF_VERDICT_FLAG_QUEUE_BYPASS	0x00008000

/* queue number (NF_QUEUE) or errno (NF_DROP) */
#define NF_VERDICT_QMASK 0xffff0000
#define NF_VERDICT_QBITS 16

#define NF_QUEUE_NR(x) ((((x) << 16) & NF_VERDICT_QMASK) | NF_QUEUE)

#define NF_DROP_ERR(x) (((-x) << 16) | NF_DROP)

/* only for userspace compatibility */
/* Generic cache responses from hook functions.
   <= 0x2000 is used for protocol-flags. */
#define NFC_UNKNOWN 0x4000
#define NFC_ALTERED 0x8000

/* NF_VERDICT_BITS should be 8 now, but userspace might break if this changes */
#define NF_VERDICT_BITS 16

enum nf_inet_hooks {
	NF_INET_PRE_ROUTING,
	NF_INET_LOCAL_IN,
	NF_INET_FORWARD,
	NF_INET_LOCAL_OUT,
	NF_INET_POST_ROUTING,
	NF_INET_NUMHOOKS
};

enum {
	NFPROTO_UNSPEC =  0,
	NFPROTO_IPV4   =  2,
	NFPROTO_ARP    =  3,
	NFPROTO_BRIDGE =  7,
	NFPROTO_IPV6   = 10,
	NFPROTO_DECNET = 12,
	NFPROTO_NUMPROTO,
};

union nf_inet_addr {
	__u32		all[4];
	__be32		ip;
	__be32		ip6[4];
	struct in_addr	in;
	struct in6_addr	in6;
};

#endif /*__LINUX_NETFILTER_H*/
