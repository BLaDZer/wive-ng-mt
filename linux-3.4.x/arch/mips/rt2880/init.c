/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     init setup for Ralink RT2880 solution
 *
 *  Copyright 2007 Ralink Inc. (bruce_chang@ralinktech.com.tw)
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2007 Bruce Chang
 *
 * Initial Release
 *
 *
 *
 **************************************************************************
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/serial_8250.h>
#include <linux/delay.h>
#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/rt2880/prom.h>
#include <asm/rt2880/generic.h>
#include <asm/rt2880/surfboard.h>
#include <asm/rt2880/surfboardint.h>
#include <asm/rt2880/rt_mmap.h>
#include <asm/rt2880/rt_serial.h>

#define UART_BAUDRATE	CONFIG_RALINK_UART_BRATE

u32 mips_cpu_feq;
u32 surfboard_sysclk;
u32 ralink_asic_rev_id;
EXPORT_SYMBOL(ralink_asic_rev_id);

#ifdef CONFIG_UBOOT_CMDLINE
/* Environment variable */
typedef struct {
	char *name;
	char *val;
} t_env_var;

int prom_argc;
int *_prom_argv, *_prom_envp;

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension, if running in 64-bit mode.
 */
#define prom_envp(index) ((char *)(((int *)(int)_prom_envp)[(index)]))
#endif

char *prom_getenv(char *envname)
{
#ifdef CONFIG_UBOOT_CMDLINE
	/*
	 * Return a pointer to the given environment variable.
	 * In 64-bit mode: we're using 64-bit pointers, but all pointers
	 * in the PROM structures are only 32-bit, so we need some
	 * workarounds, if we are running in 64-bit mode.
	 */
	int i, index=0;
	char *p, *q;

	i = strlen(envname);
	while (prom_envp(index)) {
		p = (char*) KSEG0ADDR(prom_envp(index));
		if(!strncmp(envname, p, i)) {
			q = strchr(p, '=');
			if (q)
				q++;
			return q;
		}
		index++;
	}
#endif
	return NULL;
}

static inline unsigned char str2hexnum(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0; /* foo */
}

static inline void str2eaddr(unsigned char *ea, unsigned char *str)
{
	int i;

	for (i = 0; i < 6; i++) {
		unsigned char num;

		if((*str == '.') || (*str == ':'))
			str++;
		num = str2hexnum(*str++) << 4;
		num |= (str2hexnum(*str++));
		ea[i] = num;
	}
}

#if defined (CONFIG_RALINK_MT7620) || defined (CONFIG_RALINK_MT7628)

#define RALINK_CLKCFG1			*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x30)
#define RALINK_RSTCTRL			*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x34)
#define PPLL_CFG0			*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x98)
#define PPLL_CFG1			*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x9c)
#define PPLL_DRV			*(volatile u32 *)(RALINK_SYSCTL_BASE + 0xa0)

/* PCI-E Phy read/write */
#define PCIEPHY0_CFG			(RALINK_PCI_BASE + 0x90)
#define BUSY				0x80000000
#define WAITRETRY_MAX			10
#define WRITE_MODE			(1UL<<23)
#define DATA_SHIFT			0
#define ADDR_SHIFT			8

static int wait_pciephy_busy(void)
{
	u32 reg_value = 0x0, retry = 0;

	while(1){
		reg_value = (*((volatile u32 *)PCIEPHY0_CFG));
		
		if(reg_value & BUSY)
			mdelay(100);
		else
			break;
		if(retry++ > WAITRETRY_MAX){
			printk("PCIE-PHY retry failed.\n");
			return -1;
		}
	}
	return 0;
}

static u32 pcie_phy(char rwmode, unsigned long addr, unsigned long val)
{
	u32 reg_value = 0x0;

	wait_pciephy_busy();
	if(rwmode == 'w'){
		reg_value |= WRITE_MODE;
		reg_value |= (val) << DATA_SHIFT;
	}
	reg_value |= (addr) << ADDR_SHIFT;

	// apply the action
	(*((volatile u32 *)PCIEPHY0_CFG)) = reg_value;

	mdelay(1);
	wait_pciephy_busy();

	if(rwmode == 'r'){
		reg_value = (*((volatile u32 *)PCIEPHY0_CFG));
		return reg_value;
	}
	return 0;
}

static void prom_init_pcie(void)
{
	/* pcie bypass dll */
	pcie_phy('w', 0x0, 0x80);
	pcie_phy('w', 0x1, 0x04);

	pcie_phy('w', 0x68, 0xB4);

	RALINK_RSTCTRL = (RALINK_RSTCTRL | RALINK_PCIE0_RST);
	RALINK_CLKCFG1 = (RALINK_CLKCFG1 & ~RALINK_PCIE0_CLK_EN);
	PPLL_DRV = (PPLL_DRV & ~(1<<19));
	PPLL_DRV = (PPLL_DRV | 1<<31);

#if defined (CONFIG_RALINK_MT7620)
	if (!(ralink_asic_rev_id & (1UL<<16))) {
		/* Only MT7620N do this, not MT7620A */
		PPLL_CFG0 = (PPLL_CFG0 | (1UL << 31));
		PPLL_CFG1 = (PPLL_CFG1 | (1UL << 26));
	}
#elif defined (CONFIG_RALINK_MT7628)
	/* todo */
#endif
}
#else
static void prom_init_pcie(void)
{
}
#endif

static void prom_init_usb(void)
{
#if !defined (CONFIG_RALINK_RT3052) && !defined (CONFIG_RALINK_MT7621)
	u32 reg=0;
#endif
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || \
    defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7620) || \
    defined (CONFIG_RALINK_MT7628)
	reg = *(volatile u32 *)KSEG1ADDR((RALINK_SYSCTL_BASE + 0x34));
	reg |= (RALINK_UDEV_RST | RALINK_UHST_RST);
	*(volatile u32 *)KSEG1ADDR((RALINK_SYSCTL_BASE + 0x34))= reg;

	reg = *(volatile u32 *)KSEG1ADDR((RALINK_SYSCTL_BASE + 0x30));
#if defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7620) || \
    defined (CONFIG_RALINK_MT7628)
	reg = reg & ~(RALINK_UPHY0_CLK_EN);
#else
	reg = reg & ~(RALINK_UPHY0_CLK_EN | RALINK_UPHY1_CLK_EN);
#endif
	*(volatile u32 *)KSEG1ADDR((RALINK_SYSCTL_BASE + 0x30))= reg;
#elif defined (CONFIG_RALINK_RT3052)
	*(volatile u32 *)KSEG1ADDR((RALINK_USB_OTG_BASE + 0xE00)) = 0xF;	// power saving
#elif defined (CONFIG_RALINK_MT7621)
	/* TODO */
#endif
}

static void prom_init_sysclk(void)
{
	char asic_id[8], *ram_type = "SDRAM";
	u32 reg;
	u8  clk_sel;
#if defined (CONFIG_RT5350_ASIC) || defined (CONFIG_MT7620_ASIC) || \
    defined (CONFIG_MT7621_ASIC) || defined (CONFIG_MT7628_ASIC)
	u8  clk_sel2;
#endif
#if defined (CONFIG_RALINK_MT7621)
	u32 cpu_fdiv = 0;
	u32 cpu_ffrac = 0;
	u32 fbdiv = 0;
#endif

	reg = (*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x00));
	memcpy(asic_id, &reg, 4);
	reg = (*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x04));
	memcpy(asic_id+4, &reg, 4);
	asic_id[6] = '\0';

	ralink_asic_rev_id = (*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x0c));

#if defined (CONFIG_RALINK_MT7620)
	if (ralink_asic_rev_id & (1UL<<16))
		asic_id[6] = 'A';
	else
		asic_id[6] = 'N';
	asic_id[7] = '\0';
#endif

	reg = (*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x10)));

#if defined (CONFIG_RT3052_ASIC)
	clk_sel = (reg>>18) & 0x01;
#elif defined (CONFIG_RT3352_ASIC)
	clk_sel = (reg>>8) & 0x01;
#elif defined (CONFIG_RT5350_ASIC)
	clk_sel = (reg>>8) & 0x01;
	clk_sel2 = (reg>>10) & 0x01;
	clk_sel |= (clk_sel2 << 1);
#elif defined (CONFIG_RT3883_ASIC)
	clk_sel = (reg>>8) & 0x03;
#elif defined (CONFIG_MT7620_ASIC) || defined (CONFIG_MT7628_ASIC)
	clk_sel = 0;	/* clock from CPU PLL (600MHz) */
	clk_sel2 = (reg>>4) & 0x03;
	reg = (*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x58)));
	if (reg & (0x1UL << 24))
		clk_sel = 1;	/* clock from BBP PLL (480MHz ) */
#elif defined (CONFIG_MT7621_ASIC)
	clk_sel = 0;	/* GPLL (500MHz) */
	clk_sel2 = (reg>>4) & 0x01;
	reg = (*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x2C)));
	if (reg & (0x1UL << 30))
		clk_sel = 1;	/* CPU PLL */
#else
#error Please Choice System Type
#endif

	switch(clk_sel) {
#if defined (CONFIG_RALINK_RT3052)
#if defined (CONFIG_RALINK_RT3350)
		// MA10 is floating
	case 0:
	case 1:
		mips_cpu_feq = (320*1000*1000);
		break;
#else
	case 0:
		mips_cpu_feq = (320*1000*1000);
		break;
	case 1:
		mips_cpu_feq = (384*1000*1000);
		break;
#endif
#elif defined (CONFIG_RALINK_RT3352)
	case 0:
		mips_cpu_feq = (384*1000*1000);
		break;
	case 1:
		mips_cpu_feq = (400*1000*1000);
		break;
#elif defined (CONFIG_RALINK_RT3883)
	case 0:
		mips_cpu_feq = (250*1000*1000);
		break;
	case 1:
		mips_cpu_feq = (384*1000*1000);
		break;
	case 2:
		mips_cpu_feq = (480*1000*1000);
		break;
	case 3:
		mips_cpu_feq = (500*1000*1000);
		break;
#elif defined (CONFIG_RALINK_RT5350)
	case 0:
		mips_cpu_feq = (360*1000*1000);
		break;
	case 1:
		//reserved
		break;
	case 2:
		mips_cpu_feq = (320*1000*1000);
		break;
	case 3:
		mips_cpu_feq = (300*1000*1000);
		break;
#elif defined (CONFIG_RALINK_MT7620) || defined (CONFIG_RALINK_MT7628)
	case 0:
		/* set CPU ratio to 3/3 for normal mode (1/3 for sleep mode) */
		reg = (*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x3C)));
		reg &= ~0x1F1F;
		reg |=  0x0303;
		(*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x3C))) = reg;
		udelay(10);
		reg = (*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x54)));
		if (reg & CPLL_SW_CONFIG) {
			int rewrite_reg = 0;
			u32 pll_mult_ratio;
			u32 pll_div_ratio;
			/* disable bit SSC_EN (wrong CPU_PLL frequency, cause system clock drift) */
			if (reg & 0x80) {
				reg &= ~(0x80); 
				rewrite_reg = 1;
			}
#if defined (CONFIG_RALINK_MT7620_PLL600)
			pll_mult_ratio = (reg & CPLL_MULT_RATIO) >> CPLL_MULT_RATIO_SHIFT;
			pll_div_ratio = (reg & CPLL_DIV_RATIO) >> CPLL_DIV_RATIO_SHIFT;
			if (pll_mult_ratio != 6 || pll_div_ratio != 0) {
				reg &= ~(CPLL_MULT_RATIO);
				reg &= ~(CPLL_DIV_RATIO);
				reg |=  (6 << CPLL_MULT_RATIO_SHIFT);
				rewrite_reg = 1;
			}
#endif
			if (rewrite_reg) {
				(*((volatile u32 *)(RALINK_SYSCTL_BASE + 0x54))) = reg;
				udelay(10);
			}
			
			/* read CPLL_CFG0 to determine real CPU clock */
			pll_mult_ratio = (reg & CPLL_MULT_RATIO) >> CPLL_MULT_RATIO_SHIFT;
			pll_div_ratio = (reg & CPLL_DIV_RATIO) >> CPLL_DIV_RATIO_SHIFT;
			pll_mult_ratio += 24;	/* begin from 24 */
			if(pll_div_ratio == 0)	/* define from datasheet */
				pll_div_ratio = 2;
			else if(pll_div_ratio == 1)
				pll_div_ratio = 3;
			else if(pll_div_ratio == 2)
				pll_div_ratio = 4;
			else if(pll_div_ratio == 3)
				pll_div_ratio = 8;
			mips_cpu_feq = ((BASE_CLOCK * pll_mult_ratio ) / pll_div_ratio) * 1000 * 1000;
		} else {
			mips_cpu_feq = (600*1000*1000);
		}
		break;
	case 1:
		mips_cpu_feq = (480*1000*1000);
		break;
#elif defined (CONFIG_RALINK_MT7621)
	case 0: /* GPLL (500MHz) */
		reg = (*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x44));
		cpu_fdiv = ((reg >> 8) & 0x1F);
		cpu_ffrac = (reg & 0x1F);
		mips_cpu_feq = (500 * cpu_ffrac / cpu_fdiv) * 1000 * 1000;
		break;
	case 1: /* CPU PLL */
		reg = (*(volatile u32 *)(RALINK_MEMCTRL_BASE + 0x648));
		fbdiv = ((reg >> 4) & 0x7F) + 1;
		reg = (*(volatile u32 *)(RALINK_SYSCTL_BASE + 0x10));
		reg = (reg >> 6) & 0x7;
		if (reg >= 6)
			mips_cpu_feq = 25 * fbdiv * 1000 * 1000;	// 25Mhz Xtal
		else if (reg >= 3)
			mips_cpu_feq = 20 * fbdiv * 1000 * 1000;	// 40Mhz Xtal
		else {
			/* 20Mhz Xtal: TODO */
		}
		break;
#else
#error Please Choice Chip Type
#endif
	}

#if defined (CONFIG_RT3883_ASIC)
	if ((reg>>17) & 0x1) { //DDR2
		switch (clk_sel) {
		case 0:
			surfboard_sysclk = (125*1000*1000);
			break;
		case 1:
			surfboard_sysclk = (128*1000*1000);
			break;
		case 2:
			surfboard_sysclk = (160*1000*1000);
			break;
		case 3:
			surfboard_sysclk = (166*1000*1000);
			break;
		}
		ram_type = "DDR2";
	}
	else { //SDR
		switch (clk_sel) {
		case 0:
			surfboard_sysclk = (83*1000*1000);
			break;
		case 1:
			surfboard_sysclk = (96*1000*1000);
			break;
		case 2:
			surfboard_sysclk = (120*1000*1000);
			break;
		case 3:
			surfboard_sysclk = (125*1000*1000);
			break;
		}
	}
#elif defined (CONFIG_RT3352_ASIC)
	if ((reg>>17) & 0x1) {
		ram_type = "DDR2";
	}
#elif defined(CONFIG_RT5350_ASIC)
	switch (clk_sel) {
	case 0:
		surfboard_sysclk = (120*1000*1000);
		break;
	case 1:
		//reserved
		break;
	case 2:
		surfboard_sysclk = (80*1000*1000);
		break;
	case 3:
		surfboard_sysclk = (100*1000*1000);
		break;
	}
#elif defined (CONFIG_RALINK_MT7620)
	switch (clk_sel2) {
	case 0:
		surfboard_sysclk = mips_cpu_feq/4;	/* SDR 150MHz */
		break;
	case 1:
		surfboard_sysclk = mips_cpu_feq/3;	/* DDR1 */
		ram_type = "DDR1";
		break;
	case 2:
		surfboard_sysclk = mips_cpu_feq/3;	/* DDR2 */
		ram_type = "DDR2";
		break;
	case 3:
		surfboard_sysclk = mips_cpu_feq/5;	/* SDR 120MHz */
		break;
	}
#elif defined (CONFIG_RALINK_MT7628)
	surfboard_sysclk = mips_cpu_feq/3;
	switch (clk_sel2 & 0x01) {
	case 0:
		ram_type = "DDR2";
		break;
	case 1:
		ram_type = "DDR1";
		break;
	}
#elif defined (CONFIG_RALINK_MT7621)
	surfboard_sysclk = mips_cpu_feq/4;
	switch (clk_sel2) {
	case 0:
		ram_type = "DDR3";
		break;
	case 1:
		ram_type = "DDR2";
		break;
	}
#else
	surfboard_sysclk = mips_cpu_feq/3;
#endif
	printk("\nRalink SoC: %s, RevID: %04X, RAM: %s\n", asic_id, (ralink_asic_rev_id & 0xffff), ram_type);
	printk("CPU/SYS frequency: %d/%d MHz\n", mips_cpu_feq / 1000 / 1000, surfboard_sysclk / 1000 / 1000);

#if defined (CONFIG_RALINK_SYSTICK_COUNTER) && defined (CONFIG_RALINK_CPUSLEEP)
	/* enable cpu sleep mode for power saving */
	printk("CPU sleep mode: ON\n");
#if defined (CONFIG_RALINK_MT7621)
	reg = (*((volatile u32 *)(RALINK_RBUS_MATRIXCTL_BASE + 0x14)));
	reg |= 0x80000000;
	(*((volatile u32 *)(RALINK_RBUS_MATRIXCTL_BASE + 0x14))) = reg;
#else
	reg = (*((volatile u32 *)(RALINK_CPU_CLK_AUTO_CFG)));
	reg |= 0x80000000;
	(*((volatile u32 *)(RALINK_CPU_CLK_AUTO_CFG))) = reg;
#endif
#endif
}

/*
** This function sets up the local prom_rs_table used only for the fake console
** console (mainly prom_printf for debug display and no input processing)
** and also sets up the global rs_table used for the actual serial console.
** To get the correct baud_base value, prom_init_sysclk() must be called before
** this function is called.
*/

void __init prom_init_serial_port(void)
{
	unsigned int uartclk;
	unsigned int clock_divisor;
	struct uart_port serial_req[2];

	/*
	 * baud rate = system clock freq / (CLKDIV * 16)
	 * CLKDIV=system clock freq/16/baud rate
	 */
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || \
    defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7620) || \
    defined (CONFIG_RALINK_MT7628)
	uartclk = 40000000;
#elif defined (CONFIG_RALINK_MT7621)
	uartclk = 50000000;
#else
	uartclk = surfboard_sysclk;
#endif
	clock_divisor = (uartclk / SURFBOARD_BAUD_DIV / UART_BAUDRATE);

	memset(serial_req, 0, 2*sizeof(struct uart_port));

	// fixed 8n1
	IER(RALINK_SYSCTL_BASE + 0xC00) = 0;
	FCR(RALINK_SYSCTL_BASE + 0xC00) = 0;
	LCR(RALINK_SYSCTL_BASE + 0xC00) = UART_LCR_WLEN8;
	DLL(RALINK_SYSCTL_BASE + 0xC00) = (clock_divisor & 0xFF);
	DLM(RALINK_SYSCTL_BASE + 0xC00) = (clock_divisor >> 8) & 0xFF;

	// fixed 8n1
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_RALINK_MT7628)
	IER(RALINK_SYSCTL_BASE + 0xD00) = 0;
	FCR(RALINK_SYSCTL_BASE + 0xD00) = 0;
	LCR(RALINK_SYSCTL_BASE + 0xD00) = UART_LCR_WLEN8;
	DLL(RALINK_SYSCTL_BASE + 0xD00) = (clock_divisor & 0xFF);
	DLM(RALINK_SYSCTL_BASE + 0xD00) = (clock_divisor >> 8) & 0xFF;
#else
	IER(RALINK_SYSCTL_BASE + 0x500) = 0;
	FCR(RALINK_SYSCTL_BASE + 0x500) = 0;
	LCR(RALINK_SYSCTL_BASE + 0x500) = UART_LCR_WLEN8;
	DLL(RALINK_SYSCTL_BASE + 0x500) = (clock_divisor & 0xFF);
	DLM(RALINK_SYSCTL_BASE + 0x500) = (clock_divisor >> 8) & 0xFF;
#endif

	serial_req[0].line       = 0;
	serial_req[0].type       = PORT_16550A;
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_RALINK_MT7628)
	serial_req[0].iotype     = UPIO_MEM32;
#else
	serial_req[0].iotype     = UPIO_AU;
#endif
	serial_req[0].irq        = SURFBOARDINT_UART1;
	serial_req[0].flags      = UPF_SKIP_TEST;
	serial_req[0].uartclk    = uartclk;
	serial_req[0].regshift   = 2;
	serial_req[0].mapbase    = RALINK_UART_LITE_BASE;
	serial_req[0].membase    = ioremap_nocache(serial_req[0].mapbase, 0x0100);
	early_serial_setup(&serial_req[0]);

#if !defined(CONFIG_RALINK_GPIOMODE_UARTF) && (CONFIG_SERIAL_8250_NR_UARTS > 1)
	serial_req[1].line       = 1;
	serial_req[1].type       = PORT_16550A;
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_RALINK_MT7628)
	serial_req[1].iotype     = UPIO_MEM32;
#else
	serial_req[1].iotype     = UPIO_AU;
#endif
	serial_req[1].irq        = SURFBOARDINT_UART2;
	serial_req[1].flags      = UPF_SKIP_TEST;
	serial_req[1].uartclk    = uartclk;
	serial_req[1].regshift   = 2;
	serial_req[1].mapbase    = RALINK_UART_BASE;
	serial_req[1].membase    = ioremap_nocache(serial_req[1].mapbase, 0x0100);
	early_serial_setup(&serial_req[1]);
#endif
}

int __init prom_get_ttysnum(void)
{
	char *argptr;
	int ttys_num = 0;  /* default UART Lite */

	/* get ttys_num to use with the fake console/prom_printf */
	argptr = prom_getcmdline();

	if ((argptr = strstr(argptr, "console=ttyS")) != NULL) {
		argptr += strlen("console=ttyS");
		if (argptr[0] == '0')           /* ttyS0 */
			ttys_num = 0;           /* happens to be rs_table[0] */
		else if (argptr[0] == '1')      /* ttyS1 */
			ttys_num = 1;           /* happens to be rs_table[1] */
	}

	return (ttys_num);
}

void __init prom_init(void)
{
	mips_machtype = MACH_RALINK_ROUTER;

#ifdef CONFIG_UBOOT_CMDLINE
	prom_argc = fw_arg0;
	_prom_argv = (int*) fw_arg1;
	_prom_envp = (int*) fw_arg2;
#endif

	set_io_port_base(KSEG1);

	/* remove all wired TLB entries */
	write_c0_wired(0);

	prom_init_cmdline();
	prom_init_printf(prom_get_ttysnum());
	prom_init_sysclk();
	prom_init_serial_port();	/* Needed for Serial Console */
	prom_init_usb();		/* USB power saving */
	prom_init_pcie();		/* PCIe power saving*/
	prom_meminit();
	prom_init_irq();

	prom_printf("\nLINUX started...\n");
}

