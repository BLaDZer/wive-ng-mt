/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2007 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <asm/addrspace.h>
#include <common.h>
#include <command.h>

#include "phy.h"
#include "athrs16_phy.h" 

/* PHY selections and access functions */



#define DRV_DEBUG_PHYERROR  0x00000001
#define DRV_DEBUG_PHYCHANGE 0x00000002
#define DRV_DEBUG_PHYSETUP  0x00000004

#define ATHR_LAN_PORT_VLAN          1
#define ATHR_WAN_PORT_VLAN          2

/*depend on connection between cpu mac and s16 mac*/
#define ENET_UNIT_LAN 0  /* AR7100's MAC 0 is connected to LAN */
#define ENET_UNIT_WAN 1  /* AR7100's MAC 1 is connected to WAN */

#define TRUE    1
#define FALSE   0

#define ATHR_PHY0_ADDR   0x0
#define ATHR_PHY1_ADDR   0x1
#define ATHR_PHY2_ADDR   0x2
#define ATHR_PHY3_ADDR   0x3
#define ATHR_PHY4_ADDR   0x4
#define ATHR_IND_PHY 4
#define MODULE_NAME "ATHRS16"

/*
 * Track per-PHY port information.
 */
typedef struct {
    int  isEnetPort;       /* normal enet port */
    int   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    u32 phyBase;
    u32 phyAddr;          /* PHY registers associated with this phy port */
    u32 VLANTableSetting; /* Value to be written to VLAN table */
} athrPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */
static athrPhyInfo_t athrPhyInfo[] = {
    {TRUE,   /* phy port 0 -- LAN port 0 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY0_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY1_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY2_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY3_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 4 -- WAN port or LAN port 4 */
     FALSE,
     ENET_UNIT_WAN,
     0,
     ATHR_PHY4_ADDR, 
     ATHR_LAN_PORT_VLAN   /* Send to all ports */
    },

    {FALSE,  /* phy port 5 -- CPU port (no RJ45 connector) */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00, 
     ATHR_LAN_PORT_VLAN    /* Send to all ports */
    },
};

static u8 athr16_init_flag = 0;
#define ATHR_GLOBALREGBASE    0

//#define ATHR_PHY_MAX (sizeof(athrPhyInfo) / sizeof(athrPhyInfo[0]))
#define ATHR_PHY_MAX 5

/* Range of valid PHY IDs is [MIN..MAX] */
#define ATHR_ID_MIN 0
#define ATHR_ID_MAX (ATHR_PHY_MAX-1)

/* Convenience macros to access myPhyInfo */
#define ATHR_IS_ENET_PORT(phyUnit) (athrPhyInfo[phyUnit].isEnetPort)
#define ATHR_IS_PHY_ALIVE(phyUnit) (athrPhyInfo[phyUnit].isPhyAlive)
#define ATHR_ETHUNIT(phyUnit) (athrPhyInfo[phyUnit].ethUnit)
#define ATHR_PHYBASE(phyUnit) (athrPhyInfo[phyUnit].phyBase)
#define ATHR_PHYADDR(phyUnit) (athrPhyInfo[phyUnit].phyAddr)
#define ATHR_VLAN_TABLE_SETTING(phyUnit) (athrPhyInfo[phyUnit].VLANTableSetting)


#define ATHR_IS_ETHUNIT(phyUnit, ethUnit) \
            (ATHR_IS_ENET_PORT(phyUnit) &&        \
            ATHR_ETHUNIT(phyUnit) == (ethUnit))

#define ATHR_IS_WAN_PORT(phyUnit) (!(ATHR_ETHUNIT(phyUnit)==ENET_UNIT_LAN))
            
/* Forward references */
int athrs16_phy_is_link_alive(int phyUnit);
static u32 athrs16_reg_read(u32 reg_addr);
static void athrs16_reg_write(u32 reg_addr, u32 reg_val);

#define sysMsDelay(_x) udelay((_x) * 1000)

void phy_mode_setup() 
{
    /*work around for phy4 rgmii mode*/
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 18);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x480c);    

    /*rx delay*/ 
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 0);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x824e);  

    /*tx delay*/ 
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 5);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x3d47);    

}

void athrs16_reg_init()
{
    if (athr16_init_flag)
        return;

    /*Power on strip mode setup*/
    athrs16_reg_write(0x8, 0x012e1bea);
    
    athrs16_reg_write(0x100, 0x7e);
    athrs16_reg_write(0x200, 0x200);
    athrs16_reg_write(0x300, 0x200);
    athrs16_reg_write(0x400, 0x200);
    athrs16_reg_write(0x500, 0x200);
    athrs16_reg_write(0x600, 0x0);

    athrs16_reg_write(0x38, 0xc000050e);
        
       
    athrs16_reg_write(0x60, 0xffffffff);
    athrs16_reg_write(0x64, 0xaaaaaaaa);
    athrs16_reg_write(0x68, 0x55555555);    
    athrs16_reg_write(0x6c, 0x0);    

    athrs16_reg_write(0x70, 0x41af);

    athr16_init_flag = 1;
}

/******************************************************************************
*
* athrs16_phy_is_link_alive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/
int
athrs16_phy_is_link_alive(int phyUnit)
{
    u16 phyHwStatus;
    u32 phyBase;
    u32 phyAddr;

    phyBase = ATHR_PHYBASE(phyUnit);
    phyAddr = ATHR_PHYADDR(phyUnit);

    phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS);

    if (phyHwStatus & ATHR_STATUS_LINK_PASS)
        return TRUE;

    return FALSE;
}


/******************************************************************************
*
* athrs16_phy_setup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

int
athrs16_phy_setup(int ethUnit)
{
    int         phyUnit;
    u16    phyHwStatus;
    u16    timeout;
    int         liveLinks = 0;
    u32    phyBase = 0;
    int        foundPhy = FALSE;
    u32    phyAddr = 0;
    

    //DBG 
    /* See if there's any configuration data for this enet */
    /* start auto negogiation on each phy */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        foundPhy = TRUE;
        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);
        
        phy_reg_write(phyBase, phyAddr, ATHR_AUTONEG_ADVERT,
                      ATHR_ADVERTISE_ALL);

        phy_reg_write(phyBase, phyAddr, ATHR_1000BASET_CONTROL,
                      ATHR_ADVERTISE_1000FULL);

        /* Reset PHYs*/
        phy_reg_write(phyBase, phyAddr, ATHR_PHY_CONTROL,
                      ATHR_CTRL_AUTONEGOTIATION_ENABLE 
                      | ATHR_CTRL_SOFTWARE_RESET);
    }

    if (!foundPhy) {
        return FALSE; /* No PHY's configured for this ethUnit */
    }
    
    /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */
    sysMsDelay(1000);
    
    /*
     * Wait up to .75 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    for (phyUnit=0; (phyUnit < ATHR_PHY_MAX) /*&& (timeout > 0) */; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        timeout=20;
        for (;;) {
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_CONTROL);

            if (ATHR_RESET_DONE(phyHwStatus)) {
                break;
            }
            if (timeout == 0) {
                printf("Port %d, Negogiation timeout\n", phyUnit);
                break;
            }
            if (--timeout == 0) {
               	printf("Port %d, Negogiation timeout\n", phyUnit); 
                break;
            }

            sysMsDelay(150);
        }
    }

    /*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(phyUnit)) {
            liveLinks++;
            ATHR_IS_PHY_ALIVE(phyUnit) = TRUE;
        } else {
            ATHR_IS_PHY_ALIVE(phyUnit) = FALSE;
        }

        printf("eth%d: Phy Specific Status=%4.4x\n", 
            ethUnit, 
            phy_reg_read(ATHR_PHYBASE(phyUnit),
                       ATHR_PHYADDR(phyUnit),
                       ATHR_PHY_SPEC_STATUS));
    }

    if (ethUnit == ENET_UNIT_LAN) {
        athrs16_reg_init();
    }

    return (liveLinks > 0);
}

/******************************************************************************
*
* athrs16_phy_is_fdx - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1 --> FULL
*    0 --> HALF
*/
int
athrs16_phy_is_fdx(int ethUnit)
{
    int         phyUnit;
    u32    phyBase;
    u32    phyAddr;
    u16    phyHwStatus;
    int         ii = 200;
    
    if (ethUnit == ENET_UNIT_LAN)
        return TRUE;
    
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(phyUnit)) {

            phyBase = ATHR_PHYBASE(phyUnit);
            phyAddr = ATHR_PHYADDR(phyUnit);

            do {
                phyHwStatus = phy_reg_read(phyBase, phyAddr, 
                                              ATHR_PHY_SPEC_STATUS);
        	sysMsDelay(10);
            } while((!(phyHwStatus & ATHR_STATUS_RESOVLED)) && --ii);

            if (phyHwStatus & ATHER_STATUS_FULL_DEPLEX)
                return TRUE;
        }
    }

    return FALSE;
}


/******************************************************************************
*
* athrs16_phy_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/

int
athrs16_phy_speed(int ethUnit)
{
    int         phyUnit;
    u16    phyHwStatus;
    u32    phyBase;
    u32    phyAddr;
    int         ii = 200;
    
    if (ethUnit == ENET_UNIT_LAN)
        return AG7100_PHY_SPEED_1000T;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(phyUnit)) {

            phyBase = ATHR_PHYBASE(phyUnit);
            phyAddr = ATHR_PHYADDR(phyUnit);
            
            do {
                phyHwStatus = phy_reg_read(phyBase, phyAddr, 
                                              ATHR_PHY_SPEC_STATUS);
                sysMsDelay(10);
            }while((!(phyHwStatus & ATHR_STATUS_RESOVLED)) && --ii);

            phyHwStatus = ((phyHwStatus & ATHER_STATUS_LINK_MASK) >>
                           ATHER_STATUS_LINK_SHIFT);

            switch(phyHwStatus) {
            case 0:
                return AG7100_PHY_SPEED_10T;
            case 1:
                return AG7100_PHY_SPEED_100TX;
            case 2:
                return AG7100_PHY_SPEED_1000T;
            default:
                printf("Unkown speed read!\n");
            }
        }
    }

    return AG7100_PHY_SPEED_10T;
}

/*****************************************************************************
*
* athr_phy_is_up -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/

int
athrs16_phy_is_up(int ethUnit)
{
    int           phyUnit;
    u16      phyHwStatus, phyHwControl;
    athrPhyInfo_t *lastStatus;
    int           linkCount   = 0;
    int           lostLinks   = 0;
    int           gainedLinks = 0;
    u32      phyBase;
    u32      phyAddr;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);

        lastStatus = &athrPhyInfo[phyUnit];

        if (lastStatus->isPhyAlive) { /* last known link status was ALIVE */
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS);

            /* See if we've lost link */
            if (phyHwStatus & ATHR_STATUS_LINK_PASS) {
                linkCount++;
            } else {
                lostLinks++;
                printf("\nenet%d port%d down\n",
                                               ethUnit, phyUnit);
                lastStatus->isPhyAlive = FALSE;
            }
        } else { /* last known link status was DEAD */
            /* Check for reset complete */
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_STATUS);
            if (!ATHR_RESET_DONE(phyHwStatus))
                continue;

            phyHwControl = phy_reg_read(phyBase, phyAddr, ATHR_PHY_CONTROL);
            /* Check for AutoNegotiation complete */            
            if ((!(phyHwControl & ATHR_CTRL_AUTONEGOTIATION_ENABLE)) 
                 || ATHR_AUTONEG_DONE(phyHwStatus)) {
                phyHwStatus = phy_reg_read(phyBase, phyAddr, 
                                           ATHR_PHY_SPEC_STATUS);

                if (phyHwStatus & ATHR_STATUS_LINK_PASS) {
                gainedLinks++;
                linkCount++;
                printf("\nenet%d port%d up\n",
                                               ethUnit, phyUnit);
                lastStatus->isPhyAlive = TRUE;
                }
            }
        }
    }

    return (linkCount);
}

static u32
athrs16_reg_read(u32 reg_addr)
{
    u32 reg_word_addr;
    u32 phy_addr, tmp_val, reg_val;
    u16 phy_val;
    u8 phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (u16) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    reg_val = (u32) phy_reg_read(0, phy_addr, phy_reg);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    tmp_val = (u32) phy_reg_read(0, phy_addr, phy_reg);
    reg_val |= (tmp_val << 16);

    return reg_val;   
}

static void
athrs16_reg_write(u32 reg_addr, u32 reg_val)
{
    u32 reg_word_addr;
    u32 phy_addr;
    u16 phy_val;
    u8 phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (u16) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (u16) ((reg_val >> 16) & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (u8) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (u16) (reg_val & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val); 
}

int ag7100_miiphy_read(char *devname, unsigned char phy_addr, unsigned char phy_register, unsigned short *value)
{
        unsigned int volatile status = 0;
        unsigned long volatile t_start = get_timer(0);

        /* We enable mdio gpio purpose register, and disable it when exit. */
        enable_mdio(1);

        // make sure previous read operation is complete
        while (1) {
                // rd_rdy: read operation is complete
                if(!( inw(MDIO_PHY_CONTROL_1) & (0x1 << 1)))
                {
                        break;
                }
                else if (get_timer(t_start) > (5 * CFG_HZ)) {
                        enable_mdio(0);
                        return 0;
                }
        }

        outw(MDIO_PHY_CONTROL_0 , (1<<14) | (phy_register << 8) |(phy_addr));

        // make sure read operation is complete
        t_start = get_timer(0);
        while (1) {
                if (inw(MDIO_PHY_CONTROL_1) & (0x1 << 1)) {
                        status = inw(MDIO_PHY_CONTROL_1);
                        *value  = (status >> 16);

                        enable_mdio(0);
                }
                else if (get_timer(t_start) > (5 * CFG_HZ)) {
                        enable_mdio(0);
                        return 0;
                }
        }
}

int ag7100_miiphy_write(char *devname, unsigned char phy_addr, unsigned char phy_register, unsigned short write_data)
{
        unsigned long volatile t_start=get_timer(0);
        unsigned int volatile data;

        enable_mdio(1);

        // make sure previous write operation is complete
        while(1) {
                if (!(inw(MDIO_PHY_CONTROL_1) & (0x1 << 0)))
                {
                        break;
                }
                else if (get_timer(t_start) > (5 * CFG_HZ)) {
                        enable_mdio(0);
                        return 0;
                }
        }

        data = ((write_data & 0xFFFF) << 16);
        data |= (phy_register << 8) | (phy_addr);
        data |= (1<<13);
        outw(MDIO_PHY_CONTROL_0, data);

        t_start = get_timer(0);
        // make sure write operation is complete
        while (1) {
                if (inw(MDIO_PHY_CONTROL_1) & (0x1 << 0)) //wt_done ?= 1
                {
                     enable_mdio(0);
                     return 1;
                }
                else if (get_timer(t_start) > (5 * CFG_HZ)) {
                        enable_mdio(0);
                        return 0;
                }
        }
}

