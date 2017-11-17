#ifndef __RA_PHY_H__
#define __RA_PHY_H__

#if defined (CONFIG_RALINK_GPIO_MDIOSW) && (CONFIG_RALINK_GPIO_MDIOSW > -1)
void ext_gphy_mdioswitch(int on);
#endif

void ext_gphy_init(u32 phy_addr);
u32  ext_gphy_fill_pmsr(u32 phy_addr);
void ext_gphy_eee_enable(u32 phy_addr, int is_eee_enabled);
void enable_autopoll_phy(int ge);
void early_phy_init(void);
#endif
