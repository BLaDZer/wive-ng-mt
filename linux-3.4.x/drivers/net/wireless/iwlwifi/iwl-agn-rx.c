/******************************************************************************
 *
 * Copyright(c) 2003 - 2012 Intel Corporation. All rights reserved.
 *
 * Portions of this file are derived from the ipw3945 project, as well
 * as portionhelp of the ieee80211 subsystem header files.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 *****************************************************************************/

#include <linux/etherdevice.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <net/mac80211.h>
#include <asm/unaligned.h>
#include "iwl-eeprom.h"
#include "iwl-dev.h"
#include "iwl-core.h"
#include "iwl-io.h"
#include "iwl-agn-calib.h"
#include "iwl-agn.h"
#include "iwl-shared.h"

const char *get_cmd_string(u8 cmd)
{
	switch (cmd) {
		IWL_CMD(REPLY_ALIVE);
		IWL_CMD(REPLY_ERROR);
		IWL_CMD(REPLY_ECHO);
		IWL_CMD(REPLY_RXON);
		IWL_CMD(REPLY_RXON_ASSOC);
		IWL_CMD(REPLY_QOS_PARAM);
		IWL_CMD(REPLY_RXON_TIMING);
		IWL_CMD(REPLY_ADD_STA);
		IWL_CMD(REPLY_REMOVE_STA);
		IWL_CMD(REPLY_REMOVE_ALL_STA);
		IWL_CMD(REPLY_TXFIFO_FLUSH);
		IWL_CMD(REPLY_WEPKEY);
		IWL_CMD(REPLY_TX);
		IWL_CMD(REPLY_LEDS_CMD);
		IWL_CMD(REPLY_TX_LINK_QUALITY_CMD);
		IWL_CMD(COEX_PRIORITY_TABLE_CMD);
		IWL_CMD(COEX_MEDIUM_NOTIFICATION);
		IWL_CMD(COEX_EVENT_CMD);
		IWL_CMD(REPLY_QUIET_CMD);
		IWL_CMD(REPLY_CHANNEL_SWITCH);
		IWL_CMD(CHANNEL_SWITCH_NOTIFICATION);
		IWL_CMD(REPLY_SPECTRUM_MEASUREMENT_CMD);
		IWL_CMD(SPECTRUM_MEASURE_NOTIFICATION);
		IWL_CMD(POWER_TABLE_CMD);
		IWL_CMD(PM_SLEEP_NOTIFICATION);
		IWL_CMD(PM_DEBUG_STATISTIC_NOTIFIC);
		IWL_CMD(REPLY_SCAN_CMD);
		IWL_CMD(REPLY_SCAN_ABORT_CMD);
		IWL_CMD(SCAN_START_NOTIFICATION);
		IWL_CMD(SCAN_RESULTS_NOTIFICATION);
		IWL_CMD(SCAN_COMPLETE_NOTIFICATION);
		IWL_CMD(BEACON_NOTIFICATION);
		IWL_CMD(REPLY_TX_BEACON);
		IWL_CMD(WHO_IS_AWAKE_NOTIFICATION);
		IWL_CMD(QUIET_NOTIFICATION);
		IWL_CMD(REPLY_TX_PWR_TABLE_CMD);
		IWL_CMD(MEASURE_ABORT_NOTIFICATION);
		IWL_CMD(REPLY_BT_CONFIG);
		IWL_CMD(REPLY_STATISTICS_CMD);
		IWL_CMD(STATISTICS_NOTIFICATION);
		IWL_CMD(REPLY_CARD_STATE_CMD);
		IWL_CMD(CARD_STATE_NOTIFICATION);
		IWL_CMD(MISSED_BEACONS_NOTIFICATION);
		IWL_CMD(REPLY_CT_KILL_CONFIG_CMD);
		IWL_CMD(SENSITIVITY_CMD);
		IWL_CMD(REPLY_PHY_CALIBRATION_CMD);
		IWL_CMD(REPLY_RX_PHY_CMD);
		IWL_CMD(REPLY_RX_MPDU_CMD);
		IWL_CMD(REPLY_RX);
		IWL_CMD(REPLY_COMPRESSED_BA);
		IWL_CMD(CALIBRATION_CFG_CMD);
		IWL_CMD(CALIBRATION_RES_NOTIFICATION);
		IWL_CMD(CALIBRATION_COMPLETE_NOTIFICATION);
		IWL_CMD(REPLY_TX_POWER_DBM_CMD);
		IWL_CMD(TEMPERATURE_NOTIFICATION);
		IWL_CMD(TX_ANT_CONFIGURATION_CMD);
		IWL_CMD(REPLY_BT_COEX_PROFILE_NOTIF);
		IWL_CMD(REPLY_BT_COEX_PRIO_TABLE);
		IWL_CMD(REPLY_BT_COEX_PROT_ENV);
		IWL_CMD(REPLY_WIPAN_PARAMS);
		IWL_CMD(REPLY_WIPAN_RXON);
		IWL_CMD(REPLY_WIPAN_RXON_TIMING);
		IWL_CMD(REPLY_WIPAN_RXON_ASSOC);
		IWL_CMD(REPLY_WIPAN_QOS_PARAM);
		IWL_CMD(REPLY_WIPAN_WEPKEY);
		IWL_CMD(REPLY_WIPAN_P2P_CHANNEL_SWITCH);
		IWL_CMD(REPLY_WIPAN_NOA_NOTIFICATION);
		IWL_CMD(REPLY_WIPAN_DEACTIVATION_COMPLETE);
		IWL_CMD(REPLY_WOWLAN_PATTERNS);
		IWL_CMD(REPLY_WOWLAN_WAKEUP_FILTER);
		IWL_CMD(REPLY_WOWLAN_TSC_RSC_PARAMS);
		IWL_CMD(REPLY_WOWLAN_TKIP_PARAMS);
		IWL_CMD(REPLY_WOWLAN_KEK_KCK_MATERIAL);
		IWL_CMD(REPLY_WOWLAN_GET_STATUS);
		IWL_CMD(REPLY_D3_CONFIG);
	default:
		return "UNKNOWN";

	}
}

/******************************************************************************
 *
 * Generic RX handler implementations
 *
 ******************************************************************************/

static int iwlagn_rx_reply_error(struct iwl_priv *priv,
			       struct iwl_rx_cmd_buffer *rxb,
			       struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_error_resp *err_resp = (void *)pkt->data;

	IWL_ERR(priv, "Error Reply type 0x%08X cmd %s (0x%02X) "
		"seq 0x%04X ser 0x%08X\n",
		le32_to_cpu(err_resp->error_type),
		get_cmd_string(err_resp->cmd_id),
		err_resp->cmd_id,
		le16_to_cpu(err_resp->bad_cmd_seq_num),
		le32_to_cpu(err_resp->error_info));
	return 0;
}

static int iwlagn_rx_csa(struct iwl_priv *priv, struct iwl_rx_cmd_buffer *rxb,
			       struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_csa_notification *csa = (void *)pkt->data;
	/*
	 * MULTI-FIXME
	 * See iwlagn_mac_channel_switch.
	 */
	struct iwl_rxon_context *ctx = &priv->contexts[IWL_RXON_CTX_BSS];
	struct iwl_rxon_cmd *rxon = (void *)&ctx->active;

	if (!test_bit(STATUS_CHANNEL_SWITCH_PENDING, &priv->status))
		return 0;

	if (!le32_to_cpu(csa->status) && csa->channel == priv->switch_channel) {
		rxon->channel = csa->channel;
		ctx->staging.channel = csa->channel;
		IWL_DEBUG_11H(priv, "CSA notif: channel %d\n",
			      le16_to_cpu(csa->channel));
		iwl_chswitch_done(priv, true);
	} else {
		IWL_ERR(priv, "CSA notif (fail) : channel %d\n",
			le16_to_cpu(csa->channel));
		iwl_chswitch_done(priv, false);
	}
	return 0;
}


static int iwlagn_rx_spectrum_measure_notif(struct iwl_priv *priv,
					  struct iwl_rx_cmd_buffer *rxb,
					  struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_spectrum_notification *report = (void *)pkt->data;

	if (!report->state) {
		IWL_DEBUG_11H(priv,
			"Spectrum Measure Notification: Start\n");
		return 0;
	}

	memcpy(&priv->measure_report, report, sizeof(*report));
	priv->measurement_status |= MEASUREMENT_READY;
	return 0;
}

static int iwlagn_rx_pm_sleep_notif(struct iwl_priv *priv,
				  struct iwl_rx_cmd_buffer *rxb,
				  struct iwl_device_cmd *cmd)
{
#ifdef CONFIG_IWLWIFI_DEBUG
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_sleep_notification *sleep = (void *)pkt->data;
	IWL_DEBUG_RX(priv, "sleep mode: %d, src: %d\n",
		     sleep->pm_sleep_mode, sleep->pm_wakeup_src);
#endif
	return 0;
}

static int iwlagn_rx_pm_debug_statistics_notif(struct iwl_priv *priv,
					     struct iwl_rx_cmd_buffer *rxb,
					     struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	u32 __maybe_unused len =
		le32_to_cpu(pkt->len_n_flags) & FH_RSCSR_FRAME_SIZE_MSK;
	IWL_DEBUG_RADIO(priv, "Dumping %d bytes of unhandled "
			"notification for %s:\n", len,
			get_cmd_string(pkt->hdr.cmd));
	iwl_print_hex_dump(priv, IWL_DL_RADIO, pkt->data, len);
	return 0;
}

static int iwlagn_rx_beacon_notif(struct iwl_priv *priv,
				struct iwl_rx_cmd_buffer *rxb,
				struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwlagn_beacon_notif *beacon = (void *)pkt->data;
#ifdef CONFIG_IWLWIFI_DEBUG
	u16 status = le16_to_cpu(beacon->beacon_notify_hdr.status.status);
	u8 rate = iwl_hw_get_rate(beacon->beacon_notify_hdr.rate_n_flags);

	IWL_DEBUG_RX(priv, "beacon status %#x, retries:%d ibssmgr:%d "
		"tsf:0x%.8x%.8x rate:%d\n",
		status & TX_STATUS_MSK,
		beacon->beacon_notify_hdr.failure_frame,
		le32_to_cpu(beacon->ibss_mgr_status),
		le32_to_cpu(beacon->high_tsf),
		le32_to_cpu(beacon->low_tsf), rate);
#endif

	priv->ibss_manager = le32_to_cpu(beacon->ibss_mgr_status);

	return 0;
}

/* the threshold ratio of actual_ack_cnt to expected_ack_cnt in percent */
#define ACK_CNT_RATIO (50)
#define BA_TIMEOUT_CNT (5)
#define BA_TIMEOUT_MAX (16)

/**
 * iwl_good_ack_health - checks for ACK count ratios, BA timeout retries.
 *
 * When the ACK count ratio is low and aggregated BA timeout retries exceeding
 * the BA_TIMEOUT_MAX, reload firmware and bring system back to normal
 * operation state.
 */
static bool iwlagn_good_ack_health(struct iwl_priv *priv,
				struct statistics_tx *cur)
{
	int actual_delta, expected_delta, ba_timeout_delta;
	struct statistics_tx *old;

	if (priv->agg_tids_count)
		return true;

	lockdep_assert_held(&priv->statistics.lock);

	old = &priv->statistics.tx;

	actual_delta = le32_to_cpu(cur->actual_ack_cnt) -
		       le32_to_cpu(old->actual_ack_cnt);
	expected_delta = le32_to_cpu(cur->expected_ack_cnt) -
			 le32_to_cpu(old->expected_ack_cnt);

	/* Values should not be negative, but we do not trust the firmware */
	if (actual_delta <= 0 || expected_delta <= 0)
		return true;

	ba_timeout_delta = le32_to_cpu(cur->agg.ba_timeout) -
			   le32_to_cpu(old->agg.ba_timeout);

	if ((actual_delta * 100 / expected_delta) < ACK_CNT_RATIO &&
	    ba_timeout_delta > BA_TIMEOUT_CNT) {
		IWL_DEBUG_RADIO(priv,
			"deltas: actual %d expected %d ba_timeout %d\n",
			actual_delta, expected_delta, ba_timeout_delta);

#ifdef CONFIG_IWLWIFI_DEBUGFS
		/*
		 * This is ifdef'ed on DEBUGFS because otherwise the
		 * statistics aren't available. If DEBUGFS is set but
		 * DEBUG is not, these will just compile out.
		 */
		IWL_DEBUG_RADIO(priv, "rx_detected_cnt delta %d\n",
				priv->delta_stats.tx.rx_detected_cnt);
		IWL_DEBUG_RADIO(priv,
				"ack_or_ba_timeout_collision delta %d\n",
				priv->delta_stats.tx.ack_or_ba_timeout_collision);
#endif

		if (ba_timeout_delta >= BA_TIMEOUT_MAX)
			return false;
	}

	return true;
}

/**
 * iwl_good_plcp_health - checks for plcp error.
 *
 * When the plcp error is exceeding the thresholds, reset the radio
 * to improve the throughput.
 */
static bool iwlagn_good_plcp_health(struct iwl_priv *priv,
				 struct statistics_rx_phy *cur_ofdm,
				 struct statistics_rx_ht_phy *cur_ofdm_ht,
				 unsigned int msecs)
{
	int delta;
	int threshold = priv->plcp_delta_threshold;

	if (threshold == IWL_MAX_PLCP_ERR_THRESHOLD_DISABLE) {
		IWL_DEBUG_RADIO(priv, "plcp_err check disabled\n");
		return true;
	}

	delta = le32_to_cpu(cur_ofdm->plcp_err) -
		le32_to_cpu(priv->statistics.rx_ofdm.plcp_err) +
		le32_to_cpu(cur_ofdm_ht->plcp_err) -
		le32_to_cpu(priv->statistics.rx_ofdm_ht.plcp_err);

	/* Can be negative if firmware reset statistics */
	if (delta <= 0)
		return true;

	if ((delta * 100 / msecs) > threshold) {
		IWL_DEBUG_RADIO(priv,
				"plcp health threshold %u delta %d msecs %u\n",
				threshold, delta, msecs);
		return false;
	}

	return true;
}

static void iwlagn_recover_from_statistics(struct iwl_priv *priv,
				struct statistics_rx_phy *cur_ofdm,
				struct statistics_rx_ht_phy *cur_ofdm_ht,
				struct statistics_tx *tx,
				unsigned long stamp)
{
	unsigned int msecs;

	if (test_bit(STATUS_EXIT_PENDING, &priv->status))
		return;

	msecs = jiffies_to_msecs(stamp - priv->rx_statistics_jiffies);

	/* Only gather statistics and update time stamp when not associated */
	if (!iwl_is_any_associated(priv))
		return;

	/* Do not check/recover when do not have enough statistics data */
	if (msecs < 99)
		return;

	if (iwlagn_mod_params.ack_check && !iwlagn_good_ack_health(priv, tx)) {
		IWL_ERR(priv, "low ack count detected, restart firmware\n");
		if (!iwl_force_reset(priv, IWL_FW_RESET, false))
			return;
	}

	if (iwlagn_mod_params.plcp_check &&
	    !iwlagn_good_plcp_health(priv, cur_ofdm, cur_ofdm_ht, msecs))
		iwl_force_reset(priv, IWL_RF_RESET, false);
}

/* Calculate noise level, based on measurements during network silence just
 *   before arriving beacon.  This measurement can be done only if we know
 *   exactly when to expect beacons, therefore only when we're associated. */
static void iwlagn_rx_calc_noise(struct iwl_priv *priv)
{
	struct statistics_rx_non_phy *rx_info;
	int num_active_rx = 0;
	int total_silence = 0;
	int bcn_silence_a, bcn_silence_b, bcn_silence_c;
	int last_rx_noise;

	rx_info = &priv->statistics.rx_non_phy;

	bcn_silence_a =
		le32_to_cpu(rx_info->beacon_silence_rssi_a) & IN_BAND_FILTER;
	bcn_silence_b =
		le32_to_cpu(rx_info->beacon_silence_rssi_b) & IN_BAND_FILTER;
	bcn_silence_c =
		le32_to_cpu(rx_info->beacon_silence_rssi_c) & IN_BAND_FILTER;

	if (bcn_silence_a) {
		total_silence += bcn_silence_a;
		num_active_rx++;
	}
	if (bcn_silence_b) {
		total_silence += bcn_silence_b;
		num_active_rx++;
	}
	if (bcn_silence_c) {
		total_silence += bcn_silence_c;
		num_active_rx++;
	}

	/* Average among active antennas */
	if (num_active_rx)
		last_rx_noise = (total_silence / num_active_rx) - 107;
	else
		last_rx_noise = IWL_NOISE_MEAS_NOT_AVAILABLE;

	IWL_DEBUG_CALIB(priv, "inband silence a %u, b %u, c %u, dBm %d\n",
			bcn_silence_a, bcn_silence_b, bcn_silence_c,
			last_rx_noise);
}

#ifdef CONFIG_IWLWIFI_DEBUGFS
/*
 *  based on the assumption of all statistics counter are in DWORD
 *  FIXME: This function is for debugging, do not deal with
 *  the case of counters roll-over.
 */
static void accum_stats(__le32 *prev, __le32 *cur, __le32 *delta,
			__le32 *max_delta, __le32 *accum, int size)
{
	int i;

	for (i = 0;
	     i < size / sizeof(__le32);
	     i++, prev++, cur++, delta++, max_delta++, accum++) {
		if (le32_to_cpu(*cur) > le32_to_cpu(*prev)) {
			*delta = cpu_to_le32(
				le32_to_cpu(*cur) - le32_to_cpu(*prev));
			le32_add_cpu(accum, le32_to_cpu(*delta));
			if (le32_to_cpu(*delta) > le32_to_cpu(*max_delta))
				*max_delta = *delta;
		}
	}
}

static void
iwlagn_accumulative_statistics(struct iwl_priv *priv,
			    struct statistics_general_common *common,
			    struct statistics_rx_non_phy *rx_non_phy,
			    struct statistics_rx_phy *rx_ofdm,
			    struct statistics_rx_ht_phy *rx_ofdm_ht,
			    struct statistics_rx_phy *rx_cck,
			    struct statistics_tx *tx,
			    struct statistics_bt_activity *bt_activity)
{
#define ACCUM(_name)	\
	accum_stats((__le32 *)&priv->statistics._name,		\
		    (__le32 *)_name,				\
		    (__le32 *)&priv->delta_stats._name,		\
		    (__le32 *)&priv->max_delta_stats._name,	\
		    (__le32 *)&priv->accum_stats._name,		\
		    sizeof(*_name));

	ACCUM(common);
	ACCUM(rx_non_phy);
	ACCUM(rx_ofdm);
	ACCUM(rx_ofdm_ht);
	ACCUM(rx_cck);
	ACCUM(tx);
	if (bt_activity)
		ACCUM(bt_activity);
#undef ACCUM
}
#else
static inline void
iwlagn_accumulative_statistics(struct iwl_priv *priv,
			    struct statistics_general_common *common,
			    struct statistics_rx_non_phy *rx_non_phy,
			    struct statistics_rx_phy *rx_ofdm,
			    struct statistics_rx_ht_phy *rx_ofdm_ht,
			    struct statistics_rx_phy *rx_cck,
			    struct statistics_tx *tx,
			    struct statistics_bt_activity *bt_activity)
{
}
#endif

static int iwlagn_rx_statistics(struct iwl_priv *priv,
			      struct iwl_rx_cmd_buffer *rxb,
			      struct iwl_device_cmd *cmd)
{
	unsigned long stamp = jiffies;
	const int reg_recalib_period = 60;
	int change;
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	u32 len = le32_to_cpu(pkt->len_n_flags) & FH_RSCSR_FRAME_SIZE_MSK;
	__le32 *flag;
	struct statistics_general_common *common;
	struct statistics_rx_non_phy *rx_non_phy;
	struct statistics_rx_phy *rx_ofdm;
	struct statistics_rx_ht_phy *rx_ofdm_ht;
	struct statistics_rx_phy *rx_cck;
	struct statistics_tx *tx;
	struct statistics_bt_activity *bt_activity;

	len -= sizeof(struct iwl_cmd_header); /* skip header */

	IWL_DEBUG_RX(priv, "Statistics notification received (%d bytes).\n",
		     len);

	spin_lock(&priv->statistics.lock);

	if (len == sizeof(struct iwl_bt_notif_statistics)) {
		struct iwl_bt_notif_statistics *stats;
		stats = (void *)&pkt->data;
		flag = &stats->flag;
		common = &stats->general.common;
		rx_non_phy = &stats->rx.general.common;
		rx_ofdm = &stats->rx.ofdm;
		rx_ofdm_ht = &stats->rx.ofdm_ht;
		rx_cck = &stats->rx.cck;
		tx = &stats->tx;
		bt_activity = &stats->general.activity;

#ifdef CONFIG_IWLWIFI_DEBUGFS
		/* handle this exception directly */
		priv->statistics.num_bt_kills = stats->rx.general.num_bt_kills;
		le32_add_cpu(&priv->statistics.accum_num_bt_kills,
			     le32_to_cpu(stats->rx.general.num_bt_kills));
#endif
	} else if (len == sizeof(struct iwl_notif_statistics)) {
		struct iwl_notif_statistics *stats;
		stats = (void *)&pkt->data;
		flag = &stats->flag;
		common = &stats->general.common;
		rx_non_phy = &stats->rx.general;
		rx_ofdm = &stats->rx.ofdm;
		rx_ofdm_ht = &stats->rx.ofdm_ht;
		rx_cck = &stats->rx.cck;
		tx = &stats->tx;
		bt_activity = NULL;
	} else {
		WARN_ONCE(1, "len %d doesn't match BT (%zu) or normal (%zu)\n",
			  len, sizeof(struct iwl_bt_notif_statistics),
			  sizeof(struct iwl_notif_statistics));
		spin_unlock(&priv->statistics.lock);
		return 0;
	}

	change = common->temperature != priv->statistics.common.temperature ||
		 (*flag & STATISTICS_REPLY_FLG_HT40_MODE_MSK) !=
		 (priv->statistics.flag & STATISTICS_REPLY_FLG_HT40_MODE_MSK);

	iwlagn_accumulative_statistics(priv, common, rx_non_phy, rx_ofdm,
				    rx_ofdm_ht, rx_cck, tx, bt_activity);

	iwlagn_recover_from_statistics(priv, rx_ofdm, rx_ofdm_ht, tx, stamp);

	priv->statistics.flag = *flag;
	memcpy(&priv->statistics.common, common, sizeof(*common));
	memcpy(&priv->statistics.rx_non_phy, rx_non_phy, sizeof(*rx_non_phy));
	memcpy(&priv->statistics.rx_ofdm, rx_ofdm, sizeof(*rx_ofdm));
	memcpy(&priv->statistics.rx_ofdm_ht, rx_ofdm_ht, sizeof(*rx_ofdm_ht));
	memcpy(&priv->statistics.rx_cck, rx_cck, sizeof(*rx_cck));
	memcpy(&priv->statistics.tx, tx, sizeof(*tx));
#ifdef CONFIG_IWLWIFI_DEBUGFS
	if (bt_activity)
		memcpy(&priv->statistics.bt_activity, bt_activity,
			sizeof(*bt_activity));
#endif

	priv->rx_statistics_jiffies = stamp;

	set_bit(STATUS_STATISTICS, &priv->status);

	/* Reschedule the statistics timer to occur in
	 * reg_recalib_period seconds to ensure we get a
	 * thermal update even if the uCode doesn't give
	 * us one */
	mod_timer(&priv->statistics_periodic, jiffies +
		  msecs_to_jiffies(reg_recalib_period * 1000));

	if (unlikely(!test_bit(STATUS_SCANNING, &priv->status)) &&
	    (pkt->hdr.cmd == STATISTICS_NOTIFICATION)) {
		iwlagn_rx_calc_noise(priv);
		queue_work(priv->workqueue, &priv->run_time_calib_work);
	}
	if (cfg(priv)->lib->temperature && change)
		cfg(priv)->lib->temperature(priv);

	spin_unlock(&priv->statistics.lock);

	return 0;
}

static int iwlagn_rx_reply_statistics(struct iwl_priv *priv,
				    struct iwl_rx_cmd_buffer *rxb,
				    struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_notif_statistics *stats = (void *)pkt->data;

	if (le32_to_cpu(stats->flag) & UCODE_STATISTICS_CLEAR_MSK) {
#ifdef CONFIG_IWLWIFI_DEBUGFS
		memset(&priv->accum_stats, 0,
			sizeof(priv->accum_stats));
		memset(&priv->delta_stats, 0,
			sizeof(priv->delta_stats));
		memset(&priv->max_delta_stats, 0,
			sizeof(priv->max_delta_stats));
#endif
		IWL_DEBUG_RX(priv, "Statistics have been cleared\n");
	}
	iwlagn_rx_statistics(priv, rxb, cmd);
	return 0;
}

/* Handle notification from uCode that card's power state is changing
 * due to software, hardware, or critical temperature RFKILL */
static int iwlagn_rx_card_state_notif(struct iwl_priv *priv,
				    struct iwl_rx_cmd_buffer *rxb,
				    struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_card_state_notif *card_state_notif = (void *)pkt->data;
	u32 flags = le32_to_cpu(card_state_notif->flags);
	unsigned long status = priv->status;

	IWL_DEBUG_RF_KILL(priv, "Card state received: HW:%s SW:%s CT:%s\n",
			  (flags & HW_CARD_DISABLED) ? "Kill" : "On",
			  (flags & SW_CARD_DISABLED) ? "Kill" : "On",
			  (flags & CT_CARD_DISABLED) ?
			  "Reached" : "Not reached");

	if (flags & (SW_CARD_DISABLED | HW_CARD_DISABLED |
		     CT_CARD_DISABLED)) {

		iwl_write32(trans(priv), CSR_UCODE_DRV_GP1_SET,
			    CSR_UCODE_DRV_GP1_BIT_CMD_BLOCKED);

		iwl_write_direct32(trans(priv), HBUS_TARG_MBX_C,
					HBUS_TARG_MBX_C_REG_BIT_CMD_BLOCKED);

		if (!(flags & RXON_CARD_DISABLED)) {
			iwl_write32(trans(priv), CSR_UCODE_DRV_GP1_CLR,
				    CSR_UCODE_DRV_GP1_BIT_CMD_BLOCKED);
			iwl_write_direct32(trans(priv), HBUS_TARG_MBX_C,
					HBUS_TARG_MBX_C_REG_BIT_CMD_BLOCKED);
		}
		if (flags & CT_CARD_DISABLED)
			iwl_tt_enter_ct_kill(priv);
	}
	if (!(flags & CT_CARD_DISABLED))
		iwl_tt_exit_ct_kill(priv);

	if (flags & HW_CARD_DISABLED)
		set_bit(STATUS_RF_KILL_HW, &priv->status);
	else
		clear_bit(STATUS_RF_KILL_HW, &priv->status);


	if (!(flags & RXON_CARD_DISABLED))
		iwl_scan_cancel(priv);

	if ((test_bit(STATUS_RF_KILL_HW, &status) !=
	     test_bit(STATUS_RF_KILL_HW, &priv->status)))
		wiphy_rfkill_set_hw_state(priv->hw->wiphy,
			test_bit(STATUS_RF_KILL_HW, &priv->status));
	else
		wake_up(&trans(priv)->wait_command_queue);
	return 0;
}

static int iwlagn_rx_missed_beacon_notif(struct iwl_priv *priv,
				       struct iwl_rx_cmd_buffer *rxb,
				       struct iwl_device_cmd *cmd)

{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_missed_beacon_notif *missed_beacon = (void *)pkt->data;

	if (le32_to_cpu(missed_beacon->consecutive_missed_beacons) >
	    priv->missed_beacon_threshold) {
		IWL_DEBUG_CALIB(priv,
		    "missed bcn cnsq %d totl %d rcd %d expctd %d\n",
		    le32_to_cpu(missed_beacon->consecutive_missed_beacons),
		    le32_to_cpu(missed_beacon->total_missed_becons),
		    le32_to_cpu(missed_beacon->num_recvd_beacons),
		    le32_to_cpu(missed_beacon->num_expected_beacons));
		if (!test_bit(STATUS_SCANNING, &priv->status))
			iwl_init_sensitivity(priv);
	}
	return 0;
}

/* Cache phy data (Rx signal strength, etc) for HT frame (REPLY_RX_PHY_CMD).
 * This will be used later in iwl_rx_reply_rx() for REPLY_RX_MPDU_CMD. */
static int iwlagn_rx_reply_rx_phy(struct iwl_priv *priv,
				struct iwl_rx_cmd_buffer *rxb,
				struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);

	priv->last_phy_res_valid = true;
	memcpy(&priv->last_phy_res, pkt->data,
	       sizeof(struct iwl_rx_phy_res));
	return 0;
}

/*
 * returns non-zero if packet should be dropped
 */
static int iwlagn_set_decrypted_flag(struct iwl_priv *priv,
				  struct ieee80211_hdr *hdr,
				  u32 decrypt_res,
				  struct ieee80211_rx_status *stats)
{
	u16 fc = le16_to_cpu(hdr->frame_control);

	/*
	 * All contexts have the same setting here due to it being
	 * a module parameter, so OK to check any context.
	 */
	if (priv->contexts[IWL_RXON_CTX_BSS].active.filter_flags &
						RXON_FILTER_DIS_DECRYPT_MSK)
		return 0;

	if (!(fc & IEEE80211_FCTL_PROTECTED))
		return 0;

	IWL_DEBUG_RX(priv, "decrypt_res:0x%x\n", decrypt_res);
	switch (decrypt_res & RX_RES_STATUS_SEC_TYPE_MSK) {
	case RX_RES_STATUS_SEC_TYPE_TKIP:
		/* The uCode has got a bad phase 1 Key, pushes the packet.
		 * Decryption will be done in SW. */
		if ((decrypt_res & RX_RES_STATUS_DECRYPT_TYPE_MSK) ==
		    RX_RES_STATUS_BAD_KEY_TTAK)
			break;

	case RX_RES_STATUS_SEC_TYPE_WEP:
		if ((decrypt_res & RX_RES_STATUS_DECRYPT_TYPE_MSK) ==
		    RX_RES_STATUS_BAD_ICV_MIC) {
			/* bad ICV, the packet is destroyed since the
			 * decryption is inplace, drop it */
			IWL_DEBUG_RX(priv, "Packet destroyed\n");
			return -1;
		}
	case RX_RES_STATUS_SEC_TYPE_CCMP:
		if ((decrypt_res & RX_RES_STATUS_DECRYPT_TYPE_MSK) ==
		    RX_RES_STATUS_DECRYPT_OK) {
			IWL_DEBUG_RX(priv, "hw decrypt successfully!!!\n");
			stats->flag |= RX_FLAG_DECRYPTED;
		}
		break;

	default:
		break;
	}
	return 0;
}

static void iwlagn_pass_packet_to_mac80211(struct iwl_priv *priv,
					struct ieee80211_hdr *hdr,
					u16 len,
					u32 ampdu_status,
					struct iwl_rx_cmd_buffer *rxb,
					struct ieee80211_rx_status *stats)
{
	struct sk_buff *skb;
	__le16 fc = hdr->frame_control;
	struct iwl_rxon_context *ctx;
	unsigned int hdrlen, fraglen;

	/* We only process data packets if the interface is open */
	if (unlikely(!priv->is_open)) {
		IWL_DEBUG_DROP_LIMIT(priv,
		    "Dropping packet while interface is not open.\n");
		return;
	}

	/* In case of HW accelerated crypto and bad decryption, drop */
	if (!iwlagn_mod_params.sw_crypto &&
	    iwlagn_set_decrypted_flag(priv, hdr, ampdu_status, stats))
		return;

	/* Dont use dev_alloc_skb(), we'll have enough headroom once
	 * ieee80211_hdr pulled.
	 */
	skb = alloc_skb(128, GFP_ATOMIC);
	if (!skb) {
		IWL_ERR(priv, "alloc_skb failed\n");
		return;
	}
	hdrlen = min_t(unsigned int, len, skb_tailroom(skb));
	memcpy(skb_put(skb, hdrlen), hdr, hdrlen);
	fraglen = len - hdrlen;

	if (fraglen) {
		int offset = (void *)hdr + hdrlen - rxb_addr(rxb);

		skb_add_rx_frag(skb, 0, rxb_steal_page(rxb), offset,
				fraglen, rxb->truesize);
	}
	iwl_update_stats(priv, false, fc, len);

	/*
	* Wake any queues that were stopped due to a passive channel tx
	* failure. This can happen because the regulatory enforcement in
	* the device waits for a beacon before allowing transmission,
	* sometimes even after already having transmitted frames for the
	* association because the new RXON may reset the information.
	*/
	if (unlikely(ieee80211_is_beacon(fc) && priv->passive_no_rx)) {
		for_each_context(priv, ctx) {
			if (!ether_addr_equal(hdr->addr3,
					      ctx->active.bssid_addr))
				continue;
			iwlagn_lift_passive_no_rx(priv);
		}
	}

	memcpy(IEEE80211_SKB_RXCB(skb), stats, sizeof(*stats));

	ieee80211_rx(priv->hw, skb);
}

static u32 iwlagn_translate_rx_status(struct iwl_priv *priv, u32 decrypt_in)
{
	u32 decrypt_out = 0;

	if ((decrypt_in & RX_RES_STATUS_STATION_FOUND) ==
					RX_RES_STATUS_STATION_FOUND)
		decrypt_out |= (RX_RES_STATUS_STATION_FOUND |
				RX_RES_STATUS_NO_STATION_INFO_MISMATCH);

	decrypt_out |= (decrypt_in & RX_RES_STATUS_SEC_TYPE_MSK);

	/* packet was not encrypted */
	if ((decrypt_in & RX_RES_STATUS_SEC_TYPE_MSK) ==
					RX_RES_STATUS_SEC_TYPE_NONE)
		return decrypt_out;

	/* packet was encrypted with unknown alg */
	if ((decrypt_in & RX_RES_STATUS_SEC_TYPE_MSK) ==
					RX_RES_STATUS_SEC_TYPE_ERR)
		return decrypt_out;

	/* decryption was not done in HW */
	if ((decrypt_in & RX_MPDU_RES_STATUS_DEC_DONE_MSK) !=
					RX_MPDU_RES_STATUS_DEC_DONE_MSK)
		return decrypt_out;

	switch (decrypt_in & RX_RES_STATUS_SEC_TYPE_MSK) {

	case RX_RES_STATUS_SEC_TYPE_CCMP:
		/* alg is CCM: check MIC only */
		if (!(decrypt_in & RX_MPDU_RES_STATUS_MIC_OK))
			/* Bad MIC */
			decrypt_out |= RX_RES_STATUS_BAD_ICV_MIC;
		else
			decrypt_out |= RX_RES_STATUS_DECRYPT_OK;

		break;

	case RX_RES_STATUS_SEC_TYPE_TKIP:
		if (!(decrypt_in & RX_MPDU_RES_STATUS_TTAK_OK)) {
			/* Bad TTAK */
			decrypt_out |= RX_RES_STATUS_BAD_KEY_TTAK;
			break;
		}
		/* fall through if TTAK OK */
	default:
		if (!(decrypt_in & RX_MPDU_RES_STATUS_ICV_OK))
			decrypt_out |= RX_RES_STATUS_BAD_ICV_MIC;
		else
			decrypt_out |= RX_RES_STATUS_DECRYPT_OK;
		break;
	}

	IWL_DEBUG_RX(priv, "decrypt_in:0x%x  decrypt_out = 0x%x\n",
					decrypt_in, decrypt_out);

	return decrypt_out;
}

/* Calc max signal level (dBm) among 3 possible receivers */
static int iwlagn_calc_rssi(struct iwl_priv *priv,
			     struct iwl_rx_phy_res *rx_resp)
{
	/* data from PHY/DSP regarding signal strength, etc.,
	 *   contents are always there, not configurable by host
	 */
	struct iwlagn_non_cfg_phy *ncphy =
		(struct iwlagn_non_cfg_phy *)rx_resp->non_cfg_phy_buf;
	u32 val, rssi_a, rssi_b, rssi_c, max_rssi;
	u8 agc;

	val  = le32_to_cpu(ncphy->non_cfg_phy[IWLAGN_RX_RES_AGC_IDX]);
	agc = (val & IWLAGN_OFDM_AGC_MSK) >> IWLAGN_OFDM_AGC_BIT_POS;

	/* Find max rssi among 3 possible receivers.
	 * These values are measured by the digital signal processor (DSP).
	 * They should stay fairly constant even as the signal strength varies,
	 *   if the radio's automatic gain control (AGC) is working right.
	 * AGC value (see below) will provide the "interesting" info.
	 */
	val = le32_to_cpu(ncphy->non_cfg_phy[IWLAGN_RX_RES_RSSI_AB_IDX]);
	rssi_a = (val & IWLAGN_OFDM_RSSI_INBAND_A_BITMSK) >>
		IWLAGN_OFDM_RSSI_A_BIT_POS;
	rssi_b = (val & IWLAGN_OFDM_RSSI_INBAND_B_BITMSK) >>
		IWLAGN_OFDM_RSSI_B_BIT_POS;
	val = le32_to_cpu(ncphy->non_cfg_phy[IWLAGN_RX_RES_RSSI_C_IDX]);
	rssi_c = (val & IWLAGN_OFDM_RSSI_INBAND_C_BITMSK) >>
		IWLAGN_OFDM_RSSI_C_BIT_POS;

	max_rssi = max_t(u32, rssi_a, rssi_b);
	max_rssi = max_t(u32, max_rssi, rssi_c);

	IWL_DEBUG_STATS(priv, "Rssi In A %d B %d C %d Max %d AGC dB %d\n",
		rssi_a, rssi_b, rssi_c, max_rssi, agc);

	/* dBm = max_rssi dB - agc dB - constant.
	 * Higher AGC (higher radio gain) means lower signal. */
	return max_rssi - agc - IWLAGN_RSSI_OFFSET;
}

/* Called for REPLY_RX (legacy ABG frames), or
 * REPLY_RX_MPDU_CMD (HT high-throughput N frames). */
static int iwlagn_rx_reply_rx(struct iwl_priv *priv,
			    struct iwl_rx_cmd_buffer *rxb,
			    struct iwl_device_cmd *cmd)
{
	struct ieee80211_hdr *header;
	struct ieee80211_rx_status rx_status;
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_rx_phy_res *phy_res;
	__le32 rx_pkt_status;
	struct iwl_rx_mpdu_res_start *amsdu;
	u32 len;
	u32 ampdu_status;
	u32 rate_n_flags;

	/**
	 * REPLY_RX and REPLY_RX_MPDU_CMD are handled differently.
	 *	REPLY_RX: physical layer info is in this buffer
	 *	REPLY_RX_MPDU_CMD: physical layer info was sent in separate
	 *		command and cached in priv->last_phy_res
	 *
	 * Here we set up local variables depending on which command is
	 * received.
	 */
	if (pkt->hdr.cmd == REPLY_RX) {
		phy_res = (struct iwl_rx_phy_res *)pkt->data;
		header = (struct ieee80211_hdr *)(pkt->data + sizeof(*phy_res)
				+ phy_res->cfg_phy_cnt);

		len = le16_to_cpu(phy_res->byte_count);
		rx_pkt_status = *(__le32 *)(pkt->data + sizeof(*phy_res) +
				phy_res->cfg_phy_cnt + len);
		ampdu_status = le32_to_cpu(rx_pkt_status);
	} else {
		if (!priv->last_phy_res_valid) {
			IWL_ERR(priv, "MPDU frame without cached PHY data\n");
			return 0;
		}
		phy_res = &priv->last_phy_res;
		amsdu = (struct iwl_rx_mpdu_res_start *)pkt->data;
		header = (struct ieee80211_hdr *)(pkt->data + sizeof(*amsdu));
		len = le16_to_cpu(amsdu->byte_count);
		rx_pkt_status = *(__le32 *)(pkt->data + sizeof(*amsdu) + len);
		ampdu_status = iwlagn_translate_rx_status(priv,
						le32_to_cpu(rx_pkt_status));
	}

	if ((unlikely(phy_res->cfg_phy_cnt > 20))) {
		IWL_DEBUG_DROP(priv, "dsp size out of range [0,20]: %d/n",
				phy_res->cfg_phy_cnt);
		return 0;
	}

	if (!(rx_pkt_status & RX_RES_STATUS_NO_CRC32_ERROR) ||
	    !(rx_pkt_status & RX_RES_STATUS_NO_RXE_OVERFLOW)) {
		IWL_DEBUG_RX(priv, "Bad CRC or FIFO: 0x%08X.\n",
				le32_to_cpu(rx_pkt_status));
		return 0;
	}

	/* This will be used in several places later */
	rate_n_flags = le32_to_cpu(phy_res->rate_n_flags);

	/* rx_status carries information about the packet to mac80211 */
	rx_status.mactime = le64_to_cpu(phy_res->timestamp);
	rx_status.band = (phy_res->phy_flags & RX_RES_PHY_FLAGS_BAND_24_MSK) ?
				IEEE80211_BAND_2GHZ : IEEE80211_BAND_5GHZ;
	rx_status.freq =
		ieee80211_channel_to_frequency(le16_to_cpu(phy_res->channel),
					       rx_status.band);
	rx_status.rate_idx =
		iwlagn_hwrate_to_mac80211_idx(rate_n_flags, rx_status.band);
	rx_status.flag = 0;

	/* TSF isn't reliable. In order to allow smooth user experience,
	 * this W/A doesn't propagate it to the mac80211 */
	/*rx_status.flag |= RX_FLAG_MACTIME_MPDU;*/

	priv->ucode_beacon_time = le32_to_cpu(phy_res->beacon_time_stamp);

	/* Find max signal strength (dBm) among 3 antenna/receiver chains */
	rx_status.signal = iwlagn_calc_rssi(priv, phy_res);

	iwl_dbg_log_rx_data_frame(priv, len, header);
	IWL_DEBUG_STATS_LIMIT(priv, "Rssi %d, TSF %llu\n",
		rx_status.signal, (unsigned long long)rx_status.mactime);

	/*
	 * "antenna number"
	 *
	 * It seems that the antenna field in the phy flags value
	 * is actually a bit field. This is undefined by radiotap,
	 * it wants an actual antenna number but I always get "7"
	 * for most legacy frames I receive indicating that the
	 * same frame was received on all three RX chains.
	 *
	 * I think this field should be removed in favor of a
	 * new 802.11n radiotap field "RX chains" that is defined
	 * as a bitmask.
	 */
	rx_status.antenna =
		(le16_to_cpu(phy_res->phy_flags) & RX_RES_PHY_FLAGS_ANTENNA_MSK)
		>> RX_RES_PHY_FLAGS_ANTENNA_POS;

	/* set the preamble flag if appropriate */
	if (phy_res->phy_flags & RX_RES_PHY_FLAGS_SHORT_PREAMBLE_MSK)
		rx_status.flag |= RX_FLAG_SHORTPRE;

	/* Set up the HT phy flags */
	if (rate_n_flags & RATE_MCS_HT_MSK)
		rx_status.flag |= RX_FLAG_HT;
	if (rate_n_flags & RATE_MCS_HT40_MSK)
		rx_status.flag |= RX_FLAG_40MHZ;
	if (rate_n_flags & RATE_MCS_SGI_MSK)
		rx_status.flag |= RX_FLAG_SHORT_GI;

	iwlagn_pass_packet_to_mac80211(priv, header, len, ampdu_status,
				    rxb, &rx_status);
	return 0;
}

static int iwlagn_rx_noa_notification(struct iwl_priv *priv,
				      struct iwl_rx_cmd_buffer *rxb,
				      struct iwl_device_cmd *cmd)
{
	struct iwl_wipan_noa_data *new_data, *old_data;
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_wipan_noa_notification *noa_notif = (void *)pkt->data;

	/* no condition -- we're in softirq */
	old_data = rcu_dereference_protected(priv->noa_data, true);

	if (noa_notif->noa_active) {
		u32 len = le16_to_cpu(noa_notif->noa_attribute.length);
		u32 copylen = len;

		/* EID, len, OUI, subtype */
		len += 1 + 1 + 3 + 1;
		/* P2P id, P2P length */
		len += 1 + 2;
		copylen += 1 + 2;

		new_data = kmalloc(sizeof(*new_data) + len, GFP_ATOMIC);
		if (new_data) {
			new_data->length = len;
			new_data->data[0] = WLAN_EID_VENDOR_SPECIFIC;
			new_data->data[1] = len - 2; /* not counting EID, len */
			new_data->data[2] = (WLAN_OUI_WFA >> 16) & 0xff;
			new_data->data[3] = (WLAN_OUI_WFA >> 8) & 0xff;
			new_data->data[4] = (WLAN_OUI_WFA >> 0) & 0xff;
			new_data->data[5] = WLAN_OUI_TYPE_WFA_P2P;
			memcpy(&new_data->data[6], &noa_notif->noa_attribute,
			       copylen);
		}
	} else
		new_data = NULL;

	rcu_assign_pointer(priv->noa_data, new_data);

	if (old_data)
		kfree_rcu(old_data, rcu_head);

	return 0;
}

/**
 * iwl_setup_rx_handlers - Initialize Rx handler callbacks
 *
 * Setup the RX handlers for each of the reply types sent from the uCode
 * to the host.
 */
void iwl_setup_rx_handlers(struct iwl_priv *priv)
{
	int (**handlers)(struct iwl_priv *priv, struct iwl_rx_cmd_buffer *rxb,
			       struct iwl_device_cmd *cmd);

	handlers = priv->rx_handlers;

	handlers[REPLY_ERROR]			= iwlagn_rx_reply_error;
	handlers[CHANNEL_SWITCH_NOTIFICATION]	= iwlagn_rx_csa;
	handlers[SPECTRUM_MEASURE_NOTIFICATION]	=
		iwlagn_rx_spectrum_measure_notif;
	handlers[PM_SLEEP_NOTIFICATION]		= iwlagn_rx_pm_sleep_notif;
	handlers[PM_DEBUG_STATISTIC_NOTIFIC]	=
		iwlagn_rx_pm_debug_statistics_notif;
	handlers[BEACON_NOTIFICATION]		= iwlagn_rx_beacon_notif;
	handlers[REPLY_ADD_STA]			= iwl_add_sta_callback;

	handlers[REPLY_WIPAN_NOA_NOTIFICATION]	= iwlagn_rx_noa_notification;

	/*
	 * The same handler is used for both the REPLY to a discrete
	 * statistics request from the host as well as for the periodic
	 * statistics notifications (after received beacons) from the uCode.
	 */
	handlers[REPLY_STATISTICS_CMD]		= iwlagn_rx_reply_statistics;
	handlers[STATISTICS_NOTIFICATION]	= iwlagn_rx_statistics;

	iwl_setup_rx_scan_handlers(priv);

	handlers[CARD_STATE_NOTIFICATION]	= iwlagn_rx_card_state_notif;
	handlers[MISSED_BEACONS_NOTIFICATION]	=
		iwlagn_rx_missed_beacon_notif;

	/* Rx handlers */
	handlers[REPLY_RX_PHY_CMD]		= iwlagn_rx_reply_rx_phy;
	handlers[REPLY_RX_MPDU_CMD]		= iwlagn_rx_reply_rx;

	/* block ack */
	handlers[REPLY_COMPRESSED_BA]		=
		iwlagn_rx_reply_compressed_ba;

	/* init calibration handlers */
	priv->rx_handlers[CALIBRATION_RES_NOTIFICATION] =
					iwlagn_rx_calib_result;
	priv->rx_handlers[REPLY_TX] = iwlagn_rx_reply_tx;

	/* set up notification wait support */
	iwl_notification_wait_init(&priv->notif_wait);

	/* Set up BT Rx handlers */
	if (cfg(priv)->bt_params)
		iwlagn_bt_rx_handler_setup(priv);
}

int iwl_rx_dispatch(struct iwl_op_mode *op_mode, struct iwl_rx_cmd_buffer *rxb,
		    struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_priv *priv = IWL_OP_MODE_GET_DVM(op_mode);
	void (*pre_rx_handler)(struct iwl_priv *,
			       struct iwl_rx_cmd_buffer *);
	int err = 0;

	/*
	 * Do the notification wait before RX handlers so
	 * even if the RX handler consumes the RXB we have
	 * access to it in the notification wait entry.
	 */
	iwl_notification_wait_notify(&priv->notif_wait, pkt);

	/* RX data may be forwarded to userspace (using pre_rx_handler) in one
	 * of two cases: the first, that the user owns the uCode through
	 * testmode - in such case the pre_rx_handler is set and no further
	 * processing takes place. The other case is when the user want to
	 * monitor the rx w/o affecting the regular flow - the pre_rx_handler
	 * will be set but the ownership flag != IWL_OWNERSHIP_TM and the flow
	 * continues.
	 * We need to use ACCESS_ONCE to prevent a case where the handler
	 * changes between the check and the call.
	 */
	pre_rx_handler = READ_ONCE(priv->pre_rx_handler);
	if (pre_rx_handler)
		pre_rx_handler(priv, rxb);
	if (priv->ucode_owner != IWL_OWNERSHIP_TM) {
		/* Based on type of command response or notification,
		 *   handle those that need handling via function in
		 *   rx_handlers table.  See iwl_setup_rx_handlers() */
		if (priv->rx_handlers[pkt->hdr.cmd]) {
			priv->rx_handlers_stats[pkt->hdr.cmd]++;
			err = priv->rx_handlers[pkt->hdr.cmd] (priv, rxb, cmd);
		} else {
			/* No handling needed */
			IWL_DEBUG_RX(priv,
				"No handler needed for %s, 0x%02x\n",
				get_cmd_string(pkt->hdr.cmd), pkt->hdr.cmd);
		}
	}
	return err;
}
