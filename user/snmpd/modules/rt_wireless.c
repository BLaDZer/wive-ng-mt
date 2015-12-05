#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<sys/socket.h>
#include        <sys/param.h>
#include        <sys/types.h>
#include        <sys/time.h>
#include	<sys/ioctl.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<net/route.h>
#include	<string.h>
#include	<dirent.h>
#include        <stdio.h>
#include        <netdb.h>
#include        <unistd.h>
#include        <sys/file.h>

#include	<linux/autoconf.h>
#include	<linux/wireless.h>

#include	<net/route.h>
#include	<arpa/inet.h>

#include        "ctypes.h"
#include        "local.h"
#include        "debug.h"
#include        "miv.h"
#include        "asn.h"
#include        "mix.h"
#include        "systm.h"
#include        "src/oid.h"


#define WIRELESS_DEVICE	"ra0"
#define IFNAMESIZE	sizeof(WIRELESS_DEVICE)

#if 0
static	MixStatusType ssidRelease(MixCookieType mix)
{
	mix = mix;
	return (smpErrorGeneric);
}

static	MixStatusType ssidCreate(MixCookieType mix, MixNamePtrType name, MixLengthType namelen, AsnIdType value)
{
	mix = mix;
	name = name;
	namelen = namelen;
	value = value;
	return (smpErrorGeneric);
}

static MixStatusType ssidDestroy(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	return (smpErrorGeneric);
}

static MixStatusType ssidNext(MixCookieType cookie, MixNamePtrType name, MixLengthPtrType namelenp)
{
	return (smpErrorGeneric);
}

static MixStatusType ssidGet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	return (smpErrorGeneric);
}

static MixStatusType ssidSet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen, AsnIdType asn)
{
	return (smpErrorGeneric);
}
#endif

int test_wireless()
{
	struct iwreq iwr;
	int	skfd;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stderr, "test_wireless()- open socket error: %s\n", WIRELESS_DEVICE);
		return -1;
	}
	strncpy(iwr.ifr_name, WIRELESS_DEVICE, IFNAMESIZE);

	if (ioctl(skfd, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0)
	{
		fprintf(stderr, "test_wireless()- ioctl() error: %s\n", WIRELESS_DEVICE);
		return -1;
	} else
		printf("RTPRIV_IOCTL_GET_MAC_TABLE... ok\n");
    return 0;
}

#if 0
static  MixOpsType ra_ssid_Ops = {
	ssidRelease,
	ssidCreate ,
	ssidDestroy,
	ssidNext,
	ssidGet,
	ssidSet
};
#endif

CVoidType wirelessInit()
{
	/* ssid name */
	test_wireless();
}
