/* netfilter.c: look after the filters for various protocols.
 * Heavily influenced by the old firewall.c by David Bonn and Alan Cox.
 *
 * Thanks to Rob `CmdrTaco' Malda for not influencing this code in any
 * way.
 *
 * Rusty Russell (C)2000 -- This code is GPL.
 */
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <net/net_namespace.h>
#include <net/sock.h>

#include "nf_internals.h"

#if defined(CONFIG_NETFILTER_FP_SMB)
#include <net/netfilter/nf_fp_smb.h>
#endif

static DEFINE_MUTEX(afinfo_mutex);

const struct nf_afinfo __rcu *nf_afinfo[NFPROTO_NUMPROTO] __read_mostly;
EXPORT_SYMBOL(nf_afinfo);

int nf_register_afinfo(const struct nf_afinfo *afinfo)
{
	int err;

	err = mutex_lock_interruptible(&afinfo_mutex);
	if (err < 0)
		return err;
	RCU_INIT_POINTER(nf_afinfo[afinfo->family], afinfo);
	mutex_unlock(&afinfo_mutex);
	return 0;
}
EXPORT_SYMBOL_GPL(nf_register_afinfo);

void nf_unregister_afinfo(const struct nf_afinfo *afinfo)
{
	mutex_lock(&afinfo_mutex);
	RCU_INIT_POINTER(nf_afinfo[afinfo->family], NULL);
	mutex_unlock(&afinfo_mutex);
	synchronize_rcu();
}
EXPORT_SYMBOL_GPL(nf_unregister_afinfo);

struct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS] __read_mostly;
EXPORT_SYMBOL(nf_hooks);

#if defined(CONFIG_JUMP_LABEL)
struct static_key nf_hooks_needed[NFPROTO_NUMPROTO][NF_MAX_HOOKS];
EXPORT_SYMBOL(nf_hooks_needed);
#endif

static DEFINE_MUTEX(nf_hook_mutex);

int nf_register_hook(struct nf_hook_ops *reg)
{
	struct nf_hook_ops *elem;
	int err;

	err = mutex_lock_interruptible(&nf_hook_mutex);
	if (err < 0)
		return err;
	list_for_each_entry(elem, &nf_hooks[reg->pf][reg->hooknum], list) {
		if (reg->priority < elem->priority)
			break;
	}
	list_add_rcu(&reg->list, elem->list.prev);
	mutex_unlock(&nf_hook_mutex);
#if defined(CONFIG_JUMP_LABEL)
	static_key_slow_inc(&nf_hooks_needed[reg->pf][reg->hooknum]);
#endif
	return 0;
}
EXPORT_SYMBOL(nf_register_hook);

void nf_unregister_hook(struct nf_hook_ops *reg)
{
	mutex_lock(&nf_hook_mutex);
	list_del_rcu(&reg->list);
	mutex_unlock(&nf_hook_mutex);
#if defined(CONFIG_JUMP_LABEL)
	static_key_slow_dec(&nf_hooks_needed[reg->pf][reg->hooknum]);
#endif
	synchronize_net();
}
EXPORT_SYMBOL(nf_unregister_hook);

int nf_register_hooks(struct nf_hook_ops *reg, unsigned int n)
{
	unsigned int i;
	int err = 0;

	for (i = 0; i < n; i++) {
		err = nf_register_hook(&reg[i]);
		if (err)
			goto err;
	}
	return err;

err:
	if (i > 0)
		nf_unregister_hooks(reg, i);
	return err;
}
EXPORT_SYMBOL(nf_register_hooks);

void nf_unregister_hooks(struct nf_hook_ops *reg, unsigned int n)
{
	while (n-- > 0)
		nf_unregister_hook(&reg[n]);
}
EXPORT_SYMBOL(nf_unregister_hooks);

unsigned int __fastpathnet nf_iterate(struct list_head *head,
			struct sk_buff *skb,
			unsigned int hook,
			const struct net_device *indev,
			const struct net_device *outdev,
			struct list_head **i,
			int (*okfn)(struct sk_buff *),
			int hook_thresh)
{
	unsigned int verdict;

	/*
	 * The caller must not block between calls to this
	 * function because of risk of continuing from deleted element.
	 */
	list_for_each_continue_rcu(*i, head) {
		struct nf_hook_ops *elem = (struct nf_hook_ops *)*i;

#if defined(CONFIG_BCM_NAT)
    		if (!elem->hook) {
		    NFDEBUG("nf_hook_slow: elem is empty return NF_DROP\n");
		    return NF_DROP;
		}
#endif

		if (hook_thresh > elem->priority)
			continue;

		/* Optimization: we don't need to hold module
		   reference here, since function can't sleep. --RR */
repeat:
		verdict = elem->hook(hook, skb, indev, outdev, okfn);
#if defined(CONFIG_BCM_NAT)
		if (verdict == NF_FAST_NAT)
			return NF_FAST_NAT;
#endif
		if (verdict != NF_ACCEPT) {
#ifdef CONFIG_NETFILTER_DEBUG
			if (unlikely((verdict & NF_VERDICT_MASK)
							> NF_MAX_VERDICT)) {
				NFDEBUG("Evil return from %p(%u).\n",
					elem->hook, hook);
				continue;
			}
#endif
			if (verdict != NF_REPEAT)
				return verdict;
			goto repeat;
		}
	}
	return NF_ACCEPT;
}


/* Returns 1 if okfn() needs to be executed by the caller,
 * -EPERM for NF_DROP, 0 otherwise. */
int __fastpathnet nf_hook_slow(u_int8_t pf, unsigned int hook, struct sk_buff *skb,
		 struct net_device *indev,
		 struct net_device *outdev,
		 int (*okfn)(struct sk_buff *),
		 int hook_thresh)
{
	struct list_head *elem;
	unsigned int verdict;
	int ret = 0;

	/* We may already have this, but read-locks nest anyway */
	rcu_read_lock();

	elem = &nf_hooks[pf][hook];
next_hook:
	verdict = nf_iterate(&nf_hooks[pf][hook], skb, hook, indev,
			     outdev, &elem, okfn, hook_thresh);
	if (verdict == NF_ACCEPT || verdict == NF_STOP) {
		ret = 1;
	} else if ((verdict & NF_VERDICT_MASK) == NF_DROP) {
		kfree_skb(skb);
		ret = NF_DROP_GETERR(verdict);
		if (ret == 0)
			ret = -EPERM;
#if IS_ENABLED(CONFIG_IMQ)
	} else if ((verdict & NF_VERDICT_MASK) == NF_QUEUE ||
		   (verdict & NF_VERDICT_MASK) == NF_IMQ_QUEUE) {
		int err = nf_queue(skb, elem, pf, hook, indev, outdev, okfn,
						verdict >> NF_VERDICT_QBITS,
						verdict & NF_VERDICT_MASK);
#else
	} else if ((verdict & NF_VERDICT_MASK) == NF_QUEUE) {
		int err = nf_queue(skb, elem, pf, hook, indev, outdev, okfn,
						verdict >> NF_VERDICT_QBITS);
#endif
		if (err < 0) {
			if (err == -ECANCELED)
				goto next_hook;
			if (err == -ESRCH &&
			   (verdict & NF_VERDICT_FLAG_QUEUE_BYPASS))
				goto next_hook;
			kfree_skb(skb);
		}
	}
#if defined(CONFIG_BCM_NAT)
	else if (verdict == NF_FAST_NAT) {
	    if (nf_conntrack_fastnat || nf_conntrack_fastroute) {
		ret = bcm_fast_path(skb);
	    } else {
		kfree_skb(skb);
		ret = -EPERM;
	    }
	}
#endif
	rcu_read_unlock();
	return ret;
}

int skb_make_writable(struct sk_buff *skb, unsigned int writable_len)
{
	if (writable_len > skb->len)
		return 0;

	/* Not exclusive use of packet?  Must copy. */
	if (!skb_cloned(skb)) {
		if (writable_len <= skb_headlen(skb))
			return 1;
	} else if (skb_clone_writable(skb, writable_len))
		return 1;

	if (writable_len <= skb_headlen(skb))
		writable_len = 0;
	else
		writable_len -= skb_headlen(skb);

	return !!__pskb_pull_tail(skb, writable_len);
}
EXPORT_SYMBOL(skb_make_writable);

#if IS_ENABLED(CONFIG_NF_CONNTRACK)
/* This does not belong here, but locally generated errors need it if connection
   tracking in use: without this, connection may not be in hash table, and hence
   manufactured ICMP or RST packets will not be associated with it. */
void (*ip_ct_attach)(struct sk_buff *, const struct sk_buff *)
		__rcu __read_mostly;
EXPORT_SYMBOL(ip_ct_attach);

void nf_ct_attach(struct sk_buff *new, const struct sk_buff *skb)
{
	void (*attach)(struct sk_buff *, const struct sk_buff *);

	if (skb->nfct) {
		rcu_read_lock();
		attach = rcu_dereference(ip_ct_attach);
		if (attach)
			attach(new, skb);
		rcu_read_unlock();
	}
}
EXPORT_SYMBOL(nf_ct_attach);

void (*nf_ct_destroy)(struct nf_conntrack *) __rcu __read_mostly;
EXPORT_SYMBOL(nf_ct_destroy);

void nf_conntrack_destroy(struct nf_conntrack *nfct)
{
	void (*destroy)(struct nf_conntrack *);

	rcu_read_lock();
	destroy = rcu_dereference(nf_ct_destroy);
	BUG_ON(destroy == NULL);
	destroy(nfct);
	rcu_read_unlock();
}
EXPORT_SYMBOL(nf_conntrack_destroy);
#endif /* CONFIG_NF_CONNTRACK */

#ifdef CONFIG_PROC_FS
struct proc_dir_entry *proc_net_netfilter;
EXPORT_SYMBOL(proc_net_netfilter);
#endif

void __init netfilter_init(void)
{
	int i, h;
	for (i = 0; i < ARRAY_SIZE(nf_hooks); i++) {
		for (h = 0; h < NF_MAX_HOOKS; h++)
			INIT_LIST_HEAD(&nf_hooks[i][h]);
	}

#ifdef CONFIG_PROC_FS
	proc_net_netfilter = proc_mkdir("netfilter", init_net.proc_net);
	if (!proc_net_netfilter)
		panic("cannot create netfilter proc entry");
#endif

	if (netfilter_queue_init() < 0)
		panic("cannot initialize nf_queue");
	if (netfilter_log_init() < 0)
		panic("cannot initialize nf_log");

#if defined(CONFIG_NETFILTER_FP_SMB)
	netfilter_fp_smb_init();
#endif
}

#ifdef CONFIG_SYSCTL
struct ctl_path nf_net_netfilter_sysctl_path[] = {
	{ .procname = "net", },
	{ .procname = "netfilter", },
	{ }
};
EXPORT_SYMBOL_GPL(nf_net_netfilter_sysctl_path);
#endif /* CONFIG_SYSCTL */
