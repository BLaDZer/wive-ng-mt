RT_DRV_DIR  := ralink/mt76x2
RT_DRV_PATH := ../mt76x2
RT_DRV_NAME := mt76x2_sta

CHIP_MT7620 =
CHIP_MT76X2 =
ifeq ($(CONFIG_RT_FIRST_CARD),7620)
CHIP_MT7620 = y
endif
ifeq ($(CONFIG_RT_FIRST_CARD),7602)
CHIP_MT76X2 = y
endif
ifeq ($(CONFIG_RT_FIRST_CARD),7612)
CHIP_MT76X2 = y
endif
ifeq ($(CONFIG_RT_SECOND_CARD),7602)
CHIP_MT76X2 = y
endif
ifeq ($(CONFIG_RT_SECOND_CARD),7612)
CHIP_MT76X2 = y
endif

obj-$(CONFIG_MT76X2_STA) += $(RT_DRV_NAME).o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/crypt_md5.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/crypt_sha2.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/crypt_hmac.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/crypt_aes.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/crypt_arc4.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/mlme.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/action.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/ba_action.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_wep.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_wpa.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_tkip.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_aes.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_data.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_data_pci.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_mac_pci.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_sync.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_sanity.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_info.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_cfg.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_profile.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_asic.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_cmd.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_radar.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rtmp_init.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rtmp_init_inf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rtmp_timer.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rt_channel.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rt_os_util.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/spectrum.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/txpower.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/scan.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/ps.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/uapsd.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/eeprom.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/ee_flash.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/ee_efuse.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/ee_prom.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/tx_rx/wdev_tx.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mgmt/mgmt_entrytb.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mgmt/mgmt_ht.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mac/rtmp_mac.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mcu/mcu.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/rf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/rt_rf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/phy.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/chips/rtmp_chip.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/rate_ctrl/ra_ctrl.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/rate_ctrl/alg_legacy.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/rate_ctrl/alg_grp.o
#$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/rate_ctrl/alg_ags.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/assoc.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/auth.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/auth_rsp.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/sync.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/sanity.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/rtmp_data.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/connect.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/wpa.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/sta_cfg.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/sta_ioctl.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/sta/sta.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_proc.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_linux.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_profile.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_main_dev.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_rbus_pci_drv.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_rbus_pci_util.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rt_pci_rbus.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/multi_main_dev.o

### MT7620 (RBUS)
ifeq ($(CHIP_MT7620),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/chips/rt6352.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/chips/rtxx_dft.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rtmp_swmcu.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_rf_cal.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_loft_cal.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/rtmp_phy.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/rlt_rf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mac/ral_omac.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/rbus_main_dev.o
endif

### MT7602E/MT7612E (PCIe)
ifeq ($(CHIP_MT76X2),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/chips/rt65xx.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/chips/mt76x2.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mcu/mcu_and.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/mt_rf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/phy/rlt_phy.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mac/ral_nmac.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/os/linux/pci_main_dev.o
endif

ifeq ($(CONFIG_MT76X2_AP_LED),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/rt_led.o
endif

ifeq ($(CONFIG_MT76X2_AP_80211AC_VHT),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/vht.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/mgmt/mgmt_vht.o
endif

ifeq ($(CONFIG_MT76X2_AP_NETIF_BLOCK),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/netif_block.o
endif

ifeq ($(CONFIG_MT76X2_AP_CARRIER),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_cs.o
endif

ifeq ($(CONFIG_MT76X2_AP_TXBF_SUPPORT),y)
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_txbf.o
$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/cmm_txbf_cal.o
endif

$(RT_DRV_NAME)-objs += $(RT_DRV_PATH)/common/sys_log.o

###################
#  CFLAGS
##################
EXTRA_CFLAGS := -Idrivers/net/wireless/$(RT_DRV_DIR)/include
EXTRA_CFLAGS += -Wall -Wno-unused-parameter -Wno-sign-compare -Wno-type-limits
EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT -DLINUX
EXTRA_CFLAGS += -DCONFIG_STA_SUPPORT -DUAPSD_SUPPORT -DDOT11_N_SUPPORT
EXTRA_CFLAGS += -DSTATS_COUNT_SUPPORT -DENHANCED_STAT_DISPLAY
EXTRA_CFLAGS += -DRESOURCE_PRE_ALLOC -DSCAN_SUPPORT

# do not lock CountryRegion from EEPROM
EXTRA_CFLAGS += -DEEPROM_COUNTRY_UNLOCK

# provide external Flash memory support
EXTRA_CFLAGS += -DRTMP_FLASH_SUPPORT

# provide internal eFuse memory support
#EXTRA_CFLAGS += -DRTMP_EFUSE_SUPPORT -DCAL_FREE_IC_SUPPORT

# provide information on the current STA population and traffic levels in the QBSS
#EXTRA_CFLAGS += -DAP_QLOAD_SUPPORT -DQLOAD_FUNC_BUSY_TIME_STATS

ifneq ($(CONFIG_RA_HW_NAT_WIFI),y)
EXTRA_CFLAGS += -DCONFIG_RA_NAT_NONE
endif

ifeq ($(CONFIG_RA_NETWORK_WORKQUEUE_BH),y)
EXTRA_CFLAGS += -DWORKQUEUE_BH
endif

### MT7620 (RBUS)
ifeq ($(CHIP_MT7620),y)
EXTRA_CFLAGS += -DRT6352 -DCONFIG_SWMCU_SUPPORT
EXTRA_CFLAGS += -DRTMP_RBUS_SUPPORT -DRTMP_MAC_PCI
EXTRA_CFLAGS += -DRTMP_MAC -DRTMP_BBP -DRLT_RF -DRTMP_RF_RW_SUPPORT
EXTRA_CFLAGS += -DVCORECAL_SUPPORT
EXTRA_CFLAGS += -DRTMP_TEMPERATURE_CALIBRATION
EXTRA_CFLAGS += -DFIFO_EXT_SUPPORT
EXTRA_CFLAGS += -DMCS_LUT_SUPPORT -DPEER_DELBA_TX_ADAPT
#EXTRA_CFLAGS += -DSPECIFIC_BCN_BUF_SUPPORT
#EXTRA_CFLAGS += -DTHERMAL_PROTECT_SUPPORT
#EXTRA_CFLAGS += -DADJUST_POWER_CONSUMPTION_SUPPORT
ifeq ($(CONFIG_MT76X2_AP_RTMP_INTERNAL_TX_ALC),y)
EXTRA_CFLAGS += -DRTMP_INTERNAL_TX_ALC
endif
ifeq ($(CONFIG_MT76X2_AP_RTMP_TEMPERATURE_COMPENSATION),y)
EXTRA_CFLAGS += -DRTMP_TEMPERATURE_COMPENSATION
endif
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
EXTRA_CFLAGS += -DRT6352_EL_SUPPORT
endif
ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
EXTRA_CFLAGS += -DRT6352_EP_SUPPORT -DRT6352_EL_SUPPORT
endif
endif

### MT7602E/MT7612E (PCIe)
ifeq ($(CHIP_MT76X2),y)
EXTRA_CFLAGS += -DRT65xx -DMT76x2 -DMT7612
#EXTRA_CFLAGS += -DCONFIG_FPGA_MODE
EXTRA_CFLAGS += -DRTMP_PCI_SUPPORT -DRTMP_MAC_PCI
EXTRA_CFLAGS += -DRLT_MAC -DRLT_BBP -DMT_RF -DRTMP_RF_RW_SUPPORT
EXTRA_CFLAGS += -DCONFIG_ANDES_SUPPORT
EXTRA_CFLAGS += -DA_BAND_SUPPORT
EXTRA_CFLAGS += -DNEW_RATE_ADAPT_SUPPORT
#EXTRA_CFLAGS += -DNEW_RATE_ADAPT_QUICK_DOWN # has dropdown to CCK/0 issue on 5GHz
EXTRA_CFLAGS += -DFIFO_EXT_SUPPORT
EXTRA_CFLAGS += -DMCS_LUT_SUPPORT -DPEER_DELBA_TX_ADAPT
EXTRA_CFLAGS += -DRTMP_TEMPERATURE_TX_ALC
#EXTRA_CFLAGS += -DSPECIFIC_BCN_BUF_SUPPORT
#EXTRA_CFLAGS += -DSPECIFIC_TX_POWER_SUPPORT
#EXTRA_CFLAGS += -DDMA_BUSY_RESET
ifeq ($(CHIP_MT7620),y)
EXTRA_CFLAGS += -DMULTI_INF_SUPPORT
endif
endif

ifeq ($(CONFIG_MT76X2_AP_READ_MAC_FROM_MTD),y)
EXTRA_CFLAGS += -DREAD_MAC_FROM_EEPROM
endif

ifeq ($(CONFIG_MT76X2_AP_DBG),y)
EXTRA_CFLAGS += -DDBG
#EXTRA_CFLAGS += -DDBG_DIAGNOSE -DDBG_RX_MCS -DDBG_TX_MCS
EXTRA_CFLAGS += -DSYSTEM_LOG_SUPPORT
endif

ifeq ($(CONFIG_MT76X2_AP_80211N_DRAFT3),y)
EXTRA_CFLAGS += -DDOT11N_DRAFT3
endif

ifeq ($(CONFIG_MT76X2_AP_80211AC_VHT),y)
EXTRA_CFLAGS += -DDOT11_VHT_AC
endif

ifeq ($(CONFIG_MT76X2_AP_TXBF_SUPPORT),y)
EXTRA_CFLAGS += -DMCS_LUT_SUPPORT -DTXBF_SUPPORT -DVHT_TXBF_SUPPORT
endif

ifeq ($(CONFIG_MT76X2_AP_LED),y)
EXTRA_CFLAGS += -DLED_CONTROL_SUPPORT
endif

ifeq ($(CONFIG_MT76X2_AP_NETIF_BLOCK),y)
EXTRA_CFLAGS += -DBLOCK_NET_IF
endif

ifeq ($(CONFIG_MT76X2_AP_HDR_TRANS),y)
EXTRA_CFLAGS += -DHDR_TRANS_SUPPORT
endif

#ifeq ($(CONFIG_MT76X2_AP_DELAYED_TCP_ACK),y)
#EXTRA_CFLAGS += -DDELAYED_TCP_ACK
#endif

ifeq ($(CONFIG_MT76X2_AP_EXT_CHANNEL_LIST),y)
EXTRA_CFLAGS += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(CONFIG_MT76X2_AP_SINGLE_SKU),y)
EXTRA_CFLAGS += -DSINGLE_SKU_V2
endif

ifeq ($(CONFIG_MT76X2_AP_MEMORY_OPTIMIZATION),y)
EXTRA_CFLAGS += -DMEMORY_OPTIMIZATION
endif

ifeq ($(CONFIG_MT76X2_AP_SNMP),y)
EXTRA_CFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(CONFIG_MT76X2_AP_BIG_ENDIAN),y)
EXTRA_CFLAGS += -DRT_BIG_ENDIAN
endif

ifeq ($(CONFIG_MT76X2_AP_MC_SUPPORT),y)
EXTRA_CFLAGS += -DMULTIPLE_CARD_SUPPORT
ifeq ($(CONFIG_RT_FIRST_CARD),7620)
EXTRA_CFLAGS += -DRT_SOC_SUPPORT
endif
endif
