#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>

#include "ra_eth_reg.h"
#include "mii_mgr.h"
#include "ra_phy.h"

/* EPHY Vendor ID list */
#define EV_ICPLUS_PHY_ID0		0x0243
#define EV_ICPLUS_PHY_ID1		0x0D90

#define EV_REALTEK_PHY_ID0		0x001C
#define EV_REALTEK_PHY_ID1		0xC910

#define EV_MARVELL_PHY_ID0		0x0141
#define EV_MARVELL_PHY_ID1		0x0CC2

#define EV_VTSS_PHY_ID0			0x0007
#define EV_VTSS_PHY_ID1			0x0421

#if defined (CONFIG_GE1_RGMII_AN) || defined (CONFIG_P5_MAC_TO_PHY_MODE) || \
    defined (CONFIG_GE2_RGMII_AN) || defined (CONFIG_P4_MAC_TO_PHY_MODE)
static u32 g_phy_id[32] = { 0 };
extern int ext_phy_mode;

void ext_gphy_modeset(u32 phy_addr)
{
	u32 phy_val = 0;

	/* 0 - AN or 10/100/1000 */
	if (ext_phy_mode) {
	    mii_mgr_read(phy_addr, 0, &phy_val);
	    phy_val &=  ~(1<<12);			/* Disable AN */
	    phy_val |=  (1<<8);				/* Enable FDX */
	    if (ext_phy_mode == 10) {
		phy_val &=  ~(1<<13);			/* Force 10   (speed[0] = 0) */
		phy_val &=  ~(1<<6);			/* Force 10   (speed[1] = 0) */
	    } else if (ext_phy_mode == 100) {
		phy_val |=  (1<<13);			/* Force 100  (speed[0] = 1) */
		phy_val &=  ~(1<<6);			/* Force 100  (speed[1] = 0) */
	    } else {
		phy_val &=  ~(1<<13);			/* Force 1000 (speed[0] = 0) */
		phy_val |=  (1<<6);			/* Force 1000 (speed[1] = 1) */
	    }
	    mii_mgr_write(phy_addr, 0, phy_val);

	    /* Config AN Advertise */
	    mii_mgr_read(phy_addr, 4, &phy_val);
	    phy_val |=  (1<<15);                    	/* Additional next pages exchange desired (need for 1000BASE-T) */
	    phy_val |=  (1<<10);                    	/* Advertise support of pause frames */
	    phy_val &=  ~(1<<8);			/* Advertise disable support of 100Base-TX full-duplex mode */
	    phy_val &=  ~(1<<7);			/* Advertise disable support of 100Base-TX half-duplex mode */
	    phy_val &=  ~(1<<6);			/* Advertise disable support of 10Base-TX full-duplex mode */
	    phy_val &=  ~(1<<5);			/* Advertise disable support of 10Base-TX half-duplex mode */
	    mii_mgr_write(phy_addr, 4, phy_val);
	} else {
	    /* Enable AN: bit speed[0:1],fdx must be 0, AN bit = 1*/
	    mii_mgr_read(phy_addr, 0, &phy_val);
	    phy_val &=  ~(1<<13);			/* speed[0] = 0 */
	    phy_val |=   (1<<12);			/* Enable AN */
	    phy_val &=  ~(1<<8);			/* FDX	= 0 */
	    phy_val &=  ~(1<<6);			/* speed[1] = 0 */
	    mii_mgr_write(phy_addr, 0, phy_val);

	    /* Config AN Advertise */
	    mii_mgr_read(phy_addr, 4, &phy_val);
	    phy_val |=  (1<<15);                    	/* Additional next pages exchange desired (need for 1000BASE-T) */
	    phy_val |=  (1<<10);                    	/* Advertise support of pause frames */
	    phy_val |=  (1<<8);				/* Advertise support of 100Base-TX full-duplex mode */
	    phy_val |=  (1<<7);				/* Advertise support of 100Base-TX half-duplex mode */
	    phy_val |=  (1<<6);				/* Advertise support of 10Base-TX full-duplex mode */
	    phy_val |=  (1<<5);				/* Advertise support of 10Base-TX half-duplex mode */
	    mii_mgr_write(phy_addr, 4, phy_val);

	    /*  Config AN Advertise (next pages) */
	    mii_mgr_read(phy_addr, 9, &phy_val);
	    phy_val |=  (1<<9);				/* Advertise 1000BASE-T full duplex capable */
	    mii_mgr_write(phy_addr, 9, phy_val);
	}

	/* AN restart */
	mii_mgr_read(phy_addr, 0, &phy_val);
	phy_val |=  (1<<9);
	mii_mgr_write(phy_addr, 0, phy_val);

}

void ext_gphy_init(u32 phy_addr)
{
	const char *phy_devn = NULL, *phy_mode = "AUTO";
	u32 phy_id0 = 0, phy_id1 = 0, phy_val = 0, phy_rev;

	if (phy_addr > 0x1f)
		return;

	if (!mii_mgr_read(phy_addr, 2, &phy_id0))
		return;
	if (!mii_mgr_read(phy_addr, 3, &phy_id1))
		return;

	g_phy_id[phy_addr] = ((u32)phy_id1 << 16) | phy_id0;

	phy_rev = phy_id1 & 0xf;

	if ((phy_id0 == EV_ICPLUS_PHY_ID0) && ((phy_id1 & 0xfff0) == EV_ICPLUS_PHY_ID1)) {
		phy_devn = "IC+ IP1001";
		mii_mgr_read(phy_addr, 4, &phy_val);
		phy_val |= (1<<10);			// enable pause ability
		mii_mgr_write(phy_addr, 4, phy_val);
		mii_mgr_read(phy_addr, 0, &phy_val);
		phy_val |= (1<<15);			// PHY Software Reset
		mii_mgr_write(phy_addr, 0, phy_val);
	} else
	if ((phy_id0 == EV_REALTEK_PHY_ID0) && ((phy_id1 & 0xfff0) == EV_REALTEK_PHY_ID1)) {
		phy_devn = "RTL8211";
		if (phy_rev == 0x6) {
			u32 reg31 = 0;

			phy_devn = "RTL8211F";

			/* backup reg 0x1f */
			mii_mgr_read(phy_addr, 31, &reg31);
			if (reg31 != 0x0000 && reg31 != 0x0a42) {
				reg31 = 0x0000;
				mii_mgr_write(phy_addr, 31, reg31);
			}

			/* Disable response on MDIO addr 0 (!) */
			mii_mgr_read(phy_addr, 24, &phy_val);
			phy_val &= ~(1<<13);		// PHYAD_0 Disable
			mii_mgr_write(phy_addr, 24, phy_val);

			/* Disable Green Ethernet */
			mii_mgr_write(phy_addr, 27, 0x8011);
			mii_mgr_write(phy_addr, 28, 0x573f);

			/* Enable flow control by default */
			mii_mgr_read(phy_addr, 4, &phy_val);
			phy_val |=  (1<<10);		// Enable pause ability
			mii_mgr_write(phy_addr, 4, phy_val);

			/* Setup LED */
			mii_mgr_write(phy_addr, 31, 0x0d04);
			mii_mgr_read(phy_addr, 17, &phy_val);
			phy_val &= ~(1<<3);		// LED2 EEE Disable
			phy_val &= ~(1<<2);		// LED1 EEE Disable
			phy_val &= ~(1<<1);		// LED0 EEE Disable
			mii_mgr_write(phy_addr, 17, phy_val);

			/* restore reg 0x1f */
			mii_mgr_write(phy_addr, 31, reg31);

		} else if (phy_rev == 0x5) {
			phy_devn = "RTL8211E";

			/* Disable Green Ethernet */
			mii_mgr_write(phy_addr, 31, 0x0003);
			mii_mgr_write(phy_addr, 25, 0x3246);
			mii_mgr_write(phy_addr, 16, 0xa87c);
			mii_mgr_write(phy_addr, 31, 0x0000);

		} else if (phy_rev == 0x4) {
			phy_devn = "RTL8211D";  /* Fiber/UTP to RGMII */

			/* Disable Green Ethernet */
			mii_mgr_write(phy_addr, 31, 0x0003);	/* set to page 3 */
			mii_mgr_write(phy_addr, 25, 0x3246);
			mii_mgr_write(phy_addr, 16, 0xa87c);
			mii_mgr_write(phy_addr, 31, 0x0000);	/* back to page 0 */
		}
	} else
	if ((phy_id0 == EV_MARVELL_PHY_ID0) && (phy_id1 == EV_MARVELL_PHY_ID1)) {
		phy_devn = "Marvell";
		mii_mgr_read(phy_addr, 20, &phy_val);
		phy_val |= (1<<7);			// add delay to RX_CLK for RXD Outputs
		mii_mgr_write(phy_addr, 20, phy_val);
		mii_mgr_read(phy_addr, 0, &phy_val);
		phy_val |= (1<<15);			// PHY Software Reset
		mii_mgr_write(phy_addr, 0, phy_val);
	} else
	if ((phy_id0 == EV_VTSS_PHY_ID0) && (phy_id1 == EV_VTSS_PHY_ID1)) {
		phy_devn = "Vitesse VSC8601";
		mii_mgr_write(phy_addr, 31, 0x0001);	// extended page
		mii_mgr_read(phy_addr, 28, &phy_val);
		phy_val |=  (0x3<<12);			// RGMII RX skew compensation= 2.0 ns
		phy_val &= ~(0x3<<14);			// RGMII TX skew compensation= 0 ns
		mii_mgr_write(phy_addr, 28, phy_val);
		mii_mgr_write(phy_addr, 31, 0x0000);	// main registers
	}

	/* preconfigure PHY mode */
	ext_gphy_modeset(phy_addr);

	/* Disable EEE LPI for all phy by default (if supported) */
	ext_gphy_eee_enable(phy_addr, 0);

#ifdef CONFIG_GE2_RGMII_AN
	/* Disable EEE on external EPHY */
	ext_gphy_eee_enable(CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2, 0);
#endif

	if (ext_phy_mode == 10) {
	    phy_mode = "FORCE 10FDX";
	} else if (ext_phy_mode == 100) {
	    phy_mode = "FORCE 100FDX";
	} else if (ext_phy_mode == 1000) {
	    phy_mode = "FORCE 1000FDX";
	}

	if (phy_devn)
		printk("%s GPHY detected on MDIO addr: 0x%02X, mode: %s\n", phy_devn, phy_addr, phy_mode);
	else
		printk("Unknown EPHY (%04X:%04X) detected on MDIO addr: 0x%02X, mode: %s\n", phy_id0, phy_id1, phy_addr, phy_mode);
}

void ext_gphy_eee_enable(u32 phy_addr, int is_eee_enabled)
{
	u32 phy_id0, phy_id1, phy_rev;

	if (phy_addr > 0x1f)
		return;

	phy_id0 = g_phy_id[phy_addr] & 0xffff;
	phy_id1 = g_phy_id[phy_addr] >> 16;
	phy_rev = phy_id1 & 0xf;

	if ((phy_id0 == EV_REALTEK_PHY_ID0) && ((phy_id1 & 0xfff0) == EV_REALTEK_PHY_ID1)) {
		if (phy_rev >= 0x4) {
			/* EEE LPI 1000/100 advert (for D/E/F) */
			mii_mgr_write(phy_addr, 13, 0x0007);
			mii_mgr_write(phy_addr, 14, 0x003c);
			mii_mgr_write(phy_addr, 13, 0x4007);
			mii_mgr_write(phy_addr, 14, (is_eee_enabled) ? 0x0006 : 0x0000);
		}
	}
}

u32 ext_gphy_fill_pmsr(u32 phy_addr)
{
	u32 phy_id0, phy_id1, phy_rev, phy_val = 0;
	u32 pmsr = 0;

	if (phy_addr > 0x1f)
		return 0;

	phy_id0 = g_phy_id[phy_addr] & 0xffff;
	phy_id1 = g_phy_id[phy_addr] >> 16;
	phy_rev = phy_id1 & 0xf;

	if ((phy_id0 == EV_REALTEK_PHY_ID0) && ((phy_id1 & 0xfff0) == EV_REALTEK_PHY_ID1)) {
		if (phy_rev == 0x6) {
			mii_mgr_read(phy_addr, 26, &phy_val);
			if (phy_val && (1 << 2)) {
				if (phy_val & (1 << 6))
					pmsr |= (1 << 4);	// FC TX
				if (phy_val & (1 << 7))
					pmsr |= (1 << 5);	// FC RX
				if (phy_val & (1 << 8))
					pmsr |= (3 << 6);	// EEE
			}
		}
	}

	return pmsr;
}
#endif

#if defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR) || defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2)

#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_RALINK_MT7620)
void enable_autopoll_phy(int unused)
{
	u32 regValue, addr_s, addr_e;
#if defined (CONFIG_RALINK_MT7621)
	// PHY_ST_ADDR  = always GE1->EPHY address
	// PHY_END_ADDR = always GE2->EPHY address
#if defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2) && defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR)
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
#elif defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2)
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2-1;	// not used
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
#else
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR+1;	// not used
#endif
#elif defined (CONFIG_RALINK_MT7620)
	// PHY_ST_ADDR  = always P4->EPHY address
	// PHY_END_ADDR = always P5->EPHY address
#if defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2) && defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR)
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
#elif defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2)
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2+1;	// not used
#else
	addr_s = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR-1;	// not used
	addr_e = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
#endif
#endif

	regValue = sysRegRead(REG_MDIO_PHY_POLLING);
	regValue |= (1UL<<31);
	regValue &= ~(0x1f);
	regValue &= ~(0x1f<<8);
	regValue |= (addr_s & 0x1f);		// setup PHY address for auto polling (Start Addr).
	regValue |= ((addr_e & 0x1f) << 8);	// setup PHY address for auto polling (End Addr).
	sysRegWrite(REG_MDIO_PHY_POLLING, regValue);
}

#elif defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || \
      defined (CONFIG_RALINK_RT3883)
void enable_autopoll_phy(int ge)
{
	u32 regAddr, regValue;

	regAddr = REG_MDIO_PHY_POLLING;
#if defined (CONFIG_RALINK_RT3883) && defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2)
	if (ge == 2)
		regAddr = MDIO_CFG2;
#endif

	regValue = sysRegRead(regAddr);
	regValue &= 0xe0ff7fff;			// clear auto polling related field: (MD_PHY1ADDR & GP1_FRC_EN).
	regValue |= 0x20000000;			// force to enable MDC/MDIO auto polling.
#if defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2)
	if (ge == 2)
		regValue |= ((CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2 & 0x1f) << 24);	// setup PHY address for auto polling.
#endif
#if defined (CONFIG_MAC_TO_GIGAPHY_MODE_ADDR)
	if (ge != 2)
		regValue |= ((CONFIG_MAC_TO_GIGAPHY_MODE_ADDR & 0x1f) << 24);	// setup PHY address for auto polling.
#endif
	sysRegWrite(regAddr, regValue);
}
#endif
#endif

#if defined (CONFIG_RALINK_GPIO_SFPEN) && (CONFIG_RALINK_GPIO_SFPEN > -1)
static void ext_gphy_sfpen(void)
{
	u32 val = 0;

	/* switch JTAG to GPIO */
	val = sysRegRead(RALINK_REG_GPIOMODE);
	val |= (RALINK_GPIOMODE_JTAG); 						// GPIO 13-17 used for SFP module shared with JTAG
	sysRegWrite(RALINK_REG_GPIOMODE, val);

	val = sysRegRead(RALINK_REG_PIODIR);
	val |= (0x1<<CONFIG_RALINK_GPIO_SFPEN);					// switch pin to output mode (SFP TxEnable)
	val |= (0x1<<CONFIG_RALINK_GPIO_SFPPL);					// switch pin to output mode (SFP+ mode 10/1.25Gbps)
	val &= ~(0x1<<CONFIG_RALINK_GPIO_SFPTF);				// switch pin to input mode  (Transmitter Fault)
	val &= ~(0x1<<CONFIG_RALINK_GPIO_SFPLS);				// switch pin to input mode  (Receiver Loss of Signal)
	sysRegWrite(RALINK_REG_PIODIR, val);

	printk("Configure and enable SFP slot.\n");

	val = sysRegRead(RALINK_REG_PIODATA);
	val &= ~(0x1<<CONFIG_RALINK_GPIO_SFPPL);				// set pin to LOW (1.25Gbit/s)
	val &= ~(0x1<<CONFIG_RALINK_GPIO_SFPEN);				// set pin to LOW (SFP Enabled)
	sysRegWrite(RALINK_REG_PIODATA, val);
}
#endif

#if defined(CONFIG_RALINK_GPIO_PHY_PERST) && (CONFIG_RALINK_GPIO_PHY_PERST > -1)
static void ext_gphy_reset(void)
{
	u32 val = 0;

	printk("Hardware reset EPHY.\n");

	mdelay(10); /* wait 10ms for pulse */

	val = sysRegRead(RALINK_REG_PIODIR);					/* GPIO0 used for PE_RST PHY not shared */
	val |= (0x1<<CONFIG_RALINK_GPIO_PHY_PERST);				/* switch pin to output mode */
	sysRegWrite(RALINK_REG_PIODIR, val);

	val = sysRegRead(RALINK_REG_PIODATA);
	val &= ~(0x1<<CONFIG_RALINK_GPIO_PHY_PERST);				/* set pin to LOW (phy sleep) */
	sysRegWrite(RALINK_REG_PIODATA, val);

#if defined (CONFIG_RALINK_GPIO_SFPEN) && (CONFIG_RALINK_GPIO_SFPEN > -1)
	ext_gphy_sfpen();
#endif
	mdelay(10); /* wait 10ms for pulse ( For  a  complete  PHY reset, this pin must be asserted low for at least 10ms ) */

	val = sysRegRead(RALINK_REG_PIODATA);
	val |= (0x1<<CONFIG_RALINK_GPIO_PHY_PERST);				/* set pin to HIGH (phy wakeup) */
	sysRegWrite(RALINK_REG_PIODATA, val);

	/* wait 30ms for ready (The RTL8211 requires 30ms for power on reset, after which it can access the PHY register from MDC/MDIO). */
	mdelay(30);
}
#endif

/* called once on driver load */
void early_phy_init(void)
{
#if defined (CONFIG_RAETH_ESW) || defined (CONFIG_MT7530_GSW)
#if defined (CONFIG_P5_MAC_TO_PHY_MODE) || defined (CONFIG_GE2_RGMII_AN)
#define MAX_PHY_NUM	6
#else
#define MAX_PHY_NUM	5
#endif
	u32 i, phy_mdio_addr, phy_reg_mcr;
#endif

#if defined (CONFIG_P5_MAC_TO_PHY_MODE) || defined (CONFIG_GE1_RGMII_AN) || \
    defined (CONFIG_P4_MAC_TO_PHY_MODE) || defined (CONFIG_GE2_RGMII_AN) || \
    defined (CONFIG_MT7530_GSW)
	/* enable MDIO port */
	mii_mgr_init();
#endif

#if defined (CONFIG_RAETH_ESW) || defined (CONFIG_MT7530_GSW)
	/* early down all switch PHY (please enable from user-level) */
	for (i = 0; i < MAX_PHY_NUM; i++) {
		phy_mdio_addr = i;
#if defined (CONFIG_P4_MAC_TO_PHY_MODE)
		if (i == 4)
			phy_mdio_addr = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
#endif
#if defined (CONFIG_P5_MAC_TO_PHY_MODE)
		if (i == 5)
			phy_mdio_addr = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR;
#elif defined (CONFIG_GE2_RGMII_AN)
		if (i == 5)
			phy_mdio_addr = CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2;
#endif
		phy_reg_mcr = 0x3100;
		if (mii_mgr_read(phy_mdio_addr, 0, &phy_reg_mcr)) {
			if (phy_reg_mcr & (1<<11))
				continue;
			phy_reg_mcr &= ~(1<<9);
			phy_reg_mcr |= ((1<<12)|(1<<11));
			mii_mgr_write(phy_mdio_addr, 0, phy_reg_mcr);
		}
	}
#endif

#if defined (CONFIG_MT7530_GSW)
	/* early P5/P6 MAC link down */
	mii_mgr_write(MT7530_MDIO_ADDR, 0x3500, 0x8000);
	mii_mgr_write(MT7530_MDIO_ADDR, 0x3600, 0x8000);
#endif

#if defined(CONFIG_RALINK_GPIO_PHY_PERST) && (CONFIG_RALINK_GPIO_PHY_PERST > -1)
	/* hardware reset phy and SFP slot enable */
	ext_gphy_reset();
#endif
}

