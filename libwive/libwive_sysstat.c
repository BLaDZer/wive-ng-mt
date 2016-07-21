/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

void getcpudata(struct cpu_stats *st)
{
	FILE *fp;
	char line_buf[256];

	st->user = st->nice = st->system = st->idle = st->iowait = st->irq = st->sirq = st->steal = st->busy = st->total = 0;

	fp = fopen(PROC_CPU_STATISTIC, "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return;
	}

	if ((fgets(line_buf, sizeof(line_buf), fp)) != NULL) {
		if (sscanf(line_buf, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
			&st->user, &st->nice, &st->system, &st->idle, &st->iowait, &st->irq, &st->sirq, &st->steal) >= 4) {
			/* calculate busy/total */
			st->busy = st->user + st->nice + st->system + st->irq + st->sirq + st->steal + st->iowait;
			st->total = st->busy + st->idle;
		}
	}
	fclose(fp);
}

void get_memdata(struct mem_stats *st)
{
	FILE *fp;
	char line_buf[64];

	st->total = st->free = st->buffers = st->cached = st->sw_total = st->sw_free = 0;

	fp = fopen("/proc/meminfo", "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return;
	}

	if ((fgets(line_buf, sizeof(line_buf), fp) != NULL) && sscanf(line_buf, "MemTotal: %lu %*s", &st->total) == 1) {
		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "MemFree: %lu %*s", &st->free);

		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "Buffers: %lu %*s", &st->buffers);

		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "Cached: %lu %*s", &st->cached);
	}
	fclose(fp);
}
