#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#define PROC_IF_STATISTIC	"/proc/net/dev"
#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define PROC_CPU_STATISTIC	"/proc/stat"

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3

void formDefineManagement(void);
void management_init(void);
char *setNthValueLong(int index, char *old_values, char *new_value);

struct cpuStats
{
	unsigned long int user;    // 0 user (application) usage
	unsigned long int nice;    // 1 user usage with "niced" priority
	unsigned long int system;  // 2 system (kernel) level usage
	unsigned long int idle;    // 3 CPU idle and no disk I/O outstanding
	unsigned long int iowait;  // 4 CPU idle but with outstanding disk I/O
	unsigned long int irq;     // 5 Interrupt requests
	unsigned long int softirq; // 6 Soft interrupt requests
	unsigned long int steal;   // 7 Invol wait, hypervisor svcing other virtual CPU
};

union uCpuStats
{
	unsigned long int arrData[8];
	struct cpuStats sepData;
};
#endif
