#ifndef _WIVE_SYSTAT_H
#define _WIVE_SYSTAT_H	1

#define PROC_MEM_STATISTIC	"/proc/meminfo"
#define PROC_CPU_STATISTIC	"/proc/stat"

struct cpu_stats {
	unsigned long long user;    // user (application) usage
	unsigned long long nice;    // user usage with "niced" priority
	unsigned long long system;  // system (kernel) level usage
	unsigned long long idle;    // CPU idle and no disk I/O outstanding
	unsigned long long iowait;  // CPU idle but with outstanding disk I/O
	unsigned long long irq;     // Interrupt requests
	unsigned long long sirq;    // Soft interrupt requests
	unsigned long long steal;   // Invol wait, hypervisor svcing other virtual CPU
	unsigned long long busy;
	unsigned long long total;
};

struct mem_stats {
	unsigned long int total;    // RAM total
	unsigned long int free;     // RAM free
	unsigned long int buffers;  // RAM buffers
	unsigned long int cached;   // RAM cached
	unsigned long int sw_total; // Swap total
	unsigned long int sw_free;  // Swap free
};

void getcpudata(struct cpu_stats *st);
void get_memdata(struct mem_stats *st);
#endif
