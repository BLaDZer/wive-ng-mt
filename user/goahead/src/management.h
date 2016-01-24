#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define PROC_CPU_STATISTIC	"/proc/stat"

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3

void formDefineManagement(void);
void management_init(void);
char *setNthValueLong(int index, char *old_values, char *new_value);

struct cpuStats {
	unsigned int	user;    // user (application) usage
	unsigned int	nice;    // user usage with "niced" priority
	unsigned int	system;  // system (kernel) level usage
	unsigned int	idle;    // CPU idle and no disk I/O outstanding
	unsigned int	iowait;  // CPU idle but with outstanding disk I/O
	unsigned int	irq;     // Interrupt requests
	unsigned int	softirq; // Soft interrupt requests
	unsigned int	steal;   // Invol wait, hypervisor svcing other virtual CPU
	unsigned int	total;
};

union uCpuStats
{
	unsigned int arrData[8];
	struct cpuStats sepData;
};
#endif
