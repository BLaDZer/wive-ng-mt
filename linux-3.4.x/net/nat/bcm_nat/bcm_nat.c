/*
 *  Fastpath module for NAT speedup.
 *  This module write BCM LTD used some GPLv2 code blocks. It grants the right to change the license on GPL.
 *
 *  Some code clenup and rewrite - Tomato, wl500g and Wive-NG projects.
 *  Code refactoring and implement fastroute without nat and local connection offload by Wive-NG
 *  Ported to 3.x.x kernels by Wive-NG
 *
 *      This program is free software, you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 */

//#define DEBUG

#include "bcm_nat.h"
#include <linux/module.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/neighbour.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_conntrack_core.h>

/*
 * Direct send packets to output.
 * Stolen from ip_finish_output2 and neigh_output (need direct coding for allow gcc max optimize this)
 */
static inline int __fastpathnet bcm_fast_path_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct net_device *dev = dst->dev;
	int hh_len = LL_RESERVED_SPACE(dev);
	struct neighbour *neigh;

	/* Be paranoid, rather than too clever. */
	if (unlikely(skb_headroom(skb) < hh_len && dev->header_ops)) {
		struct sk_buff *skb2;

		skb2 = skb_realloc_headroom(skb, LL_RESERVED_SPACE(dev));
		if (skb2 == NULL) {
			kfree_skb(skb);
			return -ENOMEM;
		}
		if (skb->sk)
			skb_set_owner_w(skb2, skb->sk);
		consume_skb(skb);
		skb = skb2;
	}

	rcu_read_lock();
	neigh = dst_get_neighbour_noref(dst);
	if (likely(neigh)) {
	    int ret;
	    struct hh_cache *hh = &neigh->hh;

	    if ((neigh->nud_state & NUD_CONNECTED) && hh->hh_len)
		ret = neigh_hh_output(hh, skb);
	    else
		ret = neigh->output(neigh, skb);

	    rcu_read_unlock();
	    return (ret == 1) ? 0 : ret; /* Don't return 1 */
	}
	rcu_read_unlock();

#ifdef DEBUG
	if (net_ratelimit())
	    printk(KERN_DEBUG "bcm_fast_path_output: No header cache and no neighbour!\n");
#endif
	kfree_skb(skb);
	return -EINVAL;
}

int __fastpathnet bcm_fast_path(struct sk_buff *skb)
{
	if (skb_dst(skb) == NULL) {
		const struct iphdr *iph = ip_hdr(skb);
		int err = ip_route_input_noref(skb, iph->daddr, iph->saddr, iph->tos, skb->dev);
		if (unlikely(err)) {
			kfree_skb(skb);
			return -EINVAL;
		}

		/*  Change skb owner to output device */
		skb->dev = skb_dst(skb)->dev;
	}

	if (unlikely(skb->len > ip_skb_dst_mtu(skb) && !skb_is_gso(skb)))
		return ip_fragment(skb, bcm_fast_path_output);

	return bcm_fast_path_output(skb);
}

int __fastpathnet bcm_do_fastroute(struct nf_conn *ct,
		struct sk_buff *skb,
		unsigned int hooknum,
		int set_reply)
{
        /* change status from new to seen_reply. when receive reply packet the status will set to establish */
        if (set_reply && !test_and_set_bit(IPS_SEEN_REPLY_BIT, &ct->status))
	    nf_conntrack_event_cache(IPCT_ASSURED, ct);

	if(hooknum == NF_INET_PRE_ROUTING) {
	    FASTROUTE(skb) = 1;
	    /* this function will handle routing decision. the next hoook will be input or forward chain */
	    if (ip_rcv_finish(skb) == NF_FAST_NAT) {
	        /* Change skb owner to output device */
	        skb->dev = skb_dst(skb)->dev;
	        skb->protocol = __constant_htons(ETH_P_IP);
	        return NF_FAST_NAT;
	    }
	    /* this tell system no need to handle this packet. we will handle this. */
	    return NF_STOLEN;
	} else {
	    if(hooknum == NF_INET_LOCAL_OUT) {
		/* Change skb owner to output device */
		skb->dev = skb_dst(skb)->dev;
		skb->protocol = __constant_htons(ETH_P_IP);
		return NF_FAST_NAT;
	    }
	}

	/* return accept for continue normal processing */
	return NF_ACCEPT;
}

/*
 * NAT
 * Stolen from nf_nat_packet.
 */
extern bool manip_pkt(u_int16_t proto, struct sk_buff *skb, unsigned int iphdroff, const struct nf_conntrack_tuple *target, enum nf_nat_manip_type maniptype);

int __fastpathnet bcm_do_fastnat(struct nf_conn *ct,
		enum ip_conntrack_info ctinfo,
		struct sk_buff *skb,
		struct nf_conntrack_l3proto *l3proto,
		struct nf_conntrack_l4proto *l4proto)
{
	static int hn[2] = {NF_INET_PRE_ROUTING, NF_INET_POST_ROUTING};
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	unsigned int i = 1;

	do {
		enum nf_nat_manip_type mtype = HOOK2MANIP(hn[i]);
		unsigned long statusbit;

		if (mtype == NF_NAT_MANIP_SRC)
			statusbit = IPS_SRC_NAT;
		else
			statusbit = IPS_DST_NAT;

		/* Invert if this is reply dir. */
		if (dir == IP_CT_DIR_REPLY)
			statusbit ^= IPS_NAT_MASK;

		if (ct->status & statusbit) {
			struct nf_conntrack_tuple target;

			if (skb_dst(skb) == NULL && mtype == NF_NAT_MANIP_SRC) {
				const struct iphdr *iph = ip_hdr(skb);
				int err = ip_route_input_noref(skb, iph->daddr, iph->saddr, iph->tos, skb->dev);
				if (unlikely(err))
					return NF_DROP;

				/* Change skb owner to output device */
				skb->dev = skb_dst(skb)->dev;
			}

			/* We are aiming to look like inverse of other direction. */
			nf_ct_invert_tuple(&target, &ct->tuplehash[!dir].tuple, l3proto, l4proto);

			if (!manip_pkt(target.dst.protonum, skb, 0, &target, mtype))
				return NF_DROP;
		}
	} while (i++ < 2);

	return NF_FAST_NAT;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("Fastpath module for NAT/ROUTE offload.\n");
