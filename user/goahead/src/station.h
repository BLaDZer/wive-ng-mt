/* vi: set sw=4 ts=4 sts=4: */
/*
 * station.h -- Station Mode
 *
 * Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 * $Id: station.h,v 1.3 2007-08-02 09:23:29 yy Exp $
 */

#define Ndis802_11AuthMode8021x 20

void formDefineStation(void);
void initStaProfile(void);
void initStaConnection(void);
void freeHeaderProfileSettings(void);

int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
