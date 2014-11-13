#ifndef __SMI_H__
#define __SMI_H__

#include <rtk_types.h>
#include <rtk_error.h>

int32 smi_read(uint32 mAddrs, uint32 *rData);
int32 smi_write(uint32 mAddrs, uint32 rData);

#endif /* __SMI_H__ */
