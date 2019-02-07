#include "libwive.h"

int getCPUData(struct cpu_stats *st)
{
    st->user = st->nice = st->system = st->idle = st->iowait = st->irq = st->sirq = st->steal = st->busy = st->total = 0;
    return 0;
}

int getMemData(struct mem_stats *st)
{
    st->total = 256*1024;
    st->free = 200*1024;
    st->buffers = 1*1024;
    st->cached = 8*1024;
    st->sw_total = 1024;
    st->sw_free = 1024;
    return 0;
}

void getSyslogTail(char* log, int maxlen)
{
    snprintf(log, maxlen, "{ Disabled in DEMO Mode }");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
