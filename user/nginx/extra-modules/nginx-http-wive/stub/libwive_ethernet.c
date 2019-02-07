#include "libwive.h"

int portscounts(struct port_counts *st)
{
    st->rx_count[0] = st->rx_count[1] = st->rx_count[2] = st->rx_count[3] = st->rx_count[4] = st->rx_count[5] = 0;
    st->tx_count[0] = st->tx_count[1] = st->tx_count[2] = st->tx_count[3] = st->tx_count[4] = st->tx_count[5] = 0;

    return 0;
}

struct nic_counts* nicscounts(int *elem_count)
{
    (*elem_count) = 0;
    return NULL;
}

int portstatus(struct port_status *st, int portnm)
{
	char varname[255];
	snprintf(varname, sizeof(varname), "port%i_swmode", portnm);
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
