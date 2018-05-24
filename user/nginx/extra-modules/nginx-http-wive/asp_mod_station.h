#ifndef __ASP_MOD_STATION_H__
#define __ASP_MOD_STATION_H__

#include "asp_mod_helpers.h"

#define Ndis802_11AuthMode8021x 20

void asp_mod_station_init(void);

void initStaProfile(void);
void initStaConnection(webs_t* wp);
void StartStaConnect(webs_t* wp);
void freeHeaderProfileSettings(void);

#endif //__ASP_MOD_STATION_H__
