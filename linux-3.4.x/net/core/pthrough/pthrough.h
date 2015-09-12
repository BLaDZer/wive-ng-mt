/*
 * passthrough
 * This module is used to pass through the specific packets
 */

extern int pppoe_pt_enable;
extern int ipv6_pt_enable;

extern int pppoe_pthrough(struct sk_buff *skb);
extern int ipv6_pthrough(struct sk_buff *skb);

extern int pthrough_create_proc_entry(struct net *net);
extern void pthrough_remove_proc_entry(struct net *net);

/* return 1, if we want to handle this packet, or
 * return 0, let other ones do this */
static inline
int private_pthrough(struct sk_buff *skb)
{
	int ret = 0;
	if (pppoe_pt_enable)
		ret = pppoe_pthrough(skb);
	if (!ret && ipv6_pt_enable)
		ret = ipv6_pthrough(skb);

	return ret;
}
