/****************************************************************************
 * Driver for Solarflare Solarstorm network controllers and boards
 * Copyright 2005-2006 Fen Systems Ltd.
 * Copyright 2005-2011 Solarflare Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/crc32.h>
#include <linux/ethtool.h>
#include <linux/topology.h>
#include <linux/gfp.h>
#include <linux/cpu_rmap.h>
#include "net_driver.h"
#include "efx.h"
#include "nic.h"
#include "selftest.h"

#include "mcdi.h"
#include "workarounds.h"

/**************************************************************************
 *
 * Type name strings
 *
 **************************************************************************
 */

/* Loopback mode names (see LOOPBACK_MODE()) */
const unsigned int efx_loopback_mode_max = LOOPBACK_MAX;
const char *const efx_loopback_mode_names[] = {
	[LOOPBACK_NONE]		= "NONE",
	[LOOPBACK_DATA]		= "DATAPATH",
	[LOOPBACK_GMAC]		= "GMAC",
	[LOOPBACK_XGMII]	= "XGMII",
	[LOOPBACK_XGXS]		= "XGXS",
	[LOOPBACK_XAUI]		= "XAUI",
	[LOOPBACK_GMII]		= "GMII",
	[LOOPBACK_SGMII]	= "SGMII",
	[LOOPBACK_XGBR]		= "XGBR",
	[LOOPBACK_XFI]		= "XFI",
	[LOOPBACK_XAUI_FAR]	= "XAUI_FAR",
	[LOOPBACK_GMII_FAR]	= "GMII_FAR",
	[LOOPBACK_SGMII_FAR]	= "SGMII_FAR",
	[LOOPBACK_XFI_FAR]	= "XFI_FAR",
	[LOOPBACK_GPHY]		= "GPHY",
	[LOOPBACK_PHYXS]	= "PHYXS",
	[LOOPBACK_PCS]		= "PCS",
	[LOOPBACK_PMAPMD]	= "PMA/PMD",
	[LOOPBACK_XPORT]	= "XPORT",
	[LOOPBACK_XGMII_WS]	= "XGMII_WS",
	[LOOPBACK_XAUI_WS]	= "XAUI_WS",
	[LOOPBACK_XAUI_WS_FAR]  = "XAUI_WS_FAR",
	[LOOPBACK_XAUI_WS_NEAR] = "XAUI_WS_NEAR",
	[LOOPBACK_GMII_WS]	= "GMII_WS",
	[LOOPBACK_XFI_WS]	= "XFI_WS",
	[LOOPBACK_XFI_WS_FAR]	= "XFI_WS_FAR",
	[LOOPBACK_PHYXS_WS]	= "PHYXS_WS",
};

const unsigned int efx_reset_type_max = RESET_TYPE_MAX;
const char *const efx_reset_type_names[] = {
	[RESET_TYPE_INVISIBLE]     = "INVISIBLE",
	[RESET_TYPE_ALL]           = "ALL",
	[RESET_TYPE_WORLD]         = "WORLD",
	[RESET_TYPE_DISABLE]       = "DISABLE",
	[RESET_TYPE_TX_WATCHDOG]   = "TX_WATCHDOG",
	[RESET_TYPE_INT_ERROR]     = "INT_ERROR",
	[RESET_TYPE_RX_RECOVERY]   = "RX_RECOVERY",
	[RESET_TYPE_RX_DESC_FETCH] = "RX_DESC_FETCH",
	[RESET_TYPE_TX_DESC_FETCH] = "TX_DESC_FETCH",
	[RESET_TYPE_TX_SKIP]       = "TX_SKIP",
	[RESET_TYPE_MC_FAILURE]    = "MC_FAILURE",
};

#define EFX_MAX_MTU (9 * 1024)

/* Reset workqueue. If any NIC has a hardware failure then a reset will be
 * queued onto this work queue. This is not a per-nic work queue, because
 * efx_reset_work() acquires the rtnl lock, so resets are naturally serialised.
 */
static struct workqueue_struct *reset_workqueue;

/**************************************************************************
 *
 * Configurable values
 *
 *************************************************************************/

/*
 * Use separate channels for TX and RX events
 *
 * Set this to 1 to use separate channels for TX and RX. It allows us
 * to control interrupt affinity separately for TX and RX.
 *
 * This is only used in MSI-X interrupt mode
 */
static unsigned int separate_tx_channels;
module_param(separate_tx_channels, uint, 0444);
MODULE_PARM_DESC(separate_tx_channels,
		 "Use separate channels for TX and RX");

/* This is the weight assigned to each of the (per-channel) virtual
 * NAPI devices.
 */
static int napi_weight = 64;

/* This is the time (in jiffies) between invocations of the hardware
 * monitor.  On Falcon-based NICs, this will:
 * - Check the on-board hardware monitor;
 * - Poll the link state and reconfigure the hardware as necessary.
 */
static unsigned int efx_monitor_interval = 1 * HZ;

/* Initial interrupt moderation settings.  They can be modified after
 * module load with ethtool.
 *
 * The default for RX should strike a balance between increasing the
 * round-trip latency and reducing overhead.
 */
static unsigned int rx_irq_mod_usec = 60;

/* Initial interrupt moderation settings.  They can be modified after
 * module load with ethtool.
 *
 * This default is chosen to ensure that a 10G link does not go idle
 * while a TX queue is stopped after it has become full.  A queue is
 * restarted when it drops below half full.  The time this takes (assuming
 * worst case 3 descriptors per packet and 1024 descriptors) is
 *   512 / 3 * 1.2 = 205 usec.
 */
static unsigned int tx_irq_mod_usec = 150;

/* This is the first interrupt mode to try out of:
 * 0 => MSI-X
 * 1 => MSI
 * 2 => legacy
 */
static unsigned int interrupt_mode;

/* This is the requested number of CPUs to use for Receive-Side Scaling (RSS),
 * i.e. the number of CPUs among which we may distribute simultaneous
 * interrupt handling.
 *
 * Cards without MSI-X will only target one CPU via legacy or MSI interrupt.
 * The default (0) means to assign an interrupt to each core.
 */
static unsigned int rss_cpus;
module_param(rss_cpus, uint, 0444);
MODULE_PARM_DESC(rss_cpus, "Number of CPUs to use for Receive-Side Scaling");

static int phy_flash_cfg;
module_param(phy_flash_cfg, int, 0644);
MODULE_PARM_DESC(phy_flash_cfg, "Set PHYs into reflash mode initially");

static unsigned irq_adapt_low_thresh = 8000;
module_param(irq_adapt_low_thresh, uint, 0644);
MODULE_PARM_DESC(irq_adapt_low_thresh,
		 "Threshold score for reducing IRQ moderation");

static unsigned irq_adapt_high_thresh = 16000;
module_param(irq_adapt_high_thresh, uint, 0644);
MODULE_PARM_DESC(irq_adapt_high_thresh,
		 "Threshold score for increasing IRQ moderation");

static unsigned debug = (NETIF_MSG_DRV | NETIF_MSG_PROBE |
			 NETIF_MSG_LINK | NETIF_MSG_IFDOWN |
			 NETIF_MSG_IFUP | NETIF_MSG_RX_ERR |
			 NETIF_MSG_TX_ERR | NETIF_MSG_HW);
module_param(debug, uint, 0);
MODULE_PARM_DESC(debug, "Bitmapped debugging message enable value");

/**************************************************************************
 *
 * Utility functions and prototypes
 *
 *************************************************************************/

static void efx_start_interrupts(struct efx_nic *efx, bool may_keep_eventq);
static void efx_stop_interrupts(struct efx_nic *efx, bool may_keep_eventq);
static void efx_remove_channel(struct efx_channel *channel);
static void efx_remove_channels(struct efx_nic *efx);
static const struct efx_channel_type efx_default_channel_type;
static void efx_remove_port(struct efx_nic *efx);
static void efx_init_napi_channel(struct efx_channel *channel);
static void efx_fini_napi(struct efx_nic *efx);
static void efx_fini_napi_channel(struct efx_channel *channel);
static void efx_fini_struct(struct efx_nic *efx);
static void efx_start_all(struct efx_nic *efx);
static void efx_stop_all(struct efx_nic *efx);

#define EFX_ASSERT_RESET_SERIALISED(efx)		\
	do {						\
		if ((efx->state == STATE_RUNNING) ||	\
		    (efx->state == STATE_DISABLED))	\
			ASSERT_RTNL();			\
	} while (0)

/**************************************************************************
 *
 * Event queue processing
 *
 *************************************************************************/

/* Process channel's event queue
 *
 * This function is responsible for processing the event queue of a
 * single channel.  The caller must guarantee that this function will
 * never be concurrently called more than once on the same channel,
 * though different channels may be being processed concurrently.
 */
static int efx_process_channel(struct efx_channel *channel, int budget)
{
	int spent;

	if (unlikely(!channel->enabled))
		return 0;

	spent = efx_nic_process_eventq(channel, budget);
	if (spent && efx_channel_has_rx_queue(channel)) {
		struct efx_rx_queue *rx_queue =
			efx_channel_get_rx_queue(channel);

		/* Deliver last RX packet. */
		if (channel->rx_pkt) {
			__efx_rx_packet(channel, channel->rx_pkt);
			channel->rx_pkt = NULL;
		}
		if (rx_queue->enabled) {
			efx_rx_strategy(channel);
			efx_fast_push_rx_descriptors(rx_queue);
		}
	}

	return spent;
}

/* Mark channel as finished processing
 *
 * Note that since we will not receive further interrupts for this
 * channel before we finish processing and call the eventq_read_ack()
 * method, there is no need to use the interrupt hold-off timers.
 */
static inline void efx_channel_processed(struct efx_channel *channel)
{
	/* The interrupt handler for this channel may set work_pending
	 * as soon as we acknowledge the events we've seen.  Make sure
	 * it's cleared before then. */
	channel->work_pending = false;
	smp_wmb();

	efx_nic_eventq_read_ack(channel);
}

/* NAPI poll handler
 *
 * NAPI guarantees serialisation of polls of the same device, which
 * provides the guarantee required by efx_process_channel().
 */
static int efx_poll(struct napi_struct *napi, int budget)
{
	struct efx_channel *channel =
		container_of(napi, struct efx_channel, napi_str);
	struct efx_nic *efx = channel->efx;
	int spent;

	netif_vdbg(efx, intr, efx->net_dev,
		   "channel %d NAPI poll executing on CPU %d\n",
		   channel->channel, raw_smp_processor_id());

	spent = efx_process_channel(channel, budget);

	if (spent < budget) {
		if (efx_channel_has_rx_queue(channel) &&
		    efx->irq_rx_adaptive &&
		    unlikely(++channel->irq_count == 1000)) {
			if (unlikely(channel->irq_mod_score <
				     irq_adapt_low_thresh)) {
				if (channel->irq_moderation > 1) {
					channel->irq_moderation -= 1;
					efx->type->push_irq_moderation(channel);
				}
			} else if (unlikely(channel->irq_mod_score >
					    irq_adapt_high_thresh)) {
				if (channel->irq_moderation <
				    efx->irq_rx_moderation) {
					channel->irq_moderation += 1;
					efx->type->push_irq_moderation(channel);
				}
			}
			channel->irq_count = 0;
			channel->irq_mod_score = 0;
		}

		efx_filter_rfs_expire(channel);

		/* There is no race here; although napi_disable() will
		 * only wait for napi_complete(), this isn't a problem
		 * since efx_channel_processed() will have no effect if
		 * interrupts have already been disabled.
		 */
		napi_complete(napi);
		efx_channel_processed(channel);
	}

	return spent;
}

/* Process the eventq of the specified channel immediately on this CPU
 *
 * Disable hardware generated interrupts, wait for any existing
 * processing to finish, then directly poll (and ack ) the eventq.
 * Finally reenable NAPI and interrupts.
 *
 * This is for use only during a loopback self-test.  It must not
 * deliver any packets up the stack as this can result in deadlock.
 */
void efx_process_channel_now(struct efx_channel *channel)
{
	struct efx_nic *efx = channel->efx;

	BUG_ON(channel->channel >= efx->n_channels);
	BUG_ON(!channel->enabled);
	BUG_ON(!efx->loopback_selftest);

	/* Disable interrupts and wait for ISRs to complete */
	efx_nic_disable_interrupts(efx);
	if (efx->legacy_irq) {
		synchronize_irq(efx->legacy_irq);
		efx->legacy_irq_enabled = false;
	}
	if (channel->irq)
		synchronize_irq(channel->irq);

	/* Wait for any NAPI processing to complete */
	napi_disable(&channel->napi_str);

	/* Poll the channel */
	efx_process_channel(channel, channel->eventq_mask + 1);

	/* Ack the eventq. This may cause an interrupt to be generated
	 * when they are reenabled */
	efx_channel_processed(channel);

	napi_enable(&channel->napi_str);
	if (efx->legacy_irq)
		efx->legacy_irq_enabled = true;
	efx_nic_enable_interrupts(efx);
}

/* Create event queue
 * Event queue memory allocations are done only once.  If the channel
 * is reset, the memory buffer will be reused; this guards against
 * errors during channel reset and also simplifies interrupt handling.
 */
static int efx_probe_eventq(struct efx_channel *channel)
{
	struct efx_nic *efx = channel->efx;
	unsigned long entries;

	netif_dbg(efx, probe, efx->net_dev,
		  "chan %d create event queue\n", channel->channel);

	/* Build an event queue with room for one event per tx and rx buffer,
	 * plus some extra for link state events and MCDI completions. */
	entries = roundup_pow_of_two(efx->rxq_entries + efx->txq_entries + 128);
	EFX_BUG_ON_PARANOID(entries > EFX_MAX_EVQ_SIZE);
	channel->eventq_mask = max(entries, EFX_MIN_EVQ_SIZE) - 1;

	return efx_nic_probe_eventq(channel);
}

/* Prepare channel's event queue */
static void efx_init_eventq(struct efx_channel *channel)
{
	netif_dbg(channel->efx, drv, channel->efx->net_dev,
		  "chan %d init event queue\n", channel->channel);

	channel->eventq_read_ptr = 0;

	efx_nic_init_eventq(channel);
}

/* Enable event queue processing and NAPI */
static void efx_start_eventq(struct efx_channel *channel)
{
	netif_dbg(channel->efx, ifup, channel->efx->net_dev,
		  "chan %d start event queue\n", channel->channel);

	/* The interrupt handler for this channel may set work_pending
	 * as soon as we enable it.  Make sure it's cleared before
	 * then.  Similarly, make sure it sees the enabled flag set.
	 */
	channel->work_pending = false;
	channel->enabled = true;
	smp_wmb();

	napi_enable(&channel->napi_str);
	efx_nic_eventq_read_ack(channel);
}

/* Disable event queue processing and NAPI */
static void efx_stop_eventq(struct efx_channel *channel)
{
	if (!channel->enabled)
		return;

	napi_disable(&channel->napi_str);
	channel->enabled = false;
}

static void efx_fini_eventq(struct efx_channel *channel)
{
	netif_dbg(channel->efx, drv, channel->efx->net_dev,
		  "chan %d fini event queue\n", channel->channel);

	efx_nic_fini_eventq(channel);
}

static void efx_remove_eventq(struct efx_channel *channel)
{
	netif_dbg(channel->efx, drv, channel->efx->net_dev,
		  "chan %d remove event queue\n", channel->channel);

	efx_nic_remove_eventq(channel);
}

/**************************************************************************
 *
 * Channel handling
 *
 *************************************************************************/

/* Allocate and initialise a channel structure. */
static struct efx_channel *
efx_alloc_channel(struct efx_nic *efx, int i, struct efx_channel *old_channel)
{
	struct efx_channel *channel;
	struct efx_rx_queue *rx_queue;
	struct efx_tx_queue *tx_queue;
	int j;

	channel = kzalloc(sizeof(*channel), GFP_KERNEL);
	if (!channel)
		return NULL;

	channel->efx = efx;
	channel->channel = i;
	channel->type = &efx_default_channel_type;

	for (j = 0; j < EFX_TXQ_TYPES; j++) {
		tx_queue = &channel->tx_queue[j];
		tx_queue->efx = efx;
		tx_queue->queue = i * EFX_TXQ_TYPES + j;
		tx_queue->channel = channel;
	}

	rx_queue = &channel->rx_queue;
	rx_queue->efx = efx;
	setup_timer(&rx_queue->slow_fill, efx_rx_slow_fill,
		    (unsigned long)rx_queue);

	return channel;
}

/* Allocate and initialise a channel structure, copying parameters
 * (but not resources) from an old channel structure.
 */
static struct efx_channel *
efx_copy_channel(const struct efx_channel *old_channel)
{
	struct efx_channel *channel;
	struct efx_rx_queue *rx_queue;
	struct efx_tx_queue *tx_queue;
	int j;

	channel = kmalloc(sizeof(*channel), GFP_KERNEL);
	if (!channel)
		return NULL;

	*channel = *old_channel;

	channel->napi_dev = NULL;
	memset(&channel->eventq, 0, sizeof(channel->eventq));

	for (j = 0; j < EFX_TXQ_TYPES; j++) {
		tx_queue = &channel->tx_queue[j];
		if (tx_queue->channel)
			tx_queue->channel = channel;
		tx_queue->buffer = NULL;
		memset(&tx_queue->txd, 0, sizeof(tx_queue->txd));
	}

	rx_queue = &channel->rx_queue;
	rx_queue->buffer = NULL;
	memset(&rx_queue->rxd, 0, sizeof(rx_queue->rxd));
	setup_timer(&rx_queue->slow_fill, efx_rx_slow_fill,
		    (unsigned long)rx_queue);

	return channel;
}

static int efx_probe_channel(struct efx_channel *channel)
{
	struct efx_tx_queue *tx_queue;
	struct efx_rx_queue *rx_queue;
	int rc;

	netif_dbg(channel->efx, probe, channel->efx->net_dev,
		  "creating channel %d\n", channel->channel);

	rc = channel->type->pre_probe(channel);
	if (rc)
		goto fail;

	rc = efx_probe_eventq(channel);
	if (rc)
		goto fail;

	efx_for_each_channel_tx_queue(tx_queue, channel) {
		rc = efx_probe_tx_queue(tx_queue);
		if (rc)
			goto fail;
	}

	efx_for_each_channel_rx_queue(rx_queue, channel) {
		rc = efx_probe_rx_queue(rx_queue);
		if (rc)
			goto fail;
	}

	channel->n_rx_frm_trunc = 0;

	return 0;

fail:
	efx_remove_channel(channel);
	return rc;
}

static void
efx_get_channel_name(struct efx_channel *channel, char *buf, size_t len)
{
	struct efx_nic *efx = channel->efx;
	const char *type;
	int number;

	number = channel->channel;
	if (efx->tx_channel_offset == 0) {
		type = "";
	} else if (channel->channel < efx->tx_channel_offset) {
		type = "-rx";
	} else {
		type = "-tx";
		number -= efx->tx_channel_offset;
	}
	snprintf(buf, len, "%s%s-%d", efx->name, type, number);
}

static void efx_set_channel_names(struct efx_nic *efx)
{
	struct efx_channel *channel;

	efx_for_each_channel(channel, efx)
		channel->type->get_name(channel,
					efx->channel_name[channel->channel],
					sizeof(efx->channel_name[0]));
}

static int efx_probe_channels(struct efx_nic *efx)
{
	struct efx_channel *channel;
	int rc;

	/* Restart special buffer allocation */
	efx->next_buffer_table = 0;

	/* Probe channels in reverse, so that any 'extra' channels
	 * use the start of the buffer table. This allows the traffic
	 * channels to be resized without moving them or wasting the
	 * entries before them.
	 */
	efx_for_each_channel_rev(channel, efx) {
		rc = efx_probe_channel(channel);
		if (rc) {
			netif_err(efx, probe, efx->net_dev,
				  "failed to create channel %d\n",
				  channel->channel);
			goto fail;
		}
	}
	efx_set_channel_names(efx);

	return 0;

fail:
	efx_remove_channels(efx);
	return rc;
}

/* Channels are shutdown and reinitialised whilst the NIC is running
 * to propagate configuration changes (mtu, checksum offload), or
 * to clear hardware error conditions
 */
static void efx_start_datapath(struct efx_nic *efx)
{
	struct efx_tx_queue *tx_queue;
	struct efx_rx_queue *rx_queue;
	struct efx_channel *channel;

	/* Calculate the rx buffer allocation parameters required to
	 * support the current MTU, including padding for header
	 * alignment and overruns.
	 */
	efx->rx_buffer_len = (max(EFX_PAGE_IP_ALIGN, NET_IP_ALIGN) +
			      EFX_MAX_FRAME_LEN(efx->net_dev->mtu) +
			      efx->type->rx_buffer_hash_size +
			      efx->type->rx_buffer_padding);
	efx->rx_buffer_order = get_order(efx->rx_buffer_len +
					 sizeof(struct efx_rx_page_state));

	/* Initialise the channels */
	efx_for_each_channel(channel, efx) {
		efx_for_each_channel_tx_queue(tx_queue, channel)
			efx_init_tx_queue(tx_queue);

		/* The rx buffer allocation strategy is MTU dependent */
		efx_rx_strategy(channel);

		efx_for_each_channel_rx_queue(rx_queue, channel) {
			efx_init_rx_queue(rx_queue);
			efx_nic_generate_fill_event(rx_queue);
		}

		WARN_ON(channel->rx_pkt != NULL);
		efx_rx_strategy(channel);
	}

	if (netif_device_present(efx->net_dev))
		netif_tx_wake_all_queues(efx->net_dev);
}

static void efx_stop_datapath(struct efx_nic *efx)
{
	struct efx_channel *channel;
	struct efx_tx_queue *tx_queue;
	struct efx_rx_queue *rx_queue;
	struct pci_dev *dev = efx->pci_dev;
	int rc;

	EFX_ASSERT_RESET_SERIALISED(efx);
	BUG_ON(efx->port_enabled);

	/* Only perform flush if dma is enabled */
	if (dev->is_busmaster) {
		rc = efx_nic_flush_queues(efx);

		if (rc && EFX_WORKAROUND_7803(efx)) {
			/* Schedule a reset to recover from the flush failure. The
			 * descriptor caches reference memory we're about to free,
			 * but falcon_reconfigure_mac_wrapper() won't reconnect
			 * the MACs because of the pending reset. */
			netif_err(efx, drv, efx->net_dev,
				  "Resetting to recover from flush failure\n");
			efx_schedule_reset(efx, RESET_TYPE_ALL);
		} else if (rc) {
			netif_err(efx, drv, efx->net_dev, "failed to flush queues\n");
		} else {
			netif_dbg(efx, drv, efx->net_dev,
				  "successfully flushed all queues\n");
		}
	}

	efx_for_each_channel(channel, efx) {
		/* RX packet processing is pipelined, so wait for the
		 * NAPI handler to complete.  At least event queue 0
		 * might be kept active by non-data events, so don't
		 * use napi_synchronize() but actually disable NAPI
		 * temporarily.
		 */
		if (efx_channel_has_rx_queue(channel)) {
			efx_stop_eventq(channel);
			efx_start_eventq(channel);
		}

		efx_for_each_channel_rx_queue(rx_queue, channel)
			efx_fini_rx_queue(rx_queue);
		efx_for_each_possible_channel_tx_queue(tx_queue, channel)
			efx_fini_tx_queue(tx_queue);
	}
}

static void efx_remove_channel(struct efx_channel *channel)
{
	struct efx_tx_queue *tx_queue;
	struct efx_rx_queue *rx_queue;

	netif_dbg(channel->efx, drv, channel->efx->net_dev,
		  "destroy chan %d\n", channel->channel);

	efx_for_each_channel_rx_queue(rx_queue, channel)
		efx_remove_rx_queue(rx_queue);
	efx_for_each_possible_channel_tx_queue(tx_queue, channel)
		efx_remove_tx_queue(tx_queue);
	efx_remove_eventq(channel);
}

static void efx_remove_channels(struct efx_nic *efx)
{
	struct efx_channel *channel;

	efx_for_each_channel(channel, efx)
		efx_remove_channel(channel);
}

int
efx_realloc_channels(struct efx_nic *efx, u32 rxq_entries, u32 txq_entries)
{
	struct efx_channel *other_channel[EFX_MAX_CHANNELS], *channel;
	u32 old_rxq_entries, old_txq_entries;
	unsigned i, next_buffer_table = 0;
	int rc = 0;

	/* Not all channels should be reallocated. We must avoid
	 * reallocating their buffer table entries.
	 */
	efx_for_each_channel(channel, efx) {
		struct efx_rx_queue *rx_queue;
		struct efx_tx_queue *tx_queue;

		if (channel->type->copy)
			continue;
		next_buffer_table = max(next_buffer_table,
					channel->eventq.index +
					channel->eventq.entries);
		efx_for_each_channel_rx_queue(rx_queue, channel)
			next_buffer_table = max(next_buffer_table,
						rx_queue->rxd.index +
						rx_queue->rxd.entries);
		efx_for_each_channel_tx_queue(tx_queue, channel)
			next_buffer_table = max(next_buffer_table,
						tx_queue->txd.index +
						tx_queue->txd.entries);
	}

	efx_device_detach_sync(efx);
	efx_stop_all(efx);
	efx_stop_interrupts(efx, true);

	/* Clone channels (where possible) */
	memset(other_channel, 0, sizeof(other_channel));
	for (i = 0; i < efx->n_channels; i++) {
		channel = efx->channel[i];
		if (channel->type->copy)
			channel = channel->type->copy(channel);
		if (!channel) {
			rc = -ENOMEM;
			goto out;
		}
		other_channel[i] = channel;
	}

	/* Swap entry counts and channel pointers */
	old_rxq_entries = efx->rxq_entries;
	old_txq_entries = efx->txq_entries;
	efx->rxq_entries = rxq_entries;
	efx->txq_entries = txq_entries;
	for (i = 0; i < efx->n_channels; i++) {
		channel = efx->channel[i];
		efx->channel[i] = other_channel[i];
		other_channel[i] = channel;
	}

	/* Restart buffer table allocation */
	efx->next_buffer_table = next_buffer_table;

	for (i = 0; i < efx->n_channels; i++) {
		channel = efx->channel[i];
		if (!channel->type->copy)
			continue;
		rc = efx_probe_channel(channel);
		if (rc)
			goto rollback;
		efx_init_napi_channel(efx->channel[i]);
	}

out:
	/* Destroy unused channel structures */
	for (i = 0; i < efx->n_channels; i++) {
		channel = other_channel[i];
		if (channel && channel->type->copy) {
			efx_fini_napi_channel(channel);
			efx_remove_channel(channel);
			kfree(channel);
		}
	}

	efx_start_interrupts(efx, true);
	efx_start_all(efx);
	netif_device_attach(efx->net_dev);
	return rc;

rollback:
	/* Swap back */
	efx->rxq_entries = old_rxq_entries;
	efx->txq_entries = old_txq_entries;
	for (i = 0; i < efx->n_channels; i++) {
		channel = efx->channel[i];
		efx->channel[i] = other_channel[i];
		other_channel[i] = channel;
	}
	goto out;
}

void efx_schedule_slow_fill(struct efx_rx_queue *rx_queue)
{
	mod_timer(&rx_queue->slow_fill, jiffies + msecs_to_jiffies(100));
}

static const struct efx_channel_type efx_default_channel_type = {
	.pre_probe		= efx_channel_dummy_op_int,
	.get_name		= efx_get_channel_name,
	.copy			= efx_copy_channel,
	.keep_eventq		= false,
};

int efx_channel_dummy_op_int(struct efx_channel *channel)
{
	return 0;
}

/**************************************************************************
 *
 * Port handling
 *
 **************************************************************************/

/* This ensures that the kernel is kept informed (via
 * netif_carrier_on/off) of the link status, and also maintains the
 * link status's stop on the port's TX queue.
 */
void efx_link_status_changed(struct efx_nic *efx)
{
	struct efx_link_state *link_state = &efx->link_state;

	/* SFC Bug 5356: A net_dev notifier is registered, so we must ensure
	 * that no events are triggered between unregister_netdev() and the
	 * driver unloading. A more general condition is that NETDEV_CHANGE
	 * can only be generated between NETDEV_UP and NETDEV_DOWN */
	if (!netif_running(efx->net_dev))
		return;

	if (link_state->up != netif_carrier_ok(efx->net_dev)) {
		efx->n_link_state_changes++;

		if (link_state->up)
			netif_carrier_on(efx->net_dev);
		else
			netif_carrier_off(efx->net_dev);
	}

	/* Status message for kernel log */
	if (link_state->up)
		netif_info(efx, link, efx->net_dev,
			   "link up at %uMbps %s-duplex (MTU %d)%s\n",
			   link_state->speed, link_state->fd ? "full" : "half",
			   efx->net_dev->mtu,
			   (efx->promiscuous ? " [PROMISC]" : ""));
	else
		netif_info(efx, link, efx->net_dev, "link down\n");
}

void efx_link_set_advertising(struct efx_nic *efx, u32 advertising)
{
	efx->link_advertising = advertising;
	if (advertising) {
		if (advertising & ADVERTISED_Pause)
			efx->wanted_fc |= (EFX_FC_TX | EFX_FC_RX);
		else
			efx->wanted_fc &= ~(EFX_FC_TX | EFX_FC_RX);
		if (advertising & ADVERTISED_Asym_Pause)
			efx->wanted_fc ^= EFX_FC_TX;
	}
}

void efx_link_set_wanted_fc(struct efx_nic *efx, u8 wanted_fc)
{
	efx->wanted_fc = wanted_fc;
	if (efx->link_advertising) {
		if (wanted_fc & EFX_FC_RX)
			efx->link_advertising |= (ADVERTISED_Pause |
						  ADVERTISED_Asym_Pause);
		else
			efx->link_advertising &= ~(ADVERTISED_Pause |
						   ADVERTISED_Asym_Pause);
		if (wanted_fc & EFX_FC_TX)
			efx->link_advertising ^= ADVERTISED_Asym_Pause;
	}
}

static void efx_fini_port(struct efx_nic *efx);

/* Push loopback/power/transmit disable settings to the PHY, and reconfigure
 * the MAC appropriately. All other PHY configuration changes are pushed
 * through phy_op->set_settings(), and pushed asynchronously to the MAC
 * through efx_monitor().
 *
 * Callers must hold the mac_lock
 */
int __efx_reconfigure_port(struct efx_nic *efx)
{
	enum efx_phy_mode phy_mode;
	int rc;

	WARN_ON(!mutex_is_locked(&efx->mac_lock));

	/* Serialise the promiscuous flag with efx_set_rx_mode. */
	netif_addr_lock_bh(efx->net_dev);
	netif_addr_unlock_bh(efx->net_dev);

	/* Disable PHY transmit in mac level loopbacks */
	phy_mode = efx->phy_mode;
	if (LOOPBACK_INTERNAL(efx))
		efx->phy_mode |= PHY_MODE_TX_DISABLED;
	else
		efx->phy_mode &= ~PHY_MODE_TX_DISABLED;

	rc = efx->type->reconfigure_port(efx);

	if (rc)
		efx->phy_mode = phy_mode;

	return rc;
}

/* Reinitialise the MAC to pick up new PHY settings, even if the port is
 * disabled. */
int efx_reconfigure_port(struct efx_nic *efx)
{
	int rc;

	EFX_ASSERT_RESET_SERIALISED(efx);

	mutex_lock(&efx->mac_lock);
	rc = __efx_reconfigure_port(efx);
	mutex_unlock(&efx->mac_lock);

	return rc;
}

/* Asynchronous work item for changing MAC promiscuity and multicast
 * hash.  Avoid a drain/rx_ingress enable by reconfiguring the current
 * MAC directly. */
static void efx_mac_work(struct work_struct *data)
{
	struct efx_nic *efx = container_of(data, struct efx_nic, mac_work);

	mutex_lock(&efx->mac_lock);
	if (efx->port_enabled)
		efx->type->reconfigure_mac(efx);
	mutex_unlock(&efx->mac_lock);
}

static int efx_probe_port(struct efx_nic *efx)
{
	int rc;

	netif_dbg(efx, probe, efx->net_dev, "create port\n");

	if (phy_flash_cfg)
		efx->phy_mode = PHY_MODE_SPECIAL;

	/* Connect up MAC/PHY operations table */
	rc = efx->type->probe_port(efx);
	if (rc)
		return rc;

	/* Initialise MAC address to permanent address */
	memcpy(efx->net_dev->dev_addr, efx->net_dev->perm_addr, ETH_ALEN);

	return 0;
}

static int efx_init_port(struct efx_nic *efx)
{
	int rc;

	netif_dbg(efx, drv, efx->net_dev, "init port\n");

	mutex_lock(&efx->mac_lock);

	rc = efx->phy_op->init(efx);
	if (rc)
		goto fail1;

	efx->port_initialized = true;

	/* Reconfigure the MAC before creating dma queues (required for
	 * Falcon/A1 where RX_INGR_EN/TX_DRAIN_EN isn't supported) */
	efx->type->reconfigure_mac(efx);

	/* Ensure the PHY advertises the correct flow control settings */
	rc = efx->phy_op->reconfigure(efx);
	if (rc)
		goto fail2;

	mutex_unlock(&efx->mac_lock);
	return 0;

fail2:
	efx->phy_op->fini(efx);
fail1:
	mutex_unlock(&efx->mac_lock);
	return rc;
}

static void efx_start_port(struct efx_nic *efx)
{
	netif_dbg(efx, ifup, efx->net_dev, "start port\n");
	BUG_ON(efx->port_enabled);

	mutex_lock(&efx->mac_lock);
	efx->port_enabled = true;

	/* efx_mac_work() might have been scheduled after efx_stop_port(),
	 * and then cancelled by efx_flush_all() */
	efx->type->reconfigure_mac(efx);

	mutex_unlock(&efx->mac_lock);
}

/* Prevent efx_mac_work() and efx_monitor() from working */
static void efx_stop_port(struct efx_nic *efx)
{
	netif_dbg(efx, ifdown, efx->net_dev, "stop port\n");

	mutex_lock(&efx->mac_lock);
	efx->port_enabled = false;
	mutex_unlock(&efx->mac_lock);

	/* Serialise against efx_set_multicast_list() */
	netif_addr_lock_bh(efx->net_dev);
	netif_addr_unlock_bh(efx->net_dev);
}

static void efx_fini_port(struct efx_nic *efx)
{
	netif_dbg(efx, drv, efx->net_dev, "shut down port\n");

	if (!efx->port_initialized)
		return;

	efx->phy_op->fini(efx);
	efx->port_initialized = false;

	efx->link_state.up = false;
	efx_link_status_changed(efx);
}

static void efx_remove_port(struct efx_nic *efx)
{
	netif_dbg(efx, drv, efx->net_dev, "destroying port\n");

	efx->type->remove_port(efx);
}

/**************************************************************************
 *
 * NIC handling
 *
 **************************************************************************/

/* This configures the PCI device to enable I/O and DMA. */
static int efx_init_io(struct efx_nic *efx)
{
	struct pci_dev *pci_dev = efx->pci_dev;
	dma_addr_t dma_mask = efx->type->max_dma_mask;
	int rc;

	netif_dbg(efx, probe, efx->net_dev, "initialising I/O\n");

	rc = pci_enable_device(pci_dev);
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "failed to enable PCI device\n");
		goto fail1;
	}

	pci_set_master(pci_dev);

	/* Set the PCI DMA mask.  Try all possibilities from our
	 * genuine mask down to 32 bits, because some architectures
	 * (e.g. x86_64 with iommu_sac_force set) will allow 40 bit
	 * masks event though they reject 46 bit masks.
	 */
	while (dma_mask > 0x7fffffffUL) {
		if (pci_dma_supported(pci_dev, dma_mask)) {
			rc = pci_set_dma_mask(pci_dev, dma_mask);
			if (rc == 0)
				break;
		}
		dma_mask >>= 1;
	}
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "could not find a suitable DMA mask\n");
		goto fail2;
	}
	netif_dbg(efx, probe, efx->net_dev,
		  "using DMA mask %llx\n", (unsigned long long) dma_mask);
	rc = pci_set_consistent_dma_mask(pci_dev, dma_mask);
	if (rc) {
		/* pci_set_consistent_dma_mask() is not *allowed* to
		 * fail with a mask that pci_set_dma_mask() accepted,
		 * but just in case...
		 */
		netif_err(efx, probe, efx->net_dev,
			  "failed to set consistent DMA mask\n");
		goto fail2;
	}

	efx->membase_phys = pci_resource_start(efx->pci_dev, EFX_MEM_BAR);
	rc = pci_request_region(pci_dev, EFX_MEM_BAR, "sfc");
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "request for memory BAR failed\n");
		rc = -EIO;
		goto fail3;
	}
	efx->membase = ioremap_nocache(efx->membase_phys,
				       efx->type->mem_map_size);
	if (!efx->membase) {
		netif_err(efx, probe, efx->net_dev,
			  "could not map memory BAR at %llx+%x\n",
			  (unsigned long long)efx->membase_phys,
			  efx->type->mem_map_size);
		rc = -ENOMEM;
		goto fail4;
	}
	netif_dbg(efx, probe, efx->net_dev,
		  "memory BAR at %llx+%x (virtual %p)\n",
		  (unsigned long long)efx->membase_phys,
		  efx->type->mem_map_size, efx->membase);

	return 0;

 fail4:
	pci_release_region(efx->pci_dev, EFX_MEM_BAR);
 fail3:
	efx->membase_phys = 0;
 fail2:
	pci_disable_device(efx->pci_dev);
 fail1:
	return rc;
}

static void efx_fini_io(struct efx_nic *efx)
{
	netif_dbg(efx, drv, efx->net_dev, "shutting down I/O\n");

	if (efx->membase) {
		iounmap(efx->membase);
		efx->membase = NULL;
	}

	if (efx->membase_phys) {
		pci_release_region(efx->pci_dev, EFX_MEM_BAR);
		efx->membase_phys = 0;
	}

	pci_disable_device(efx->pci_dev);
}

static unsigned int efx_wanted_parallelism(struct efx_nic *efx)
{
	cpumask_var_t thread_mask;
	unsigned int count;
	int cpu;

	if (rss_cpus) {
		count = rss_cpus;
	} else {
		if (unlikely(!zalloc_cpumask_var(&thread_mask, GFP_KERNEL))) {
			netif_warn(efx, probe, efx->net_dev,
				   "RSS disabled due to allocation failure\n");
			return 1;
		}

		count = 0;
		for_each_online_cpu(cpu) {
			if (!cpumask_test_cpu(cpu, thread_mask)) {
				++count;
				cpumask_or(thread_mask, thread_mask,
					   topology_thread_cpumask(cpu));
			}
		}

		free_cpumask_var(thread_mask);
	}

	/* If RSS is requested for the PF *and* VFs then we can't write RSS
	 * table entries that are inaccessible to VFs
	 */
	if (efx_sriov_wanted(efx) && efx_vf_size(efx) > 1 &&
	    count > efx_vf_size(efx)) {
		netif_warn(efx, probe, efx->net_dev,
			   "Reducing number of RSS channels from %u to %u for "
			   "VF support. Increase vf-msix-limit to use more "
			   "channels on the PF.\n",
			   count, efx_vf_size(efx));
		count = efx_vf_size(efx);
	}

	return count;
}

static int
efx_init_rx_cpu_rmap(struct efx_nic *efx, struct msix_entry *xentries)
{
#ifdef CONFIG_RFS_ACCEL
	unsigned int i;
	int rc;

	efx->net_dev->rx_cpu_rmap = alloc_irq_cpu_rmap(efx->n_rx_channels);
	if (!efx->net_dev->rx_cpu_rmap)
		return -ENOMEM;
	for (i = 0; i < efx->n_rx_channels; i++) {
		rc = irq_cpu_rmap_add(efx->net_dev->rx_cpu_rmap,
				      xentries[i].vector);
		if (rc) {
			free_irq_cpu_rmap(efx->net_dev->rx_cpu_rmap);
			efx->net_dev->rx_cpu_rmap = NULL;
			return rc;
		}
	}
#endif
	return 0;
}

/* Probe the number and type of interrupts we are able to obtain, and
 * the resulting numbers of channels and RX queues.
 */
static int efx_probe_interrupts(struct efx_nic *efx)
{
	unsigned int max_channels =
		min(efx->type->phys_addr_channels, EFX_MAX_CHANNELS);
	unsigned int extra_channels = 0;
	unsigned int i, j;
	int rc;

	for (i = 0; i < EFX_MAX_EXTRA_CHANNELS; i++)
		if (efx->extra_channel_type[i])
			++extra_channels;

	if (efx->interrupt_mode == EFX_INT_MODE_MSIX) {
		struct msix_entry xentries[EFX_MAX_CHANNELS];
		unsigned int n_channels;

		n_channels = efx_wanted_parallelism(efx);
		if (separate_tx_channels)
			n_channels *= 2;
		n_channels += extra_channels;
		n_channels = min(n_channels, max_channels);

		for (i = 0; i < n_channels; i++)
			xentries[i].entry = i;
		rc = pci_enable_msix(efx->pci_dev, xentries, n_channels);
		if (rc > 0) {
			netif_err(efx, drv, efx->net_dev,
				  "WARNING: Insufficient MSI-X vectors"
				  " available (%d < %u).\n", rc, n_channels);
			netif_err(efx, drv, efx->net_dev,
				  "WARNING: Performance may be reduced.\n");
			EFX_BUG_ON_PARANOID(rc >= n_channels);
			n_channels = rc;
			rc = pci_enable_msix(efx->pci_dev, xentries,
					     n_channels);
		}

		if (rc == 0) {
			efx->n_channels = n_channels;
			if (n_channels > extra_channels)
				n_channels -= extra_channels;
			if (separate_tx_channels) {
				efx->n_tx_channels = max(n_channels / 2, 1U);
				efx->n_rx_channels = max(n_channels -
							 efx->n_tx_channels,
							 1U);
			} else {
				efx->n_tx_channels = n_channels;
				efx->n_rx_channels = n_channels;
			}
			rc = efx_init_rx_cpu_rmap(efx, xentries);
			if (rc) {
				pci_disable_msix(efx->pci_dev);
				return rc;
			}
			for (i = 0; i < efx->n_channels; i++)
				efx_get_channel(efx, i)->irq =
					xentries[i].vector;
		} else {
			/* Fall back to single channel MSI */
			efx->interrupt_mode = EFX_INT_MODE_MSI;
			netif_err(efx, drv, efx->net_dev,
				  "could not enable MSI-X\n");
		}
	}

	/* Try single interrupt MSI */
	if (efx->interrupt_mode == EFX_INT_MODE_MSI) {
		efx->n_channels = 1;
		efx->n_rx_channels = 1;
		efx->n_tx_channels = 1;
		rc = pci_enable_msi(efx->pci_dev);
		if (rc == 0) {
			efx_get_channel(efx, 0)->irq = efx->pci_dev->irq;
		} else {
			netif_err(efx, drv, efx->net_dev,
				  "could not enable MSI\n");
			efx->interrupt_mode = EFX_INT_MODE_LEGACY;
		}
	}

	/* Assume legacy interrupts */
	if (efx->interrupt_mode == EFX_INT_MODE_LEGACY) {
		efx->n_channels = 1 + (separate_tx_channels ? 1 : 0);
		efx->n_rx_channels = 1;
		efx->n_tx_channels = 1;
		efx->legacy_irq = efx->pci_dev->irq;
	}

	/* Assign extra channels if possible */
	j = efx->n_channels;
	for (i = 0; i < EFX_MAX_EXTRA_CHANNELS; i++) {
		if (!efx->extra_channel_type[i])
			continue;
		if (efx->interrupt_mode != EFX_INT_MODE_MSIX ||
		    efx->n_channels <= extra_channels) {
			efx->extra_channel_type[i]->handle_no_channel(efx);
		} else {
			--j;
			efx_get_channel(efx, j)->type =
				efx->extra_channel_type[i];
		}
	}

	/* RSS might be usable on VFs even if it is disabled on the PF */
	efx->rss_spread = ((efx->n_rx_channels > 1 || !efx_sriov_wanted(efx)) ?
			   efx->n_rx_channels : efx_vf_size(efx));

	return 0;
}

/* Enable interrupts, then probe and start the event queues */
static void efx_start_interrupts(struct efx_nic *efx, bool may_keep_eventq)
{
	struct efx_channel *channel;

	if (efx->legacy_irq)
		efx->legacy_irq_enabled = true;
	efx_nic_enable_interrupts(efx);

	efx_for_each_channel(channel, efx) {
		if (!channel->type->keep_eventq || !may_keep_eventq)
			efx_init_eventq(channel);
		efx_start_eventq(channel);
	}

	efx_mcdi_mode_event(efx);
}

static void efx_stop_interrupts(struct efx_nic *efx, bool may_keep_eventq)
{
	struct efx_channel *channel;

	efx_mcdi_mode_poll(efx);

	efx_nic_disable_interrupts(efx);
	if (efx->legacy_irq) {
		synchronize_irq(efx->legacy_irq);
		efx->legacy_irq_enabled = false;
	}

	efx_for_each_channel(channel, efx) {
		if (channel->irq)
			synchronize_irq(channel->irq);

		efx_stop_eventq(channel);
		if (!channel->type->keep_eventq || !may_keep_eventq)
			efx_fini_eventq(channel);
	}
}

static void efx_remove_interrupts(struct efx_nic *efx)
{
	struct efx_channel *channel;

	/* Remove MSI/MSI-X interrupts */
	efx_for_each_channel(channel, efx)
		channel->irq = 0;
	pci_disable_msi(efx->pci_dev);
	pci_disable_msix(efx->pci_dev);

	/* Remove legacy interrupt */
	efx->legacy_irq = 0;
}

static void efx_set_channels(struct efx_nic *efx)
{
	struct efx_channel *channel;
	struct efx_tx_queue *tx_queue;

	efx->tx_channel_offset =
		separate_tx_channels ? efx->n_channels - efx->n_tx_channels : 0;

	/* We need to adjust the TX queue numbers if we have separate
	 * RX-only and TX-only channels.
	 */
	efx_for_each_channel(channel, efx) {
		efx_for_each_channel_tx_queue(tx_queue, channel)
			tx_queue->queue -= (efx->tx_channel_offset *
					    EFX_TXQ_TYPES);
	}
}

static int efx_probe_nic(struct efx_nic *efx)
{
	size_t i;
	int rc;

	netif_dbg(efx, probe, efx->net_dev, "creating NIC\n");

	/* Carry out hardware-type specific initialisation */
	rc = efx->type->probe(efx);
	if (rc)
		return rc;

	/* Determine the number of channels and queues by trying to hook
	 * in MSI-X interrupts. */
	rc = efx_probe_interrupts(efx);
	if (rc)
		goto fail;

	efx->type->dimension_resources(efx);

	if (efx->n_channels > 1)
		get_random_bytes(&efx->rx_hash_key, sizeof(efx->rx_hash_key));
	for (i = 0; i < ARRAY_SIZE(efx->rx_indir_table); i++)
		efx->rx_indir_table[i] =
			ethtool_rxfh_indir_default(i, efx->rss_spread);

	efx_set_channels(efx);
	netif_set_real_num_tx_queues(efx->net_dev, efx->n_tx_channels);
	netif_set_real_num_rx_queues(efx->net_dev, efx->n_rx_channels);

	/* Initialise the interrupt moderation settings */
	efx_init_irq_moderation(efx, tx_irq_mod_usec, rx_irq_mod_usec, true,
				true);

	return 0;

fail:
	efx->type->remove(efx);
	return rc;
}

static void efx_remove_nic(struct efx_nic *efx)
{
	netif_dbg(efx, drv, efx->net_dev, "destroying NIC\n");

	efx_remove_interrupts(efx);
	efx->type->remove(efx);
}

/**************************************************************************
 *
 * NIC startup/shutdown
 *
 *************************************************************************/

static int efx_probe_all(struct efx_nic *efx)
{
	int rc;

	rc = efx_probe_nic(efx);
	if (rc) {
		netif_err(efx, probe, efx->net_dev, "failed to create NIC\n");
		goto fail1;
	}

	rc = efx_probe_port(efx);
	if (rc) {
		netif_err(efx, probe, efx->net_dev, "failed to create port\n");
		goto fail2;
	}

	BUILD_BUG_ON(EFX_DEFAULT_DMAQ_SIZE < EFX_RXQ_MIN_ENT);
	if (WARN_ON(EFX_DEFAULT_DMAQ_SIZE < EFX_TXQ_MIN_ENT(efx))) {
		rc = -EINVAL;
		goto fail3;
	}
	efx->rxq_entries = efx->txq_entries = EFX_DEFAULT_DMAQ_SIZE;

	rc = efx_probe_filters(efx);
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "failed to create filter tables\n");
		goto fail3;
	}

	rc = efx_probe_channels(efx);
	if (rc)
		goto fail4;

	return 0;

 fail4:
	efx_remove_filters(efx);
 fail3:
	efx_remove_port(efx);
 fail2:
	efx_remove_nic(efx);
 fail1:
	return rc;
}

/* Called after previous invocation(s) of efx_stop_all, restarts the port,
 * kernel transmit queues and NAPI processing, and ensures that the port is
 * scheduled to be reconfigured. This function is safe to call multiple
 * times when the NIC is in any state.
 */
static void efx_start_all(struct efx_nic *efx)
{
	EFX_ASSERT_RESET_SERIALISED(efx);

	/* Check that it is appropriate to restart the interface. All
	 * of these flags are safe to read under just the rtnl lock */
	if (efx->port_enabled)
		return;
	if ((efx->state != STATE_RUNNING) && (efx->state != STATE_INIT))
		return;
	if (!netif_running(efx->net_dev))
		return;

	efx_start_port(efx);
	efx_start_datapath(efx);

	/* Start the hardware monitor if there is one. Otherwise (we're link
	 * event driven), we have to poll the PHY because after an event queue
	 * flush, we could have a missed a link state change */
	if (efx->type->monitor != NULL) {
		queue_delayed_work(efx->workqueue, &efx->monitor_work,
				   efx_monitor_interval);
	} else {
		mutex_lock(&efx->mac_lock);
		if (efx->phy_op->poll(efx))
			efx_link_status_changed(efx);
		mutex_unlock(&efx->mac_lock);
	}

	efx->type->start_stats(efx);
}

/* Flush all delayed work. Should only be called when no more delayed work
 * will be scheduled. This doesn't flush pending online resets (efx_reset),
 * since we're holding the rtnl_lock at this point. */
static void efx_flush_all(struct efx_nic *efx)
{
	/* Make sure the hardware monitor and event self-test are stopped */
	cancel_delayed_work_sync(&efx->monitor_work);
	efx_selftest_async_cancel(efx);
	/* Stop scheduled port reconfigurations */
	cancel_work_sync(&efx->mac_work);
}

/* Quiesce hardware and software without bringing the link down.
 * Safe to call multiple times, when the nic and interface is in any
 * state. The caller is guaranteed to subsequently be in a position
 * to modify any hardware and software state they see fit without
 * taking locks. */
static void efx_stop_all(struct efx_nic *efx)
{
	EFX_ASSERT_RESET_SERIALISED(efx);

	/* port_enabled can be read safely under the rtnl lock */
	if (!efx->port_enabled)
		return;

	efx->type->stop_stats(efx);
	efx_stop_port(efx);

	/* Flush efx_mac_work(), refill_workqueue, monitor_work */
	efx_flush_all(efx);

	/* Stop the kernel transmit interface.  This is only valid if
	 * the device is stopped or detached; otherwise the watchdog
	 * may fire immediately.
	 */
	WARN_ON(netif_running(efx->net_dev) &&
		netif_device_present(efx->net_dev));
	netif_tx_disable(efx->net_dev);

	efx_stop_datapath(efx);
}

static void efx_remove_all(struct efx_nic *efx)
{
	efx_remove_channels(efx);
	efx_remove_filters(efx);
	efx_remove_port(efx);
	efx_remove_nic(efx);
}

/**************************************************************************
 *
 * Interrupt moderation
 *
 **************************************************************************/

static unsigned int irq_mod_ticks(unsigned int usecs, unsigned int quantum_ns)
{
	if (usecs == 0)
		return 0;
	if (usecs * 1000 < quantum_ns)
		return 1; /* never round down to 0 */
	return usecs * 1000 / quantum_ns;
}

/* Set interrupt moderation parameters */
int efx_init_irq_moderation(struct efx_nic *efx, unsigned int tx_usecs,
			    unsigned int rx_usecs, bool rx_adaptive,
			    bool rx_may_override_tx)
{
	struct efx_channel *channel;
	unsigned int irq_mod_max = DIV_ROUND_UP(efx->type->timer_period_max *
						efx->timer_quantum_ns,
						1000);
	unsigned int tx_ticks;
	unsigned int rx_ticks;

	EFX_ASSERT_RESET_SERIALISED(efx);

	if (tx_usecs > irq_mod_max || rx_usecs > irq_mod_max)
		return -EINVAL;

	tx_ticks = irq_mod_ticks(tx_usecs, efx->timer_quantum_ns);
	rx_ticks = irq_mod_ticks(rx_usecs, efx->timer_quantum_ns);

	if (tx_ticks != rx_ticks && efx->tx_channel_offset == 0 &&
	    !rx_may_override_tx) {
		netif_err(efx, drv, efx->net_dev, "Channels are shared. "
			  "RX and TX IRQ moderation must be equal\n");
		return -EINVAL;
	}

	efx->irq_rx_adaptive = rx_adaptive;
	efx->irq_rx_moderation = rx_ticks;
	efx_for_each_channel(channel, efx) {
		if (efx_channel_has_rx_queue(channel))
			channel->irq_moderation = rx_ticks;
		else if (efx_channel_has_tx_queues(channel))
			channel->irq_moderation = tx_ticks;
	}

	return 0;
}

void efx_get_irq_moderation(struct efx_nic *efx, unsigned int *tx_usecs,
			    unsigned int *rx_usecs, bool *rx_adaptive)
{
	/* We must round up when converting ticks to microseconds
	 * because we round down when converting the other way.
	 */

	*rx_adaptive = efx->irq_rx_adaptive;
	*rx_usecs = DIV_ROUND_UP(efx->irq_rx_moderation *
				 efx->timer_quantum_ns,
				 1000);

	/* If channels are shared between RX and TX, so is IRQ
	 * moderation.  Otherwise, IRQ moderation is the same for all
	 * TX channels and is not adaptive.
	 */
	if (efx->tx_channel_offset == 0)
		*tx_usecs = *rx_usecs;
	else
		*tx_usecs = DIV_ROUND_UP(
			efx->channel[efx->tx_channel_offset]->irq_moderation *
			efx->timer_quantum_ns,
			1000);
}

/**************************************************************************
 *
 * Hardware monitor
 *
 **************************************************************************/

/* Run periodically off the general workqueue */
static void efx_monitor(struct work_struct *data)
{
	struct efx_nic *efx = container_of(data, struct efx_nic,
					   monitor_work.work);

	netif_vdbg(efx, timer, efx->net_dev,
		   "hardware monitor executing on CPU %d\n",
		   raw_smp_processor_id());
	BUG_ON(efx->type->monitor == NULL);

	/* If the mac_lock is already held then it is likely a port
	 * reconfiguration is already in place, which will likely do
	 * most of the work of monitor() anyway. */
	if (mutex_trylock(&efx->mac_lock)) {
		if (efx->port_enabled)
			efx->type->monitor(efx);
		mutex_unlock(&efx->mac_lock);
	}

	queue_delayed_work(efx->workqueue, &efx->monitor_work,
			   efx_monitor_interval);
}

/**************************************************************************
 *
 * ioctls
 *
 *************************************************************************/

/* Net device ioctl
 * Context: process, rtnl_lock() held.
 */
static int efx_ioctl(struct net_device *net_dev, struct ifreq *ifr, int cmd)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct mii_ioctl_data *data = if_mii(ifr);

	EFX_ASSERT_RESET_SERIALISED(efx);

	/* Convert phy_id from older PRTAD/DEVAD format */
	if ((cmd == SIOCGMIIREG || cmd == SIOCSMIIREG) &&
	    (data->phy_id & 0xfc00) == 0x0400)
		data->phy_id ^= MDIO_PHY_ID_C45 | 0x0400;

	return mdio_mii_ioctl(&efx->mdio, data, cmd);
}

/**************************************************************************
 *
 * NAPI interface
 *
 **************************************************************************/

static void efx_init_napi_channel(struct efx_channel *channel)
{
	struct efx_nic *efx = channel->efx;

	channel->napi_dev = efx->net_dev;
	netif_napi_add(channel->napi_dev, &channel->napi_str,
		       efx_poll, napi_weight);
}

static void efx_init_napi(struct efx_nic *efx)
{
	struct efx_channel *channel;

	efx_for_each_channel(channel, efx)
		efx_init_napi_channel(channel);
}

static void efx_fini_napi_channel(struct efx_channel *channel)
{
	if (channel->napi_dev)
		netif_napi_del(&channel->napi_str);
	channel->napi_dev = NULL;
}

static void efx_fini_napi(struct efx_nic *efx)
{
	struct efx_channel *channel;

	efx_for_each_channel(channel, efx)
		efx_fini_napi_channel(channel);
}

/**************************************************************************
 *
 * Kernel netpoll interface
 *
 *************************************************************************/

#ifdef CONFIG_NET_POLL_CONTROLLER

/* Although in the common case interrupts will be disabled, this is not
 * guaranteed. However, all our work happens inside the NAPI callback,
 * so no locking is required.
 */
static void efx_netpoll(struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct efx_channel *channel;

	efx_for_each_channel(channel, efx)
		efx_schedule_channel(channel);
}

#endif

/**************************************************************************
 *
 * Kernel net device interface
 *
 *************************************************************************/

/* Context: process, rtnl_lock() held. */
static int efx_net_open(struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	EFX_ASSERT_RESET_SERIALISED(efx);

	netif_dbg(efx, ifup, efx->net_dev, "opening device on CPU %d\n",
		  raw_smp_processor_id());

	if (efx->state == STATE_DISABLED)
		return -EIO;
	if (efx->phy_mode & PHY_MODE_SPECIAL)
		return -EBUSY;
	if (efx_mcdi_poll_reboot(efx) && efx_reset(efx, RESET_TYPE_ALL))
		return -EIO;

	/* Notify the kernel of the link state polled during driver load,
	 * before the monitor starts running */
	efx_link_status_changed(efx);

	efx_start_all(efx);
	efx_selftest_async_start(efx);
	return 0;
}

/* Context: process, rtnl_lock() held.
 * Note that the kernel will ignore our return code; this method
 * should really be a void.
 */
static int efx_net_stop(struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);

	netif_dbg(efx, ifdown, efx->net_dev, "closing on CPU %d\n",
		  raw_smp_processor_id());

	if (efx->state != STATE_DISABLED) {
		/* Stop the device and flush all the channels */
		efx_stop_all(efx);
	}

	return 0;
}

/* Context: process, dev_base_lock or RTNL held, non-blocking. */
static struct rtnl_link_stats64 *efx_net_stats(struct net_device *net_dev,
					       struct rtnl_link_stats64 *stats)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct efx_mac_stats *mac_stats = &efx->mac_stats;

	spin_lock_bh(&efx->stats_lock);

	efx->type->update_stats(efx);

	stats->rx_packets = mac_stats->rx_packets;
	stats->tx_packets = mac_stats->tx_packets;
	stats->rx_bytes = mac_stats->rx_bytes;
	stats->tx_bytes = mac_stats->tx_bytes;
	stats->rx_dropped = efx->n_rx_nodesc_drop_cnt;
	stats->multicast = mac_stats->rx_multicast;
	stats->collisions = mac_stats->tx_collision;
	stats->rx_length_errors = (mac_stats->rx_gtjumbo +
				   mac_stats->rx_length_error);
	stats->rx_crc_errors = mac_stats->rx_bad;
	stats->rx_frame_errors = mac_stats->rx_align_error;
	stats->rx_fifo_errors = mac_stats->rx_overflow;
	stats->rx_missed_errors = mac_stats->rx_missed;
	stats->tx_window_errors = mac_stats->tx_late_collision;

	stats->rx_errors = (stats->rx_length_errors +
			    stats->rx_crc_errors +
			    stats->rx_frame_errors +
			    mac_stats->rx_symbol_error);
	stats->tx_errors = (stats->tx_window_errors +
			    mac_stats->tx_bad);

	spin_unlock_bh(&efx->stats_lock);

	return stats;
}

/* Context: netif_tx_lock held, BHs disabled. */
static void efx_watchdog(struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);

	netif_err(efx, tx_err, efx->net_dev,
		  "TX stuck with port_enabled=%d: resetting channels\n",
		  efx->port_enabled);

	efx_schedule_reset(efx, RESET_TYPE_TX_WATCHDOG);
}


/* Context: process, rtnl_lock() held. */
static int efx_change_mtu(struct net_device *net_dev, int new_mtu)
{
	struct efx_nic *efx = netdev_priv(net_dev);

	EFX_ASSERT_RESET_SERIALISED(efx);

	if (new_mtu > EFX_MAX_MTU)
		return -EINVAL;

	netif_dbg(efx, drv, efx->net_dev, "changing MTU to %d\n", new_mtu);

	efx_device_detach_sync(efx);
	efx_stop_all(efx);

	mutex_lock(&efx->mac_lock);
	/* Reconfigure the MAC before enabling the dma queues so that
	 * the RX buffers don't overflow */
	net_dev->mtu = new_mtu;
	efx->type->reconfigure_mac(efx);
	mutex_unlock(&efx->mac_lock);

	efx_start_all(efx);
	netif_device_attach(efx->net_dev);
	return 0;
}

static int efx_set_mac_address(struct net_device *net_dev, void *data)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct sockaddr *addr = data;
	char *new_addr = addr->sa_data;

	EFX_ASSERT_RESET_SERIALISED(efx);

	if (!is_valid_ether_addr(new_addr)) {
		netif_err(efx, drv, efx->net_dev,
			  "invalid ethernet MAC address requested: %pM\n",
			  new_addr);
		return -EADDRNOTAVAIL;
	}

	memcpy(net_dev->dev_addr, new_addr, net_dev->addr_len);
	efx_sriov_mac_address_changed(efx);

	/* Reconfigure the MAC */
	mutex_lock(&efx->mac_lock);
	efx->type->reconfigure_mac(efx);
	mutex_unlock(&efx->mac_lock);

	return 0;
}

/* Context: netif_addr_lock held, BHs disabled. */
static void efx_set_rx_mode(struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct netdev_hw_addr *ha;
	union efx_multicast_hash *mc_hash = &efx->multicast_hash;
	u32 crc;
	int bit;

	efx->promiscuous = !!(net_dev->flags & IFF_PROMISC);

	/* Build multicast hash table */
	if (efx->promiscuous || (net_dev->flags & IFF_ALLMULTI)) {
		memset(mc_hash, 0xff, sizeof(*mc_hash));
	} else {
		memset(mc_hash, 0x00, sizeof(*mc_hash));
		netdev_for_each_mc_addr(ha, net_dev) {
			crc = ether_crc_le(ETH_ALEN, ha->addr);
			bit = crc & (EFX_MCAST_HASH_ENTRIES - 1);
			set_bit_le(bit, mc_hash->byte);
		}

		/* Broadcast packets go through the multicast hash filter.
		 * ether_crc_le() of the broadcast address is 0xbe2612ff
		 * so we always add bit 0xff to the mask.
		 */
		set_bit_le(0xff, mc_hash->byte);
	}

	if (efx->port_enabled)
		queue_work(efx->workqueue, &efx->mac_work);
	/* Otherwise efx_start_port() will do this */
}

static int efx_set_features(struct net_device *net_dev, netdev_features_t data)
{
	struct efx_nic *efx = netdev_priv(net_dev);

	/* If disabling RX n-tuple filtering, clear existing filters */
	if (net_dev->features & ~data & NETIF_F_NTUPLE)
		efx_filter_clear_rx(efx, EFX_FILTER_PRI_MANUAL);

	return 0;
}

static const struct net_device_ops efx_netdev_ops = {
	.ndo_open		= efx_net_open,
	.ndo_stop		= efx_net_stop,
	.ndo_get_stats64	= efx_net_stats,
	.ndo_tx_timeout		= efx_watchdog,
	.ndo_start_xmit		= efx_hard_start_xmit,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_do_ioctl		= efx_ioctl,
	.ndo_change_mtu		= efx_change_mtu,
	.ndo_set_mac_address	= efx_set_mac_address,
	.ndo_set_rx_mode	= efx_set_rx_mode,
	.ndo_set_features	= efx_set_features,
#ifdef CONFIG_SFC_SRIOV
	.ndo_set_vf_mac		= efx_sriov_set_vf_mac,
	.ndo_set_vf_vlan	= efx_sriov_set_vf_vlan,
	.ndo_set_vf_spoofchk	= efx_sriov_set_vf_spoofchk,
	.ndo_get_vf_config	= efx_sriov_get_vf_config,
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller = efx_netpoll,
#endif
	.ndo_setup_tc		= efx_setup_tc,
#ifdef CONFIG_RFS_ACCEL
	.ndo_rx_flow_steer	= efx_filter_rfs,
#endif
};

static void efx_update_name(struct efx_nic *efx)
{
	strcpy(efx->name, efx->net_dev->name);
	efx_mtd_rename(efx);
	efx_set_channel_names(efx);
}

static int efx_netdev_event(struct notifier_block *this,
			    unsigned long event, void *ptr)
{
	struct net_device *net_dev = ptr;

	if (net_dev->netdev_ops == &efx_netdev_ops &&
	    event == NETDEV_CHANGENAME)
		efx_update_name(netdev_priv(net_dev));

	return NOTIFY_DONE;
}

static struct notifier_block efx_netdev_notifier = {
	.notifier_call = efx_netdev_event,
};

static ssize_t
show_phy_type(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct efx_nic *efx = pci_get_drvdata(to_pci_dev(dev));
	return sprintf(buf, "%d\n", efx->phy_type);
}
static DEVICE_ATTR(phy_type, 0644, show_phy_type, NULL);

static int efx_register_netdev(struct efx_nic *efx)
{
	struct net_device *net_dev = efx->net_dev;
	struct efx_channel *channel;
	int rc;

	net_dev->watchdog_timeo = 5 * HZ;
	net_dev->irq = efx->pci_dev->irq;
	net_dev->netdev_ops = &efx_netdev_ops;
	SET_ETHTOOL_OPS(net_dev, &efx_ethtool_ops);
	net_dev->gso_max_segs = EFX_TSO_MAX_SEGS;

	rtnl_lock();

	rc = dev_alloc_name(net_dev, net_dev->name);
	if (rc < 0)
		goto fail_locked;
	efx_update_name(efx);

	rc = register_netdevice(net_dev);
	if (rc)
		goto fail_locked;

	efx_for_each_channel(channel, efx) {
		struct efx_tx_queue *tx_queue;
		efx_for_each_channel_tx_queue(tx_queue, channel)
			efx_init_tx_queue_core_txq(tx_queue);
	}

	/* Always start with carrier off; PHY events will detect the link */
	netif_carrier_off(net_dev);

	rtnl_unlock();

	rc = device_create_file(&efx->pci_dev->dev, &dev_attr_phy_type);
	if (rc) {
		netif_err(efx, drv, efx->net_dev,
			  "failed to init net dev attributes\n");
		goto fail_registered;
	}

	return 0;

fail_locked:
	rtnl_unlock();
	netif_err(efx, drv, efx->net_dev, "could not register net dev\n");
	return rc;

fail_registered:
	unregister_netdev(net_dev);
	return rc;
}

static void efx_unregister_netdev(struct efx_nic *efx)
{
	struct efx_channel *channel;
	struct efx_tx_queue *tx_queue;

	if (!efx->net_dev)
		return;

	BUG_ON(netdev_priv(efx->net_dev) != efx);

	/* Free up any skbs still remaining. This has to happen before
	 * we try to unregister the netdev as running their destructors
	 * may be needed to get the device ref. count to 0. */
	efx_for_each_channel(channel, efx) {
		efx_for_each_channel_tx_queue(tx_queue, channel)
			efx_release_tx_buffers(tx_queue);
	}

	strlcpy(efx->name, pci_name(efx->pci_dev), sizeof(efx->name));
	device_remove_file(&efx->pci_dev->dev, &dev_attr_phy_type);
	unregister_netdev(efx->net_dev);
}

/**************************************************************************
 *
 * Device reset and suspend
 *
 **************************************************************************/

/* Tears down the entire software state and most of the hardware state
 * before reset.  */
void efx_reset_down(struct efx_nic *efx, enum reset_type method)
{
	EFX_ASSERT_RESET_SERIALISED(efx);

	efx_stop_all(efx);
	mutex_lock(&efx->mac_lock);

	efx_stop_interrupts(efx, false);
	if (efx->port_initialized && method != RESET_TYPE_INVISIBLE)
		efx->phy_op->fini(efx);
	efx->type->fini(efx);
}

/* This function will always ensure that the locks acquired in
 * efx_reset_down() are released. A failure return code indicates
 * that we were unable to reinitialise the hardware, and the
 * driver should be disabled. If ok is false, then the rx and tx
 * engines are not restarted, pending a RESET_DISABLE. */
int efx_reset_up(struct efx_nic *efx, enum reset_type method, bool ok)
{
	int rc;

	EFX_ASSERT_RESET_SERIALISED(efx);

	rc = efx->type->init(efx);
	if (rc) {
		netif_err(efx, drv, efx->net_dev, "failed to initialise NIC\n");
		goto fail;
	}

	if (!ok)
		goto fail;

	if (efx->port_initialized && method != RESET_TYPE_INVISIBLE) {
		rc = efx->phy_op->init(efx);
		if (rc)
			goto fail;
		if (efx->phy_op->reconfigure(efx))
			netif_err(efx, drv, efx->net_dev,
				  "could not restore PHY settings\n");
	}

	efx->type->reconfigure_mac(efx);

	efx_start_interrupts(efx, false);
	efx_restore_filters(efx);
	efx_sriov_reset(efx);

	mutex_unlock(&efx->mac_lock);

	efx_start_all(efx);

	return 0;

fail:
	efx->port_initialized = false;

	mutex_unlock(&efx->mac_lock);

	return rc;
}

/* Reset the NIC using the specified method.  Note that the reset may
 * fail, in which case the card will be left in an unusable state.
 *
 * Caller must hold the rtnl_lock.
 */
int efx_reset(struct efx_nic *efx, enum reset_type method)
{
	int rc, rc2;
	bool disabled;

	netif_info(efx, drv, efx->net_dev, "resetting (%s)\n",
		   RESET_TYPE(method));

	efx_device_detach_sync(efx);
	efx_reset_down(efx, method);

	rc = efx->type->reset(efx, method);
	if (rc) {
		netif_err(efx, drv, efx->net_dev, "failed to reset hardware\n");
		goto out;
	}

	/* Clear flags for the scopes we covered.  We assume the NIC and
	 * driver are now quiescent so that there is no race here.
	 */
	efx->reset_pending &= -(1 << (method + 1));

	/* Reinitialise bus-mastering, which may have been turned off before
	 * the reset was scheduled. This is still appropriate, even in the
	 * RESET_TYPE_DISABLE since this driver generally assumes the hardware
	 * can respond to requests. */
	pci_set_master(efx->pci_dev);

out:
	/* Leave device stopped if necessary */
	disabled = rc || method == RESET_TYPE_DISABLE;
	rc2 = efx_reset_up(efx, method, !disabled);
	if (rc2) {
		disabled = true;
		if (!rc)
			rc = rc2;
	}

	if (disabled) {
		dev_close(efx->net_dev);
		netif_err(efx, drv, efx->net_dev, "has been disabled\n");
		efx->state = STATE_DISABLED;
	} else {
		netif_dbg(efx, drv, efx->net_dev, "reset complete\n");
		netif_device_attach(efx->net_dev);
	}
	return rc;
}

/* The worker thread exists so that code that cannot sleep can
 * schedule a reset for later.
 */
static void efx_reset_work(struct work_struct *data)
{
	struct efx_nic *efx = container_of(data, struct efx_nic, reset_work);
	unsigned long pending = READ_ONCE(efx->reset_pending);

	if (!pending)
		return;

	/* If we're not RUNNING then don't reset. Leave the reset_pending
	 * flags set so that efx_pci_probe_main will be retried */
	if (efx->state != STATE_RUNNING) {
		netif_info(efx, drv, efx->net_dev,
			   "scheduled reset quenched. NIC not RUNNING\n");
		return;
	}

	rtnl_lock();
	(void)efx_reset(efx, fls(pending) - 1);
	rtnl_unlock();
}

void efx_schedule_reset(struct efx_nic *efx, enum reset_type type)
{
	enum reset_type method;

	switch (type) {
	case RESET_TYPE_INVISIBLE:
	case RESET_TYPE_ALL:
	case RESET_TYPE_WORLD:
	case RESET_TYPE_DISABLE:
		method = type;
		netif_dbg(efx, drv, efx->net_dev, "scheduling %s reset\n",
			  RESET_TYPE(method));
		break;
	default:
		method = efx->type->map_reset_reason(type);
		netif_dbg(efx, drv, efx->net_dev,
			  "scheduling %s reset for %s\n",
			  RESET_TYPE(method), RESET_TYPE(type));
		break;
	}

	set_bit(method, &efx->reset_pending);

	/* efx_process_channel() will no longer read events once a
	 * reset is scheduled. So switch back to poll'd MCDI completions. */
	efx_mcdi_mode_poll(efx);

	queue_work(reset_workqueue, &efx->reset_work);
}

/**************************************************************************
 *
 * List of NICs we support
 *
 **************************************************************************/

/* PCI device ID table */
static DEFINE_PCI_DEVICE_TABLE(efx_pci_table) = {
	{PCI_DEVICE(PCI_VENDOR_ID_SOLARFLARE,
		    PCI_DEVICE_ID_SOLARFLARE_SFC4000A_0),
	 .driver_data = (unsigned long) &falcon_a1_nic_type},
	{PCI_DEVICE(PCI_VENDOR_ID_SOLARFLARE,
		    PCI_DEVICE_ID_SOLARFLARE_SFC4000B),
	 .driver_data = (unsigned long) &falcon_b0_nic_type},
	{PCI_DEVICE(PCI_VENDOR_ID_SOLARFLARE, 0x0803),	/* SFC9020 */
	 .driver_data = (unsigned long) &siena_a0_nic_type},
	{PCI_DEVICE(PCI_VENDOR_ID_SOLARFLARE, 0x0813),	/* SFL9021 */
	 .driver_data = (unsigned long) &siena_a0_nic_type},
	{0}			/* end of list */
};

/**************************************************************************
 *
 * Dummy PHY/MAC operations
 *
 * Can be used for some unimplemented operations
 * Needed so all function pointers are valid and do not have to be tested
 * before use
 *
 **************************************************************************/
int efx_port_dummy_op_int(struct efx_nic *efx)
{
	return 0;
}
void efx_port_dummy_op_void(struct efx_nic *efx) {}

static bool efx_port_dummy_op_poll(struct efx_nic *efx)
{
	return false;
}

static const struct efx_phy_operations efx_dummy_phy_operations = {
	.init		 = efx_port_dummy_op_int,
	.reconfigure	 = efx_port_dummy_op_int,
	.poll		 = efx_port_dummy_op_poll,
	.fini		 = efx_port_dummy_op_void,
};

/**************************************************************************
 *
 * Data housekeeping
 *
 **************************************************************************/

/* This zeroes out and then fills in the invariants in a struct
 * efx_nic (including all sub-structures).
 */
static int efx_init_struct(struct efx_nic *efx, const struct efx_nic_type *type,
			   struct pci_dev *pci_dev, struct net_device *net_dev)
{
	int i;

	/* Initialise common structures */
	memset(efx, 0, sizeof(*efx));
	spin_lock_init(&efx->biu_lock);
#ifdef CONFIG_SFC_MTD
	INIT_LIST_HEAD(&efx->mtd_list);
#endif
	INIT_WORK(&efx->reset_work, efx_reset_work);
	INIT_DELAYED_WORK(&efx->monitor_work, efx_monitor);
	INIT_DELAYED_WORK(&efx->selftest_work, efx_selftest_async_work);
	efx->pci_dev = pci_dev;
	efx->msg_enable = debug;
	efx->state = STATE_INIT;
	strlcpy(efx->name, pci_name(pci_dev), sizeof(efx->name));

	efx->net_dev = net_dev;
	spin_lock_init(&efx->stats_lock);
	mutex_init(&efx->mac_lock);
	efx->phy_op = &efx_dummy_phy_operations;
	efx->mdio.dev = net_dev;
	INIT_WORK(&efx->mac_work, efx_mac_work);
	init_waitqueue_head(&efx->flush_wq);

	for (i = 0; i < EFX_MAX_CHANNELS; i++) {
		efx->channel[i] = efx_alloc_channel(efx, i, NULL);
		if (!efx->channel[i])
			goto fail;
	}

	efx->type = type;

	EFX_BUG_ON_PARANOID(efx->type->phys_addr_channels > EFX_MAX_CHANNELS);

	/* Higher numbered interrupt modes are less capable! */
	efx->interrupt_mode = max(efx->type->max_interrupt_mode,
				  interrupt_mode);

	/* Would be good to use the net_dev name, but we're too early */
	snprintf(efx->workqueue_name, sizeof(efx->workqueue_name), "sfc%s",
		 pci_name(pci_dev));
	efx->workqueue = create_singlethread_workqueue(efx->workqueue_name);
	if (!efx->workqueue)
		goto fail;

	return 0;

fail:
	efx_fini_struct(efx);
	return -ENOMEM;
}

static void efx_fini_struct(struct efx_nic *efx)
{
	int i;

	for (i = 0; i < EFX_MAX_CHANNELS; i++)
		kfree(efx->channel[i]);

	if (efx->workqueue) {
		destroy_workqueue(efx->workqueue);
		efx->workqueue = NULL;
	}
}

/**************************************************************************
 *
 * PCI interface
 *
 **************************************************************************/

/* Main body of final NIC shutdown code
 * This is called only at module unload (or hotplug removal).
 */
static void efx_pci_remove_main(struct efx_nic *efx)
{
#ifdef CONFIG_RFS_ACCEL
	free_irq_cpu_rmap(efx->net_dev->rx_cpu_rmap);
	efx->net_dev->rx_cpu_rmap = NULL;
#endif
	efx_stop_interrupts(efx, false);
	efx_nic_fini_interrupt(efx);
	efx_fini_port(efx);
	efx->type->fini(efx);
	efx_fini_napi(efx);
	efx_remove_all(efx);
}

/* Final NIC shutdown
 * This is called only at module unload (or hotplug removal).
 */
static void efx_pci_remove(struct pci_dev *pci_dev)
{
	struct efx_nic *efx;

	efx = pci_get_drvdata(pci_dev);
	if (!efx)
		return;

	/* Mark the NIC as fini, then stop the interface */
	rtnl_lock();
	efx->state = STATE_FINI;
	dev_close(efx->net_dev);

	/* Allow any queued efx_resets() to complete */
	rtnl_unlock();

	efx_stop_interrupts(efx, false);
	efx_sriov_fini(efx);
	efx_unregister_netdev(efx);

	efx_mtd_remove(efx);

	/* Wait for any scheduled resets to complete. No more will be
	 * scheduled from this point because efx_stop_all() has been
	 * called, we are no longer registered with driverlink, and
	 * the net_device's have been removed. */
	cancel_work_sync(&efx->reset_work);

	efx_pci_remove_main(efx);

	efx_fini_io(efx);
	netif_dbg(efx, drv, efx->net_dev, "shutdown successful\n");

	pci_set_drvdata(pci_dev, NULL);
	efx_fini_struct(efx);
	free_netdev(efx->net_dev);
};

/* NIC VPD information
 * Called during probe to display the part number of the
 * installed NIC.  VPD is potentially very large but this should
 * always appear within the first 512 bytes.
 */
#define SFC_VPD_LEN 512
static void efx_print_product_vpd(struct efx_nic *efx)
{
	struct pci_dev *dev = efx->pci_dev;
	char vpd_data[SFC_VPD_LEN];
	ssize_t vpd_size;
	int i, j;

	/* Get the vpd data from the device */
	vpd_size = pci_read_vpd(dev, 0, sizeof(vpd_data), vpd_data);
	if (vpd_size <= 0) {
		netif_err(efx, drv, efx->net_dev, "Unable to read VPD\n");
		return;
	}

	/* Get the Read only section */
	i = pci_vpd_find_tag(vpd_data, 0, vpd_size, PCI_VPD_LRDT_RO_DATA);
	if (i < 0) {
		netif_err(efx, drv, efx->net_dev, "VPD Read-only not found\n");
		return;
	}

	j = pci_vpd_lrdt_size(&vpd_data[i]);
	i += PCI_VPD_LRDT_TAG_SIZE;
	if (i + j > vpd_size)
		j = vpd_size - i;

	/* Get the Part number */
	i = pci_vpd_find_info_keyword(vpd_data, i, j, "PN");
	if (i < 0) {
		netif_err(efx, drv, efx->net_dev, "Part number not found\n");
		return;
	}

	j = pci_vpd_info_field_size(&vpd_data[i]);
	i += PCI_VPD_INFO_FLD_HDR_SIZE;
	if (i + j > vpd_size) {
		netif_err(efx, drv, efx->net_dev, "Incomplete part number\n");
		return;
	}

	netif_info(efx, drv, efx->net_dev,
		   "Part Number : %.*s\n", j, &vpd_data[i]);
}


/* Main body of NIC initialisation
 * This is called at module load (or hotplug insertion, theoretically).
 */
static int efx_pci_probe_main(struct efx_nic *efx)
{
	int rc;

	/* Do start-of-day initialisation */
	rc = efx_probe_all(efx);
	if (rc)
		goto fail1;

	efx_init_napi(efx);

	rc = efx->type->init(efx);
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "failed to initialise NIC\n");
		goto fail3;
	}

	rc = efx_init_port(efx);
	if (rc) {
		netif_err(efx, probe, efx->net_dev,
			  "failed to initialise port\n");
		goto fail4;
	}

	rc = efx_nic_init_interrupt(efx);
	if (rc)
		goto fail5;
	efx_start_interrupts(efx, false);

	return 0;

 fail5:
	efx_fini_port(efx);
 fail4:
	efx->type->fini(efx);
 fail3:
	efx_fini_napi(efx);
	efx_remove_all(efx);
 fail1:
	return rc;
}

/* NIC initialisation
 *
 * This is called at module load (or hotplug insertion,
 * theoretically).  It sets up PCI mappings, resets the NIC,
 * sets up and registers the network devices with the kernel and hooks
 * the interrupt service routine.  It does not prepare the device for
 * transmission; this is left to the first time one of the network
 * interfaces is brought up (i.e. efx_net_open).
 */
static int __devinit efx_pci_probe(struct pci_dev *pci_dev,
				   const struct pci_device_id *entry)
{
	const struct efx_nic_type *type = (const struct efx_nic_type *) entry->driver_data;
	struct net_device *net_dev;
	struct efx_nic *efx;
	int rc;

	/* Allocate and initialise a struct net_device and struct efx_nic */
	net_dev = alloc_etherdev_mqs(sizeof(*efx), EFX_MAX_CORE_TX_QUEUES,
				     EFX_MAX_RX_QUEUES);
	if (!net_dev)
		return -ENOMEM;
	net_dev->features |= (type->offload_features | NETIF_F_SG |
			      NETIF_F_HIGHDMA | NETIF_F_TSO |
			      NETIF_F_RXCSUM);
	if (type->offload_features & NETIF_F_V6_CSUM)
		net_dev->features |= NETIF_F_TSO6;
	/* Mask for features that also apply to VLAN devices */
	net_dev->vlan_features |= (NETIF_F_ALL_CSUM | NETIF_F_SG |
				   NETIF_F_HIGHDMA | NETIF_F_ALL_TSO |
				   NETIF_F_RXCSUM);
	/* All offloads can be toggled */
	net_dev->hw_features = net_dev->features & ~NETIF_F_HIGHDMA;
	efx = netdev_priv(net_dev);
	pci_set_drvdata(pci_dev, efx);
	SET_NETDEV_DEV(net_dev, &pci_dev->dev);
	rc = efx_init_struct(efx, type, pci_dev, net_dev);
	if (rc)
		goto fail1;

	netif_info(efx, probe, efx->net_dev,
		   "Solarflare NIC detected\n");

	efx_print_product_vpd(efx);

	/* Set up basic I/O (BAR mappings etc) */
	rc = efx_init_io(efx);
	if (rc)
		goto fail2;

	rc = efx_pci_probe_main(efx);

	/* Serialise against efx_reset(). No more resets will be
	 * scheduled since efx_stop_all() has been called, and we have
	 * not and never have been registered.
	 */
	cancel_work_sync(&efx->reset_work);

	if (rc)
		goto fail3;

	/* If there was a scheduled reset during probe, the NIC is
	 * probably hosed anyway.
	 */
	if (efx->reset_pending) {
		rc = -EIO;
		goto fail4;
	}

	/* Switch to the running state before we expose the device to the OS,
	 * so that dev_open()|efx_start_all() will actually start the device */
	efx->state = STATE_RUNNING;

	rc = efx_register_netdev(efx);
	if (rc)
		goto fail4;

	rc = efx_sriov_init(efx);
	if (rc)
		netif_err(efx, probe, efx->net_dev,
			  "SR-IOV can't be enabled rc %d\n", rc);

	netif_dbg(efx, probe, efx->net_dev, "initialisation successful\n");

	/* Try to create MTDs, but allow this to fail */
	rtnl_lock();
	rc = efx_mtd_probe(efx);
	rtnl_unlock();
	if (rc)
		netif_warn(efx, probe, efx->net_dev,
			   "failed to create MTDs (%d)\n", rc);

	return 0;

 fail4:
	efx_pci_remove_main(efx);
 fail3:
	efx_fini_io(efx);
 fail2:
	efx_fini_struct(efx);
 fail1:
	WARN_ON(rc > 0);
	netif_dbg(efx, drv, efx->net_dev, "initialisation failed. rc=%d\n", rc);
	free_netdev(net_dev);
	return rc;
}

static int efx_pm_freeze(struct device *dev)
{
	struct efx_nic *efx = pci_get_drvdata(to_pci_dev(dev));

	efx->state = STATE_FINI;

	efx_device_detach_sync(efx);

	efx_stop_all(efx);
	efx_stop_interrupts(efx, false);

	return 0;
}

static int efx_pm_thaw(struct device *dev)
{
	struct efx_nic *efx = pci_get_drvdata(to_pci_dev(dev));

	efx->state = STATE_INIT;

	efx_start_interrupts(efx, false);

	mutex_lock(&efx->mac_lock);
	efx->phy_op->reconfigure(efx);
	mutex_unlock(&efx->mac_lock);

	efx_start_all(efx);

	netif_device_attach(efx->net_dev);

	efx->state = STATE_RUNNING;

	efx->type->resume_wol(efx);

	/* Reschedule any quenched resets scheduled during efx_pm_freeze() */
	queue_work(reset_workqueue, &efx->reset_work);

	return 0;
}

static int efx_pm_poweroff(struct device *dev)
{
	struct pci_dev *pci_dev = to_pci_dev(dev);
	struct efx_nic *efx = pci_get_drvdata(pci_dev);

	efx->type->fini(efx);

	efx->reset_pending = 0;

	pci_save_state(pci_dev);
	return pci_set_power_state(pci_dev, PCI_D3hot);
}

/* Used for both resume and restore */
static int efx_pm_resume(struct device *dev)
{
	struct pci_dev *pci_dev = to_pci_dev(dev);
	struct efx_nic *efx = pci_get_drvdata(pci_dev);
	int rc;

	rc = pci_set_power_state(pci_dev, PCI_D0);
	if (rc)
		return rc;
	pci_restore_state(pci_dev);
	rc = pci_enable_device(pci_dev);
	if (rc)
		return rc;
	pci_set_master(efx->pci_dev);
	rc = efx->type->reset(efx, RESET_TYPE_ALL);
	if (rc)
		return rc;
	rc = efx->type->init(efx);
	if (rc)
		return rc;
	efx_pm_thaw(dev);
	return 0;
}

static int efx_pm_suspend(struct device *dev)
{
	int rc;

	efx_pm_freeze(dev);
	rc = efx_pm_poweroff(dev);
	if (rc)
		efx_pm_resume(dev);
	return rc;
}

static const struct dev_pm_ops efx_pm_ops = {
	.suspend	= efx_pm_suspend,
	.resume		= efx_pm_resume,
	.freeze		= efx_pm_freeze,
	.thaw		= efx_pm_thaw,
	.poweroff	= efx_pm_poweroff,
	.restore	= efx_pm_resume,
};

static struct pci_driver efx_pci_driver = {
	.name		= KBUILD_MODNAME,
	.id_table	= efx_pci_table,
	.probe		= efx_pci_probe,
	.remove		= efx_pci_remove,
	.driver.pm	= &efx_pm_ops,
};

/**************************************************************************
 *
 * Kernel module interface
 *
 *************************************************************************/

module_param(interrupt_mode, uint, 0444);
MODULE_PARM_DESC(interrupt_mode,
		 "Interrupt mode (0=>MSIX 1=>MSI 2=>legacy)");

static int __init efx_init_module(void)
{
	int rc;

	printk(KERN_INFO "Solarflare NET driver v" EFX_DRIVER_VERSION "\n");

	rc = register_netdevice_notifier(&efx_netdev_notifier);
	if (rc)
		goto err_notifier;

	rc = efx_init_sriov();
	if (rc)
		goto err_sriov;

	reset_workqueue = create_singlethread_workqueue("sfc_reset");
	if (!reset_workqueue) {
		rc = -ENOMEM;
		goto err_reset;
	}

	rc = pci_register_driver(&efx_pci_driver);
	if (rc < 0)
		goto err_pci;

	return 0;

 err_pci:
	destroy_workqueue(reset_workqueue);
 err_reset:
	efx_fini_sriov();
 err_sriov:
	unregister_netdevice_notifier(&efx_netdev_notifier);
 err_notifier:
	return rc;
}

static void __exit efx_exit_module(void)
{
	printk(KERN_INFO "Solarflare NET driver unloading\n");

	pci_unregister_driver(&efx_pci_driver);
	destroy_workqueue(reset_workqueue);
	efx_fini_sriov();
	unregister_netdevice_notifier(&efx_netdev_notifier);

}

module_init(efx_init_module);
module_exit(efx_exit_module);

MODULE_AUTHOR("Solarflare Communications and "
	      "Michael Brown <mbrown@fensystems.co.uk>");
MODULE_DESCRIPTION("Solarflare Communications network driver");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, efx_pci_table);
