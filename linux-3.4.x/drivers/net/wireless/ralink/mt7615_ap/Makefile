DRV_NAME 		:= mt7615_ap
CHIP_MT7615E 		:= y
SRC_DIR 		:= ../mt7615
SRC_EMBEDDED_DIR	:= ../mt7615/embedded

EXTRA_CFLAGS 		:= -Idrivers/net/wireless/ralink/mt7615/include \
				-Idrivers/net/wireless/ralink/mt7615/embedded/include \
				-Idrivers/net/wireless/ralink/mt7615/ate/include

EXTRA_CFLAGS		:= -Idrivers/net/wireless/ralink/mt7615/include \
				-Idrivers/net/wireless/ralink/mt7615/embedded/include \
				-Idrivers/net/wireless/ralink/mt7615/ate/include

obj-$(CONFIG_MT7615_AP) += $(DRV_NAME).o

ifdef CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_FIRST_IF_EPAELNA
endif
ifdef CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_SECOND_IF_EPAELNA
endif
ifdef CONFIG_RT_THIRD_IF_EXTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_THIRD_IF_EPAELNA
endif

ifdef CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_FIRST_IF_IPAILNA
endif
ifdef CONFIG_RT_SECOND_IF_INTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_SECOND_IF_IPAILNA
endif
ifdef CONFIG_RT_THIRD_IF_INTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_THIRD_IF_IPAILNA
endif

ifdef CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_FIRST_IF_IPAELNA
endif
ifdef CONFIG_RT_SECOND_IF_INTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_SECOND_IF_IPAELNA
endif
ifdef CONFIG_RT_THIRD_IF_INTERNAL_PA_EXTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_THIRD_IF_IPAELNA
endif

ifdef CONFIG_RT_FIRST_IF_EXTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_FIRST_IF_EPAELNA
endif
ifdef CONFIG_RT_SECOND_IF_EXTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_SECOND_IF_EPAELNA
endif
ifdef CONFIG_RT_THIRD_IF_EXTERNAL_PA_INTERNAL_LNA
EXTRA_CFLAGS +=	-DCONFIG_THIRD_IF_EPAELNA
endif


########################################################
# Common files
#		$(SRC_EMBEDDED_DIR)/common/cmm_mumimo.o\
########################################################
cmm_objs := $(SRC_EMBEDDED_DIR)/common/action.o\
		$(SRC_EMBEDDED_DIR)/common/ba_action.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev/radio_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev/omac_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev/wmm_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev/wtbl_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hdev/hdev_basic.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hw_ctrl.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hw_ctrl_cmd.o\
		$(SRC_EMBEDDED_DIR)/hw_ctrl/hw_ctrl_basic.o\
		$(SRC_DIR)/hw_ctrl/hw_init.o\
		$(SRC_EMBEDDED_DIR)/common/bcn.o\
		$(SRC_EMBEDDED_DIR)/security/cmm_aes.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_cfg.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_cmd.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_data.o\
		$(SRC_EMBEDDED_DIR)/common/qm.o\
		$(SRC_EMBEDDED_DIR)/common/fp_qm.o\
		$(SRC_EMBEDDED_DIR)/common/fq_qm.o\
		$(SRC_EMBEDDED_DIR)/common/fp_fair_qm.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_info.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_profile.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_radar.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_sync.o\
		$(SRC_EMBEDDED_DIR)/common/wifi_sys_info.o\
		$(SRC_EMBEDDED_DIR)/common/wifi_sys_notify.o\
		$(SRC_EMBEDDED_DIR)/wlan_config/config_basic.o\
		$(SRC_EMBEDDED_DIR)/wlan_config/config_phy.o\
		$(SRC_EMBEDDED_DIR)/wlan_config/config_ht.o\
		$(SRC_EMBEDDED_DIR)/wlan_config/config_vht.o\
		$(SRC_EMBEDDED_DIR)/mgmt/be_basic.o\
		$(SRC_EMBEDDED_DIR)/mgmt/be_phy.o\
		$(SRC_EMBEDDED_DIR)/mgmt/be_ht.o\
		$(SRC_EMBEDDED_DIR)/mgmt/be_vht.o\
		$(SRC_EMBEDDED_DIR)/mgmt/bss_ops.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_sanity.o\
		$(SRC_EMBEDDED_DIR)/security/cmm_sec.o\
		$(SRC_EMBEDDED_DIR)/security/cmm_tkip.o\
		$(SRC_EMBEDDED_DIR)/security/cmm_wep.o\
		$(SRC_EMBEDDED_DIR)/security/cmm_wpa.o\
		$(SRC_EMBEDDED_DIR)/security/crypt_aes.o\
		$(SRC_EMBEDDED_DIR)/security/crypt_arc4.o\
		$(SRC_EMBEDDED_DIR)/security/crypt_hmac.o\
		$(SRC_EMBEDDED_DIR)/security/crypt_md5.o\
		$(SRC_EMBEDDED_DIR)/security/crypt_sha2.o\
		$(SRC_EMBEDDED_DIR)/common/eeprom.o\
		$(SRC_EMBEDDED_DIR)/common/mlme.o\
		$(SRC_DIR)/hw_ctrl/cmm_asic.o\
		$(SRC_DIR)/hw_ctrl/cmm_chip.o \
		$(SRC_EMBEDDED_DIR)/mgmt/mgmt_ht.o\
		$(SRC_EMBEDDED_DIR)/mgmt/mgmt_dev.o\
		$(SRC_EMBEDDED_DIR)/common/rtmp_init.o\
		$(SRC_EMBEDDED_DIR)/common/rtmp_init_inf.o\
		$(SRC_EMBEDDED_DIR)/common/spectrum.o\
		$(SRC_EMBEDDED_DIR)/common/rtmp_timer.o\
		$(SRC_EMBEDDED_DIR)/common/rt_channel.o\
		$(SRC_EMBEDDED_DIR)/common/op_class.o\
		$(SRC_EMBEDDED_DIR)/common/rt_os_util.o\
		$(SRC_EMBEDDED_DIR)/common/scan.o\
		$(SRC_EMBEDDED_DIR)/common/sys_log.o\
		$(SRC_EMBEDDED_DIR)/common/txpower.o\
		$(SRC_EMBEDDED_DIR)/common/vendor.o\
		$(SRC_EMBEDDED_DIR)/common/wdev.o\
		$(SRC_DIR)/chips/rtmp_chip.o\
		$(SRC_EMBEDDED_DIR)/mgmt/mgmt_hw.o\
		$(SRC_EMBEDDED_DIR)/mgmt/mgmt_entrytb.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_info_element.o\
		$(SRC_DIR)/os/linux/rt_profile.o\
		$(SRC_DIR)/os/linux/tm.o\
		$(SRC_DIR)/txpwr/single_sku.o\
		$(SRC_EMBEDDED_DIR)/common/ra_ac_q_mgmt.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_rvr_dbg.o\
		$(SRC_DIR)/protocol/protection.o

########################################################
# Rate adaptation related files
########################################################
rate_objs := 	$(SRC_DIR)/rate_ctrl/ra_ctrl_mt.o\
		$(SRC_DIR)/rate_ctrl/ra_ctrl_mt_drv.o\
		$(SRC_DIR)/rate_ctrl/ra_wrapper_embedded.o\
		$(SRC_DIR)/rate_ctrl/ra_cfg.o

########################################################
# ASIC related files
########################################################
asic_objs := $(SRC_DIR)/phy/phy.o

########################################################
# Spec feature related files
########################################################
spec_objs += $(SRC_EMBEDDED_DIR)/common/ps.o

EXTRA_CFLAGS += -DUAPSD_SUPPORT 

spec_objs += $(SRC_EMBEDDED_DIR)/common/uapsd.o
spec_objs += $(SRC_EMBEDDED_DIR)/common/mt_ps.o
spec_objs += $(SRC_EMBEDDED_DIR)/common/mt_io.o
spec_objs += $(SRC_EMBEDDED_DIR)/common/txs.o

# VHT
EXTRA_CFLAGS += -DDOT11_VHT_AC
spec_objs += $(SRC_EMBEDDED_DIR)/mgmt/mgmt_vht.o\
	     $(SRC_EMBEDDED_DIR)/common/vht.o

#PMF
ifeq ($(CONFIG_MT7615_AP_80211W_PMF),y)
    EXTRA_CFLAGS += -DDOT11W_PMF_SUPPORT
    spec_objs += $(SRC_EMBEDDED_DIR)/security/pmf.o
endif

# LLTD
ifeq ($(CONFIG_MT7615_AP_LLTD),y)
    EXTRA_CFLAGS += -DLLTD_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT),y)
    # TPC
    EXTRA_CFLAGS += -DTPC_SUPPORT
    #RRM
    EXTRA_CFLAGS += -DDOT11K_RRM_SUPPORT -DAP_SCAN_SUPPORT -DSCAN_SUPPORT -DAPPLE_11K_IOT
    spec_objs += $(SRC_EMBEDDED_DIR)/common/rrm_tlv.o \
				 $(SRC_EMBEDDED_DIR)/common/rrm.o \
				 $(SRC_EMBEDDED_DIR)/common/rrm_sanity.o
endif

# FTM
ifeq ($(CONFIG_FTM_SUPPORT),y)
    EXTRA_CFLAGS += -DFTM_SUPPORT -DFTM_INITIATOR
    EXTRA_CFLAGS += -DTMR_SUPPORT
    EXTRA_CFLAGS += -DDOT11K_RRM_SUPPORT

    spec_objs += $(SRC_EMBEDDED_DIR)/common/ftm.o
    spec_objs += $(SRC_DIR)/protocol/tmr.o
    spec_objs += $(SRC_EMBEDDED_DIR)/common/rrm_tlv.o \
				 $(SRC_EMBEDDED_DIR)/common/rrm.o \
				 $(SRC_EMBEDDED_DIR)/common/rrm_sanity.o
endif

ifeq ($(CONFIG_MT7615_AP_PASSPOINT_R2),y)
    EXTRA_CFLAGS += -DCONFIG_DOT11U_INTERWORKING -DCONFIG_DOT11V_WNM -DCONFIG_HOTSPOT -DCONFIG_HOTSPOT_R2
    # FTM
    EXTRA_CFLAGS += -DTMR_SUPPORT
    spec_objs += $(SRC_EMBEDDED_DIR)/common/wnm.o\
		$(SRC_EMBEDDED_DIR)/common/gas.o\
		$(SRC_EMBEDDED_DIR)/common/hotspot.o
endif

ifeq ($(CONFIG_MT7615_AP_DSCP_QOS_MAP_SUPPORT),y)
	EXTRA_CFLAGS += -DDSCP_QOS_MAP_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_WNM_SUPPORT),y)
        EXTRA_CFLAGS += -DCONFIG_DOT11V_WNM

        spec_objs += $(SRC_EMBEDDED_DIR)/common/wnm.o
endif


#ifeq ($(CONFIG_MT7615_AP_TRACE_IPARP),y)
#	EXTRA_CFLAGS += -DTRACELOG_IPARP
#	spec_objs +=  $(SRC_DIR)/debug/tracelog_iparp.o
#endif

#ifeq ($(CONFIG_MT7615_AP_TRACE_TCP_PKT),y)
#       EXTRA_CFLAGS += -DTRACELOG_TCP_PKT
#endif

#ifeq ($(CONFIG_MT7615_AP_BACKGROUND_SCAN_SUPPORT),y)
    EXTRA_CFLAGS += -DBACKGROUND_SCAN_SUPPORT
    spec_objs += $(SRC_EMBEDDED_DIR)/common/bgnd_scan.o
#endif

ifeq ($(CONFIG_MT7615_AP_SMART_CARRIER_SENSE),y)
    EXTRA_CFLAGS += -DSMART_CARRIER_SENSE_SUPPORT
     spec_objs += $(SRC_EMBEDDED_DIR)/common/scs.o
endif

ifeq ($(CONFIG_MT7615_AP_DFS),y)
    EXTRA_CFLAGS += -DMT_DFS_SUPPORT
     spec_objs += $(SRC_EMBEDDED_DIR)/common/cmm_rdm_mt.o
endif

# FT
ifeq ($(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT),y)
EXTRA_CFLAGS += -DDOT11R_FT_SUPPORT
dot11_ft_objs += $(SRC_EMBEDDED_DIR)/common/ft.o\
                    $(SRC_EMBEDDED_DIR)/common/ft_tlv.o\
                    $(SRC_EMBEDDED_DIR)/common/ft_iocl.o\
                    $(SRC_EMBEDDED_DIR)/common/ft_rc.o\
                    $(SRC_EMBEDDED_DIR)/ap/ap_ftkd.o
endif

#SNIFFER
ifeq ($(CONFIG_MT7615_AP_SNIFFER_SUPPORT),y)
    EXTRA_CFLAGS += -DCONFIG_SNIFFER_SUPPORT

    spec_objs += $(SRC_EMBEDDED_DIR)/sniffer/sniffer_radiotap.o\
                 $(SRC_EMBEDDED_DIR)/sniffer/sniffer_prism.o
endif

#
# Common Feature related files
#
func_objs :=

ifeq ($(CONFIG_MT7615_AP_IGMP_SNOOP),y)
    EXTRA_CFLAGS += -DIGMP_SNOOP_SUPPORT
    func_objs += $(SRC_EMBEDDED_DIR)/common/igmp_snoop.o
endif

ifeq ($(CONFIG_MT7615_AP_BLOCK_NET_IF),y)
    EXTRA_CFLAGS += -DBLOCK_NET_IF

    func_objs += $(SRC_EMBEDDED_DIR)/common/netif_block.o
endif

ifeq ($(CONFIG_MT7615_AP_SINGLE_SKU),y)
    EXTRA_CFLAGS += -DSINGLE_SKU
    ifeq ($(CONFIG_RALINK_RT6352),y)
        EXTRA_CFLAGS += -DSINGLE_SKU_V2
    endif
endif

ifeq ($(CONFIG_MT7615_AP_VIDEO_TURBINE),y)
    EXTRA_CFLAGS += -DVIDEO_TURBINE_SUPPORT

    func_objs += $(SRC_EMBEDDED_DIR)/common/cmm_video.o
endif



########################################################
# AP feature related files
########################################################
ap_objs := $(SRC_EMBEDDED_DIR)/ap/ap.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_assoc.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_auth.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_mlme.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_sanity.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_sync.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_wpa.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_sec.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_data.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_autoChSel.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_qload.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_cfg.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_nps.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_vow.o\
            $(SRC_DIR)/os/linux/ap_ioctl.o            

ifeq ($(CONFIG_MT7615_AP_MBSS),y)
    EXTRA_CFLAGS += -DMBSS_SUPPORT

    ifeq ($(CONFIG_MT7615_AP_NEW_MBSSID_MODE),y)
        EXTRA_CFLAGS += -DNEW_MBSSID_MODE
        ifeq ($(CONFIG_MT7615_AP_ENHANCE_NEW_MBSSID_MODE),y)
            EXTRA_CFLAGS += -DENHANCE_NEW_MBSSID_MODE
        endif
    endif

    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_mbss.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_mbss_inf.o
endif


ifeq ($(CONFIG_MT7615_AP_WDS),y)
    EXTRA_CFLAGS += -DWDS_SUPPORT

    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_wds.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_wds_inf.o\
            $(SRC_EMBEDDED_DIR)/common/client_wds.o
endif

ifeq ($(CONFIG_MT7615_AP_APCLI),y)
    EXTRA_CFLAGS += -DAPCLI_SUPPORT -DMAT_SUPPORT -DAPCLI_AUTO_CONNECT_SUPPORT  -DAPCLI_AUTO_BW_TMP
    EXTRA_CFLAGS += -DDHCP_UC_SUPPORT
    EXTRA_CFLAGS += -DFAST_EAPOL_WAR
    #EXTRA_CFLAGS += -DMULTI_APCLI_SUPPORT
    #EXTRA_CFLAGS += -DAPCLI_CERT_SUPPORT -DAPCLI_CONNECTION_TRIAL
    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_apcli.o\
            $(SRC_EMBEDDED_DIR)/ap/ap_apcli_inf.o\
            $(SRC_EMBEDDED_DIR)/ap/apcli_assoc.o\
            $(SRC_EMBEDDED_DIR)/ap/apcli_auth.o\
            $(SRC_EMBEDDED_DIR)/ap/apcli_ctrl.o\
            $(SRC_EMBEDDED_DIR)/ap/apcli_sync.o\
            $(SRC_EMBEDDED_DIR)/common/cmm_mat.o\
            $(SRC_EMBEDDED_DIR)/common/cmm_mat_iparp.o\
            $(SRC_EMBEDDED_DIR)/common/cmm_mat_pppoe.o\
            $(SRC_EMBEDDED_DIR)/common/cmm_mat_ipv6.o

    ifeq ($(CONFIG_MT7615_AP_APCLI_CERT),y)
        EXTRA_CFLAGS += -DAPCLI_CERT_SUPPORT
    endif

    ifeq ($(CONFIG_MT7615_AP_MAC_REPEATER_SUPPORT),y)
        EXTRA_CFLAGS += -DMAC_REPEATER_SUPPORT

        ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_repeater.o
    endif

	ifeq ($(CONFIG_MT7615_AP_RADIO_LINK_SELECTION),y)
	    EXTRA_CFLAGS += -DRADIO_LINK_SELECTION
	    ap_objs +=  $(SRC_EMBEDDED_DIR)/ap/ap_rls.o
	endif

endif

ifeq ($(CONFIG_MT7615_AP_IDS),y)
    EXTRA_CFLAGS += -DIDS_SUPPORT

    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_ids.o
endif

ifeq ($(CONFIG_MT7615_AP_ATE_SUPPORT),y)
    EXTRA_CFLAGS += -DCONFIG_ATE -DCONFIG_QA -DCONFIG_RT2880_ATE_CMD_NEW -DCONFIG_HW_HAL_OFFLOAD
    #EXTRA_CFLAGS += -DCSD_VERIFICATION
    EXTRA_CFLAGS += -DINTERNAL_CAPTURE_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_ICAP_SUPPORT),y)
	EXTRA_CFLAGS += -DINTERNAL_CAPTURE_SUPPORT
endif


EXTRA_CFLAGS += -DRTMP_FLASH_SUPPORT
EXTRA_CFLAGS += -DPRE_CAL_TRX_SET1_SUPPORT
EXTRA_CFLAGS += -DRLM_CAL_CACHE_SUPPORT
EXTRA_CFLAGS += -DPRE_CAL_TRX_SET2_SUPPORT
#EXTRA_CFLAGS += -DPA_TRIM_SUPPORT

ifeq ($(CONFIG_MT7615_AP_RF_LOCKDOWN_SUPPORT),y)
    EXTRA_CFLAGS += -DRF_LOCKDOWN
endif

ifeq ($(CONFIG_MT7615_AP_RTMP_TEMPERATURE_TX_ALC),y)
    EXTRA_CFLAGS += -DRTMP_TEMPERATURE_TX_ALC
endif

ifeq ($(CONFIG_MT7615_AP_TXBF_SUPPORT),y)
    EXTRA_CFLAGS += -DMCS_LUT_SUPPORT -DTXBF_SUPPORT -DVHT_TXBF_SUPPORT
    ap_objs += $(SRC_DIR)/txbf/cmm_txbf_mt.o\
                    $(SRC_DIR)/txbf/cmm_txbf_cal_mt.o\
                    $(SRC_DIR)/txbf/txbf_wrapper_embedded.o
endif

ifeq ($(CONFIG_MT7615_AP_MUMIMO_SUPPORT),y)
   EXTRA_CFLAGS += -DCFG_SUPPORT_MU_MIMO
    ap_objs += $(SRC_EMBEDDED_DIR)/common/cmm_mumimo.o
    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_mumimo.o
endif

ifeq ($(CONFIG_MT7615_AP_MU_RA_SUPPORT),y)
    EXTRA_CFLAGS += -DCFG_SUPPORT_MU_MIMO_RA
    ap_objs += $(SRC_EMBEDDED_DIR)/ap/ap_mura.o
endif

ifeq ($(CONFIG_MT7615_AP_CARRIER),y)
    EXTRA_CFLAGS += -DCARRIER_DETECTION_SUPPORT

    ap_objs += $(SRC_EMBEDDED_DIR)/common/cmm_cs.o
endif

ifeq ($(CONFIG_MT7615_AP_MCAST_RATE_SPECIFIC),y)
    EXTRA_CFLAGS += -DMCAST_RATE_SPECIFIC
endif

ifeq ($(CONFIG_MT7615_AP_AIRPLAY_SUPPORT),y)
    EXTRA_CFLAGS += -DAIRPLAY_SUPPORT
endif

ifeq ($(CONFIG_BAND_STEERING),y)
    EXTRA_CFLAGS += -DBAND_STEERING
    ap_objs +=  $(SRC_EMBEDDED_DIR)/ap/ap_band_steering.o
endif

ifeq ($(CONFIG_MT7615_AP_AIR_MONITOR),y)
    EXTRA_CFLAGS += -DAIR_MONITOR
endif

ifeq ($(CONFIG_MT7615_AP_MWDS),y)
    ap_objs += $(SRC_EMBEDDED_DIR)/common/mwds.o
    ap_objs += $(SRC_EMBEDDED_DIR)/common/routing_tab.o
endif

ifeq ($(CONFIG_MT7615_AP_EVENT_NOTIFIER_SUPPORT),y)
    ap_objs += $(SRC_EMBEDDED_DIR)/common/event_notifier.o
endif

ifeq ($(CONFIG_MT7615_AP_VOW_SUPPORT),y)
    EXTRA_CFLAGS += -DVOW_SUPPORT
endif

EXTRA_CFLAGS += -DRED_SUPPORT


########################################################
# Linux system related files
########################################################
os_objs := $(SRC_DIR)/os/linux/rt_proc.o\
            $(SRC_DIR)/os/linux/rt_linux.o\
            $(SRC_DIR)/os/linux/rt_profile.o\
            $(SRC_DIR)/os/linux/rt_txrx_hook.o\
            $(SRC_DIR)/os/linux/rt_main_dev.o

EXTRA_CFLAGS += -DMULTI_INF_SUPPORT
os_objs += $(SRC_DIR)/os/linux/multi_main_dev.o

ifeq ($(CONFIG_MT7615_AP_LINUX_NET_TXQ_SUPPORT),y)
EXTRA_CFLAGS += -DLINUX_NET_TXQ_SUPPORT
endif

########################################################
# chip related files
########################################################

ifeq ($(CHIP_MT7615E),y)
EXTRA_CFLAGS += -DMT7615 -DPRODUCT_VERSION=7615
EXTRA_CFLAGS += -DMT_BBP -DMT_RF -DRTMP_RF_RW_SUPPORT -DMT_MAC -DRTMP_MAC_PCI -DRTMP_PCI_SUPPORT
EXTRA_CFLAGS += -DMCS_LUT_SUPPORT -DA_BAND_SUPPORT
EXTRA_CFLAGS += -DRTMP_EFUSE_SUPPORT
EXTRA_CFLAGS += -DCAL_FREE_IC_SUPPORT
EXTRA_CFLAGS += -DBCN_OFFLOAD_SUPPORT
EXTRA_CFLAGS += -DCONFIG_FWOWN_SUPPORT -DERR_RECOVERY
#EXTRA_CFLAGS += -DNEW_SET_RX_STREAM
EXTRA_CFLAGS += -DCONFIG_ANDES_SUPPORT  -DBA_TRIGGER_OFFLOAD
EXTRA_CFLAGS += -DHDR_TRANS_TX_SUPPORT -DHDR_TRANS_RX_SUPPORT
EXTRA_CFLAGS +=	-DCUT_THROUGH
EXTRA_CFLAGS += -DUNIFY_FW_CMD
#EXTRA_CFLAGS += -DTMR_VERIFY
#EXTRA_CFLAGS += -DDBDC_MODE
EXTRA_CFLAGS += -DNEED_ROM_PATCH
EXTRA_CFLAGS += -DMAC_INIT_OFFLOAD
EXTRA_CFLAGS += -DENHANCED_STAT_DISPLAY
EXTRA_CFLAGS += -DNEWSEC
#EXTRA_CFLAGS += -DSINGLE_SKU_V2
EXTRA_CFLAGS += -DTHERMAL_PROTECT_SUPPORT
EXTRA_CFLAGS += -DRESOURCE_PRE_ALLOC
EXTRA_CFLAGS += -DCONFIG_HW_HAL_OFFLOAD
EXTRA_CFLAGS += -DRX_SCATTER
EXTRA_CFLAGS += -DCONFIG_BA_REORDER_MONITOR
EXTRA_CFLAGS += -DPKT_BUDGET_CTRL_SUPPORT
EXTRA_CFLAGS += -DTX_AGG_ADJUST_WKR
EXTRA_CFLAGS += -DPRE_CAL_TRX_SET1_SUPPORT
EXTRA_CFLAGS += -DRATE_ADAPTION
EXTRA_CFLAGS += -DRATE_ADAPT_AGBS_SUPPORT
EXTRA_CFLAGS += -DRACTRL_FW_OFFLOAD_SUPPORT
EXTRA_CFLAGS += -DG_BAND_256QAM
#EXTRA_CFLAGS += -DRTMP_WLAN_HOOK_SUPPORT

ifeq ($(CONFIG_MT7615_AP_LED_CONTROL_SUPPORT),y)
    EXTRA_CFLAGS += -DLED_CONTROL_SUPPORT
    func_objs += $(SRC_EMBEDDED_DIR)/common/rt_led.o
endif

ifeq ($(CONFIG_MT7615_AP_GPIO_CONTROL_SUPPORT),y)
    EXTRA_CFLAGS += -DGPIO_CONTROL_SUPPORT
    func_objs += $(SRC_EMBEDDED_DIR)/hw_ctrl/mt_gpio.o
endif

ifeq ($(CONFIG_MT7615_AP_RADIUS_ACCOUNTING_SUPPORT),y)
	EXTRA_CFLAGS += -DRADIUS_ACCOUNTING_SUPPORT
endif

#ifeq ($(CONFIG_FDB_SUPPORT),y)
#    EXTRA_CFLAGS += -DMT_FDB
#    chip_objs += $(SRC_DIR)/mcu/mt_fdb.o
#endif

chip_objs += $(SRC_DIR)/chips/mt7615.o\
		$(SRC_DIR)/chips/mt7615_dbg.o\
		$(SRC_DIR)/hw_ctrl/cmm_asic_mt_dmac.o\
		$(SRC_DIR)/hw_ctrl/cmm_asic_mt_fw.o\
		$(SRC_DIR)/hw_ctrl/cmm_chip_mt.o\
		$(SRC_DIR)/mac/mt_dmac.o\
		$(SRC_EMBEDDED_DIR)/mcu/mcu.o\
		$(SRC_EMBEDDED_DIR)/mcu/andes_core.o\
		$(SRC_EMBEDDED_DIR)/mcu/andes_mt.o \
		$(SRC_DIR)/mcu/mt_cmd.o\
		$(SRC_DIR)/mcu/fwdl_mt.o\
		$(SRC_DIR)/mcu/fw_cmd.o\
		$(SRC_DIR)/mcu/fwdl.o\
		$(SRC_DIR)/phy/mt_rf.o\
		$(SRC_DIR)/phy/rf.o\
		$(SRC_DIR)/phy/mt_phy.o\
		$(SRC_DIR)/phy/rlm_cal_cache.o\
		$(SRC_EMBEDDED_DIR)/common/cmm_mac_pci.o\
		$(SRC_EMBEDDED_DIR)/common/ee_prom.o\
		$(SRC_EMBEDDED_DIR)/common/ee_efuse.o\
		$(SRC_EMBEDDED_DIR)/common/cut_through.o\
		$(SRC_DIR)/protocol/tmr.o\
		$(SRC_DIR)/os/linux/rt_rbus_pci_drv.o\
		$(SRC_EMBEDDED_DIR)/common/hif_pci.o\
		$(SRC_DIR)/os/linux/pci_main_dev.o
endif

#
# Root
#

$(DRV_NAME)-objs += $(ap_objs) $(cmm_objs) $(asic_objs) $(chip_objs) $(rate_objs)\
                    $(spec_objs) $(func_objs) $(os_objs) $(dot11_ft_objs)

$(DRV_NAME)-objs += $(SRC_EMBEDDED_DIR)/common/eeprom.o\
					$(SRC_EMBEDDED_DIR)/common/ee_flash.o\
					$(SRC_EMBEDDED_DIR)/common/ee_efuse.o

$(DRV_NAME)-objs += $(SRC_DIR)/os/linux/rt_pci_rbus.o\
                    $(SRC_DIR)/os/linux/rt_rbus_pci_drv.o\
                    $(SRC_DIR)/os/linux/rt_rbus_pci_util.o\
                    #$(SRC_DIR)/os/linux/rbus_main_dev.o

ifeq ($(CONFIG_MT7615_AP_ATE_SUPPORT),y)
$(DRV_NAME)-objs += $(SRC_DIR)/ate/ate_agent.o\
                    $(SRC_DIR)/ate/testmode_ioctl.o\
		    $(SRC_DIR)/ate/mt_mac/mt_testmode.o\
 		    $(SRC_DIR)/ate/LoopBack.o
endif


# EXTRA_CFLAGS += -DREDUCE_TCP_ACK_SUPPORT
#$(DRV_NAME)-objs += $(SRC_EMBEDDED_DIR)/common/cmm_tcprack.o

###################
#  CFLAGS
##################
EXTRA_CFLAGS += -Wall -Wno-unused-parameter -Wno-unused-variable

EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT -DLINUX -DCONFIG_WIFI_PAGE_ALLOC_SKB
EXTRA_CFLAGS += -DCONFIG_AP_SUPPORT -DSCAN_SUPPORT -DAP_SCAN_SUPPORT
EXTRA_CFLAGS += -DDOT11_N_SUPPORT -DSTATS_COUNT_SUPPORT -DIAPP_SUPPORT -DDOT1X_SUPPORT
EXTRA_CFLAGS += -DIP_ASSEMBLY
EXTRA_CFLAGS += -DOUI_CHECK_SUPPORT -DHTC_DECRYPT_IOT
EXTRA_CFLAGS += -DAUTO_CH_SELECT_ENHANCE

#EXTRA_CFLAGS += -DRALINK_ATE -DRALINK_QA -DCONFIG_RT2880_ATE_CMD_NEW

# hw_nat support
#EXTRA_CFLAGS += -DCONFIG_FAST_NAT_SUPPORT

#ifeq ($(CONFIG_RALINK_MT7621),y)
#EXTRA_CFLAGS += -DRACTRL_LIMIT_MAX_PHY_RATE=1300
#endif

#provide busy time statistics for every TBTT */
EXTRA_CFLAGS += -DAP_QLOAD_SUPPORT -DQLOAD_FUNC_BUSY_TIME_STATS

# provide busy time alarm mechanism
# use the function to avoid to locate in some noise environments
#EXTRA_CFLAGS += -DQLOAD_FUNC_BUSY_TIME_ALARM


ifeq ($(CONFIG_MT7615_AP_DEBUG),y)
#-DDBG_DIAGNOSE -DDBG_RX_MCS -DDBG_TX_MCS
#-DUAPSD_DEBUG
EXTRA_CFLAGS += -DDBG
endif

ifeq ($(CONFIG_MT7615_AP_SNMP),y)
EXTRA_CFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_HW_ANTENNA_DIVERSITY),y)
EXTRA_CFLAGS += -DHW_ANTENNA_DIVERSITY_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_EXT_CHANNEL_LIST),y)
EXTRA_CFLAGS += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(CONFIG_MT7615_AP_RTMP_INTERNAL_TX_ALC),y)
EXTRA_CFLAGS += -DRTMP_INTERNAL_TX_ALC
endif

ifeq ($(CONFIG_MT7615_AP_ROAMING_ENHANCE_SUPPORT),y)
    EXTRA_CFLAGS += -DROAMING_ENHANCE_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_MWDS),y)
    EXTRA_CFLAGS += -DMWDS -DROUTING_TAB_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_EVENT_NOTIFIER_SUPPORT),y)
    EXTRA_CFLAGS += -DWH_EVENT_NOTIFIER
endif

ifeq ($(CONFIG_MT7615_AP_STA_FORCE_ROAM_SUPPORT),y)
EXTRA_CFLAGS += -DSTA_FORCE_ROAM_SUPPORT
endif

ifeq ($(CONFIG_MT7615_AP_80211N_DRAFT3),y)
EXTRA_CFLAGS += -DDOT11N_DRAFT3
endif
