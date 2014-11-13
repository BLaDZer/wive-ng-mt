/* Include defines */
#include "ralink-flash.h"

/* this is mapping flash definition */
#if defined (CONFIG_RT2880_FLASH_32M)
static struct mtd_partition rt2880_partitions[] __maybe_unused = {
        {
                name:           "Bootloader",  /* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,  /* 192K */
                offset:         0,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
        }, {
                name:           "Kernel",
                size:           CONFIG_MTD_KERNEL_PART_SIZ,
                offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS",
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#else
        }, {
                name:           "Kernel",
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
        }, {
                name:           "Config",
                size:           0x20000,  /* 128K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",
                size:           0x20000,  /* 128K */
                offset:         MTDPART_OFS_APPEND
	}, {
                name:           "RW-FS", /* mtdblock4 */
                size:           MTD_RWFS_PART_SIZE, /* 192K */
                offset:         MTD_RWFS_PART_OFFSET,
	}
};
#else /* not 32M flash */
static struct mtd_partition rt2880_partitions[] __maybe_unused = {
        {
                name:           "Bootloader",	/* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,	/* 192K */
                offset:         0,
        }, {
                name:           "Config",	/* mtdblock1 */
                size:           MTD_CONFIG_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",	/* mtdblock2 */
                size:           MTD_FACTORY_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING)
        }, {
                name:           "Kernel_stub", /* mtdblock3 */
                size:           0,
		offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS_stub", /* mtdblock3 */
                size:           MTD_ROOTFS_PART_SIZE,
		offset:         MTDPART_OFS_APPEND,
#else
        }, {
                name:           "Kernel", 	/* mtdblock3 */
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#if  defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && ! defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING)
        }, {
                name:           "RootFS", 	/* mtdblock4 */
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
#endif
	}, {
                name:           "RW-FS",	/* mtdblock5 */
                size:           MTD_RWFS_PART_SIZE,	/* 128K */
                offset:         MTD_RWFS_PART_OFFSET,
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING)
        }, {	// Only for update firmware in not padded mode
                name:           "Kernel_RootFS", /* mtdblock6 */
                size:           UPD_RWFS_PART_SIZE,
		offset:         UPD_RWFS_PART_OFFSET,
#endif
	}
};
#endif
