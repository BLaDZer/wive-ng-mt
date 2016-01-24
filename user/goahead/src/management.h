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
	unsigned long long user;    // user (application) usage
	unsigned long long nice;    // user usage with "niced" priority
	unsigned long long system;  // system (kernel) level usage
	unsigned long long idle;    // CPU idle and no disk I/O outstanding
	unsigned long long iowait;  // CPU idle but with outstanding disk I/O
	unsigned long long irq;     // Interrupt requests
	unsigned long long softirq; // Soft interrupt requests
	unsigned long long steal;   // Invol wait, hypervisor svcing other virtual CPU
	unsigned long long busy;
	unsigned long long total;
};

union uCpuStats
{
	unsigned long long arrData[8];
	struct cpuStats sepData;
};
#endif
