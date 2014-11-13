/**********************************************************
*	File:replace.c
*	This file includes a function that is used to replace the
*	values in RF buffer.
*
*	return rc 	=0: replace successful
*						 !=0:	fail

*
**********************************************************/

#include <common.h>
#include "replace.h"

int replace(unsigned long addr, uchar *value, int len)
{
	int idx = 0;
	uchar RFbuf[0x10000];
	int rc=0;
	memset(RFbuf,0,0x10000);
	if (0==len)
	{
		printf("\nThe value to replace is vacant!");
		return -1;
	}

	for (idx =0 ;idx <0x10000;idx ++)
		RFbuf[idx]=(*(uint8_t *) (CFG_FLASH_BASE + 0x40000 + idx));

	for (idx =0 ;idx <len; idx++)
		RFbuf[0x00000 + addr + idx] = value[idx];

	flash_sect_protect(0, CFG_FLASH_BASE + 0x40000, CFG_FLASH_BASE + 0x4ffff);

	printf("\n Erase File System block !!\n From %x To %x\n", CFG_FLASH_BASE + 0x40000, CFG_FLASH_BASE + 0x4ffff);
	flash_sect_erase(CFG_FLASH_BASE + 0x40000, CFG_FLASH_BASE + 0x4ffff);
	rc = flash_write((uchar *)RFbuf, (ulong)(CFG_FLASH_BASE + 0x40000), 0x10000);

	flash_sect_protect(1, CFG_FLASH_BASE + 0x40000, CFG_FLASH_BASE + 0x4ffff);

	printf("\nrc=%d\n",rc);
 	return rc;
}
