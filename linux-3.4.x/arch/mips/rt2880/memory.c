/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     memory setup for Ralink RT2880 solution
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
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <asm/bootinfo.h>
#include <asm/page.h>

#include <asm/rt2880/prom.h>
//#define DEBUG

#ifdef CONFIG_RALINK_RAM_SIZE
#define RAM_SIZE        CONFIG_RALINK_RAM_SIZE*1024*1024	/* Fixed SDRAM SIZE */
#endif

#define MIN_RAM_SIZE  (16*1024*1024)				/* Minimumum SDRAM size */
								/* Maximumum SDRAM size */
#if defined (CONFIG_RALINK_RT2880) || \
    defined (CONFIG_RALINK_RT3352) || \
    defined (CONFIG_RALINK_RT3052) || \
    defined (CONFIG_RALINK_RT5350)
#define MAX_RAM_SIZE  (64*1024*1024)
#define TEST_OFFSET	63
#elif defined (CONFIG_RALINK_RT3883)
#define MAX_RAM_SIZE  (128*1024*1024)
#define TEST_OFFSET	127
#elif defined (CONFIG_RALINK_MT7620) || \
    defined (CONFIG_RALINK_MT7621)
#define MAX_RAM_SIZE  (512*1024*1024)
#define TEST_OFFSET	511
#else
#define MAX_RAM_SIZE  (64*1024*1024)
#define TEST_OFFSET	63
#endif

#define RAM_BASE		0x00000000

#define PFN_ALIGN(x)		(((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)

#ifdef CONFIG_RAM_SIZE_AUTO
unsigned long detect_ram_sequence[3];
static DEFINE_SPINLOCK(testmem_lock);
#endif

enum surfboard_memtypes {
	surfboard_dontuse,
	surfboard_rom,
	surfboard_ram,
};
struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

#ifdef DEBUG
static char *mtypes[3] = {
	"Dont use memory",
	"Used ROM memory",
	"Free RAM memory",
};
#endif

/* References to section boundaries */
extern char _end;

struct prom_pmemblock * __init prom_getmdesc(void)
{
	char *env_str;
	unsigned int ramsize, rambase;

	env_str = prom_getenv("ramsize");
	if (!env_str) {
#ifdef CONFIG_RAM_SIZE_AUTO
		ramsize = MAX_RAM_SIZE;
#else
		ramsize = RAM_SIZE;
#endif
#ifdef DEBUG
		prom_printf("ramsize = %d MBytes\n", ramsize / 1024 / 1024);
#endif
	} else {
#ifdef DEBUG
		prom_printf("ramsize = %s\n", env_str);
#endif
		ramsize = simple_strtol(env_str, NULL, 0);
	}

	env_str = prom_getenv("rambase");
	if (!env_str) {
		rambase = RAM_BASE;
#ifdef DEBUG
		prom_printf("rambase not set, set to default (0x%08X)\n", rambase);
#endif
	} else {
		rambase = simple_strtol(env_str, NULL, 0);
#ifdef DEBUG
		prom_printf("rambase = %s\n", env_str);
#endif
	}

	memset(mdesc, 0, sizeof(mdesc));

	mdesc[0].type = surfboard_ram;
	mdesc[0].base = rambase;
	mdesc[0].size = ramsize;

	return &mdesc[0];
}

void __init prom_meminit(void)
{
#ifdef CONFIG_RAM_SIZE_AUTO
	unsigned long mem, reg_mem;
	unsigned long before, offset;
	unsigned long flags;

	spin_lock_irqsave(&testmem_lock, flags);

	/* Maximum RAM for autodetect */
	reg_mem = MAX_RAM_SIZE >> 20;

	/* Test to be sure in RAM capacity */
	before = ((unsigned long) &prom_init) & (TEST_OFFSET << 20);
	offset = ((unsigned long) &prom_init) - before;

	for (mem = before + (1 << 20); mem < (reg_mem << 20); mem += (1 << 20))
         if (*(unsigned long *)(offset + mem) == *(unsigned long *)(prom_init)) {
    		mem -= before;
		break;
	 }

	/* correct ram size for current CPU type */
	detect_ram_sequence[0] = MAX_RAM_SIZE;
	detect_ram_sequence[1] = mem;

	if (mem > MAX_RAM_SIZE)
	    mem = MAX_RAM_SIZE;

	if (mem < MIN_RAM_SIZE)
	    mem = MIN_RAM_SIZE;

	detect_ram_sequence[2] = mem;

	spin_unlock_irq(&testmem_lock);

	/* Set ram size */
	add_memory_region(RAM_BASE, mem, BOOT_MEM_RAM);
#else	/* Fixed mesize */
        add_memory_region(RAM_BASE, RAM_SIZE, BOOT_MEM_RAM);
#endif	/* CONFIG_RAM_SIZE_AUTO */
}

void __init prom_free_prom_memory(void)
{
#ifdef DEBUG
	/* Nothing to do! Need only for DEBUG.	  */
	/* This is may be corrupt working memory. */

	unsigned long addr;
	int i;

	for (i = 0; i < boot_mem_map.nr_map; i++) {
		if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
			continue;

		addr = boot_mem_map.map[i].addr;
		free_init_pages("prom memory", addr, addr + boot_mem_map.map[i].size);
	}
#endif
}
