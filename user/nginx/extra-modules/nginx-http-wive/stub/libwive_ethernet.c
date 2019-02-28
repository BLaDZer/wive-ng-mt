#include "libwive.h"

static unsigned long rx_count[] = {0,0,0,0,0,0,0,0};
static unsigned long tx_count[] = {0,0,0,0,0,0,0,0};


int portscounts(struct port_counts *st)
{

    for (int i=0;i<6;i++) {
        if (rx_count[i] < 1) {
            rx_count[i] = 1218 + rand()%7919;
            tx_count[i] = 1218 + rand()%7919;
        }
        else
        {
            rx_count[i] += rand()%23;
            tx_count[i] += rand()%23;
        }

        st->rx_count[i] = rx_count[i]*849;
        st->tx_count[i] = tx_count[i]*847;
    }

    return 0;
}

struct nic_counts* nicscounts(int *elem_count)
{

	int res_arr_capacity = 8;
	char* ifaces[] = { "eth0", "eth1", "eth2", "eth3", "eth4", "br0", "ra0", "rai0" };

	(*elem_count) = res_arr_capacity;
	struct nic_counts* pst = calloc(res_arr_capacity,sizeof(struct nic_counts));

	if (pst == NULL)
	{
		syslog(LOG_ERR, "Memory allocation error, %s", __FUNCTION__);
		return NULL;
	}

	for (int i=0;i< res_arr_capacity;i++)
	{
		if (rx_count[i] < 1) {
			rx_count[i] = 1218 + rand()%7919;
			tx_count[i] = 1218 + rand()%7919;
		}
		else
		{
			rx_count[i] += rand()%23;
			tx_count[i] += rand()%23;
		}

		struct nic_counts* nc = &pst[i];
		nc->is_available = 1;
		strcpy(nc->ifname, ifaces[i]);
		nc->rx_packets = rx_count[i];
		nc->rx_bytes = nc->rx_packets * 849;
		nc->rx_errs = nc->rx_drops = nc->rx_fifo = nc->rx_frame = nc->rx_multi = 0;
		nc->tx_errs = nc->tx_drops = nc->tx_fifo = nc->tx_colls = nc->tx_carrier = 0;

		nc->tx_packets = tx_count[i];
		nc->tx_bytes = nc->tx_packets * 847;
	}

	return pst;
}

int portstatus(struct port_status *st, int portnm)
{
	char varname[255];
	snprintf(varname, sizeof(varname), "port%i_swmode", portnm+1);
	char *st_str = nvram_get(RT2860_NVRAM, varname);

	st->link = 1;
	st->speed = SPEED_1000;
	st->duplex = DUPLEX_FULL;
	st->portnum = portnm;

	if (!st_str)
	{
		return 0;
	}

	if (strcmp(st_str, "10h") == 0) {
		st->speed = SPEED_10;
		st->duplex = DUPLEX_HALF;
	} else
	if (strcmp(st_str, "10f") == 0) {
		st->speed = SPEED_10;
		st->duplex = DUPLEX_FULL;
	} else
	if (strcmp(st_str, "100h") == 0) {
		st->speed = SPEED_100;
		st->duplex = DUPLEX_HALF;
	} else
	if (strcmp(st_str, "100f") == 0) {
		st->speed = SPEED_100;
		st->duplex = DUPLEX_FULL;
	} else
	if (strcmp(st_str, "1000h") == 0) {
		st->speed = SPEED_1000;
		st->duplex = DUPLEX_HALF;
	} else
	if (strcmp(st_str, "1000f") == 0) {
		st->speed = SPEED_1000;
		st->duplex = DUPLEX_FULL;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
