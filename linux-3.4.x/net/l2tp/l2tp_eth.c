/*
 * L2TPv3 ethernet pseudowire driver
 *
 * Copyright (c) 2008,2009,2010 Katalix Systems Ltd
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/hash.h>
#include <linux/l2tp.h>
#include <linux/in.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/inet_common.h>
#include <net/inet_hashtables.h>
#include <net/tcp_states.h>
#include <net/protocol.h>
#include <net/xfrm.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>

#include "l2tp_core.h"

/* Default device name. May be overridden by name specified by user */
#define L2TP_ETH_DEV_NAME	"l2tpeth%d"

/* via netdev_priv() */
struct l2tp_eth {
	struct net_device	*dev;
	struct sock		*tunnel_sock;
	struct l2tp_session	*session;
};

/* via l2tp_session_priv() */
struct l2tp_eth_sess {
	struct net_device	*dev;
};


static struct lock_class_key l2tp_eth_tx_busylock;
static int l2tp_eth_dev_init(struct net_device *dev)
{
	struct l2tp_eth *priv = netdev_priv(dev);

	priv->dev = dev;
	eth_hw_addr_random(dev);
	memset(&dev->broadcast[0], 0xff, 6);
	dev->qdisc_tx_busylock = &l2tp_eth_tx_busylock;
	return 0;
}

static void l2tp_eth_dev_uninit(struct net_device *dev)
{
	dev_put(dev);
}

static int l2tp_eth_dev_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct l2tp_eth *priv = netdev_priv(dev);
	struct l2tp_session *session = priv->session;

	l2tp_xmit_skb(session, skb, session->hdr_len);

	dev->stats.tx_bytes += skb->len;
	dev->stats.tx_packets++;

	return 0;
}

static struct net_device_ops l2tp_eth_netdev_ops = {
	.ndo_init		= l2tp_eth_dev_init,
	.ndo_uninit		= l2tp_eth_dev_uninit,
	.ndo_start_xmit		= l2tp_eth_dev_xmit,
};

static void l2tp_eth_dev_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->priv_flags &= ~IFF_TX_SKB_SHARING;
	dev->netdev_ops		= &l2tp_eth_netdev_ops;
	dev->destructor		= free_netdev;
}

static void l2tp_eth_dev_recv(struct l2tp_session *session, struct sk_buff *skb, int data_len)
{
	struct l2tp_eth_sess *spriv = l2tp_session_priv(session);
	struct net_device *dev = spriv->dev;

	if (session->debug & L2TP_MSG_DATA) {
		unsigned int length;
		int offset;
		u8 *ptr = skb->data;

		length = min(32u, skb->len);
		if (!pskb_may_pull(skb, length))
			goto error;

		printk(KERN_DEBUG "%s: eth recv: ", session->name);

		offset = 0;
		do {
			printk(" %02X", ptr[offset]);
		} while (++offset < length);

		printk("\n");
	}

	if (!pskb_may_pull(skb, ETH_HLEN))
		goto error;
#ifdef CONFIG_XFRM
	secpath_reset(skb);
#endif
	/* checksums verified by L2TP */
	skb->ip_summed = CHECKSUM_NONE;

	skb_dst_drop(skb);
	nf_reset(skb);

	if (dev_forward_skb(dev, skb) == NET_RX_SUCCESS) {
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += data_len;
	} else
		dev->stats.rx_errors++;

	return;

error:
	dev->stats.rx_errors++;
	kfree_skb(skb);
}

static void l2tp_eth_delete(struct l2tp_session *session)
{
	struct l2tp_eth_sess *spriv;
	struct net_device *dev;

	if (session) {
		spriv = l2tp_session_priv(session);
		dev = spriv->dev;
		if (dev) {
			unregister_netdev(dev);
			spriv->dev = NULL;
			module_put(THIS_MODULE);
		}
	}
}

#if defined(CONFIG_L2TP_DEBUGFS) || defined(CONFIG_L2TP_DEBUGFS_MODULE)
static void l2tp_eth_show(struct seq_file *m, void *arg)
{
	struct l2tp_session *session = arg;
	struct l2tp_eth_sess *spriv = l2tp_session_priv(session);
	struct net_device *dev = spriv->dev;

	seq_printf(m, "   interface %s\n", dev->name);
}
#endif

static int l2tp_eth_create(struct net *net, struct l2tp_tunnel *tunnel,
			   u32 session_id, u32 peer_session_id,
			   struct l2tp_session_cfg *cfg)
{
	struct net_device *dev;
	char name[IFNAMSIZ];
	struct l2tp_session *session;
	struct l2tp_eth *priv;
	struct l2tp_eth_sess *spriv;
	int rc;

	if (cfg->ifname) {
		strlcpy(name, cfg->ifname, IFNAMSIZ);
	} else
		strcpy(name, L2TP_ETH_DEV_NAME);

	session = l2tp_session_create(sizeof(*spriv), tunnel, session_id,
				      peer_session_id, cfg);
	if (IS_ERR(session)) {
		rc = PTR_ERR(session);
		goto out;
	}

	dev = alloc_netdev(sizeof(*priv), name, l2tp_eth_dev_setup);
	if (!dev) {
		rc = -ENOMEM;
		goto out_del_session;
	}

	dev_net_set(dev, net);
	if (session->mtu == 0)
		session->mtu = dev->mtu - session->hdr_len;
	dev->mtu = session->mtu;
	dev->needed_headroom += session->hdr_len;

	priv = netdev_priv(dev);
	priv->dev = dev;
	priv->session = session;

	priv->tunnel_sock = tunnel->sock;
	session->recv_skb = l2tp_eth_dev_recv;
	session->session_close = l2tp_eth_delete;
#if defined(CONFIG_L2TP_DEBUGFS) || defined(CONFIG_L2TP_DEBUGFS_MODULE)
	session->show = l2tp_eth_show;
#endif

	spriv = l2tp_session_priv(session);
	spriv->dev = dev;

	rc = register_netdev(dev);
	if (rc < 0)
		goto out_del_dev;

	__module_get(THIS_MODULE);
	/* Must be done after register_netdev() */
	strlcpy(session->ifname, dev->name, IFNAMSIZ);

	dev_hold(dev);

	return 0;

out_del_dev:
	free_netdev(dev);
	spriv->dev = NULL;
out_del_session:
	l2tp_session_delete(session);
out:
	return rc;
}


static const struct l2tp_nl_cmd_ops l2tp_eth_nl_cmd_ops = {
	.session_create	= l2tp_eth_create,
	.session_delete	= l2tp_session_delete,
};


static int __init l2tp_eth_init(void)
{
	int err = 0;

	err = l2tp_nl_register_ops(L2TP_PWTYPE_ETH, &l2tp_eth_nl_cmd_ops);
	if (err)
		goto err;

	printk(KERN_INFO "L2TP ethernet pseudowire support (L2TPv3)\n");

	return 0;

err:
	return err;
}

static void __exit l2tp_eth_exit(void)
{
	l2tp_nl_unregister_ops(L2TP_PWTYPE_ETH);
}

module_init(l2tp_eth_init);
module_exit(l2tp_eth_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Chapman <jchapman@katalix.com>");
MODULE_DESCRIPTION("L2TP ethernet pseudowire driver");
MODULE_VERSION("1.0");
