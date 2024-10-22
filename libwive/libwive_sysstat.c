/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]wi-cat.ru
*/

#include "libwive.h"

int getCPUData(struct cpu_stats *st)
{
	FILE *fp;
	char line_buf[256];

	st->user = st->nice = st->system = st->idle = st->iowait = st->irq = st->sirq = st->steal = st->busy = st->total = 0;

	fp = fopen(PROC_CPU_STATISTIC, "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return 1;
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

        return 0;
}

int getMemData(struct mem_stats *st)
{
	FILE *fp;
	char line_buf[64];

	st->total = st->free = st->buffers = st->cached = st->sw_total = st->sw_free = 0;

	fp = fopen("/proc/meminfo", "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return 1;
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

        return 0;
}

/* getSyslogTail -  Write the tail of syslog into variable
 *
 * arg: (out) log - initialized char array to store
 * arg: maxlen - output variable byte size. Effective log character count would be maxlen-1.
 */
void getSyslogTail(char* log, int maxlen)
{

	FILE *fp = NULL;

	char cmd[64];
	snprintf((char*)&cmd,maxlen,"tail -c %i /var/log/messages",maxlen-1);

	fp = popen(cmd, "r");
	if(!fp){
		syslog(LOG_ERR, "no log exist, %s", __FUNCTION__);
		goto error;
	}

	memset(log, 0, maxlen);
	fread(log, 1, maxlen, fp);

error:
	if(fp)
	    pclose(fp);


}
