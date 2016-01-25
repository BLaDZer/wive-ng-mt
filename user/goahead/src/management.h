#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define PROC_CPU_STATISTIC	"/proc/stat"

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3

void formDefineManagement(void);
#endif
