#ifndef _BCM_NAT_WANTED
#define _BCM_NAT_WANTED

#include <linux/types.h>
#include <linux/skbuff.h>

/*
 * fastnat must full skip mcast/bcast traffic
 * allow only tcp with established/reply pkts (fix mss correction bug in syn pkts)
 * or udp proto for processing in software offload
 */
#define FASTNAT_SKIP_TYPE(skb)		((skb)->pkt_type == PACKET_BROADCAST || (skb)->pkt_type == PACKET_MULTICAST)
#define FASTNAT_SKIP_PROTO(protonum)	(protonum != IPPROTO_UDP && protonum != IPPROTO_TCP)
#define FASTNAT_ESTABLISHED(ctinfo)	(ctinfo == IP_CT_ESTABLISHED || ctinfo == IP_CT_ESTABLISHED_REPLY)

/*
 * cb fastforward flag
 * offset do not be replaced in any code (as hw_nat, strongly check this, now find max cb offset in wifi drivers = 41 max, default max cb offset is 48 see skbuf.h)
 * for safe use end of cb
 */

#define CB_FAST_ROUTE		(MAX_CB_OFFSET-1)
#define FASTROUTE(skb)		(skb->cb[CB_FAST_ROUTE])

#define CB_FAST_NAT		(MAX_CB_OFFSET-2)
#define FASTNAT(skb)		(!(skb->cb[CB_FAST_NAT])) /* not 0 - disable */
#define FASTNAT_DENY(skb)	(skb->cb[CB_FAST_NAT]=1)

/*
 * conntrack fastforward flags
 */
extern unsigned int nf_conntrack_fastnat;
extern unsigned int nf_conntrack_fastroute;
extern int bcm_fast_path(struct sk_buff *skb);

extern int nf_ct_ipv4_gather_frags(struct sk_buff *skb, u_int32_t user);
extern int ip_rcv_finish(struct sk_buff *skb);
extern int ip_skb_dst_mtu(struct sk_buff *skb);
#endif
