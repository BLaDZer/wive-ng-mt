/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/***********************************************************************
*
* cdp-send.c
*
* program to send CDPv1 announcements and MNDP (Mikrotik) for Linux.
*
* Copyright (C) 2011 by Alexei Takaseev <alexei@taf.ru>.
*
***********************************************************************/

#include <ctype.h> 
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <linux/unistd.h>


#define PORT 5678
#define DEST_ADDR "255.255.255.255"

#define LIBNET_LIL_ENDIAN 1

static struct utsname	myuname;
static int		debug=0;


static char	*use_sysname = 0,
		*use_machine = 0,
		*use_hostname = 0;

struct mndp_interface
{
    struct mndp_interface*	next;
    char*			name;
    struct sockaddr_in		address;
    int				index;
    unsigned char		eaddr[6];
};



struct mndp_interface* mndp_interface_find(struct mndp_interface* list, char* iface)
{
    while (list)
    {
	if (list->name && !strcmp(list->name,iface))
	    return list;

	list=list->next;
    };
    return NULL;
};


struct mndp_interface* mndp_interface_add(struct mndp_interface** head, char* iface)
{
    struct ifreq ifr;
    struct mndp_interface* mndp;
    int fd;

    if (!iface || !head)
        return NULL;

    if ((mndp=mndp_interface_find(*head, iface)))
        return mndp;

    mndp = malloc(sizeof(struct mndp_interface));
    if (!mndp)
    {
	perror("malloc");
	exit(1);
    };
    memset(mndp,0,sizeof(struct mndp_interface));

    mndp->name = iface;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    {
	perror("SIOCGIFHWADDR");
	free(mndp);
	return NULL;
    }
    memcpy(mndp->eaddr, ifr.ifr_hwaddr.sa_data, 6);

    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    {
	mndp->address.sin_addr.s_addr=inet_addr("0.0.0.0");
    }
    mndp->address.sin_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
    {
	perror("SIOCGIFINDEX");
	free(mndp);
	return NULL;
    }
    mndp->index = ifr.ifr_ifindex;
    close(fd);

    if(!*head)
    { 
	*head=mndp;
    }
    else
    { 
	struct mndp_interface* b=*head;
	while(b->next)
	    b=b->next;
	b->next=mndp;
    };
    return mndp;
};

int sx_write_long(unsigned char* buffer, u_int32_t data)
{ 
#ifdef LIBNET_LIL_ENDIAN
	buffer[3]=(data>>24)&0xff;
	buffer[2]=(data>>16)&0xff;
	buffer[1]=(data>>8)&0xff;
	buffer[0]=data&0xff;
#else
	buffer[0]=(data>>24)&0xff;
	buffer[1]=(data>>16)&0xff;
	buffer[2]=(data>>8)&0xff;
	buffer[3]=data&0xff;
#endif
	return 1;
};

int sx_write_short(unsigned char* buffer, u_int16_t data)
{ 
#ifdef LIBNET_LIL_ENDIAN
	buffer[1]=(data>>8)&0xff;
	buffer[0]=data&0xff;
#else
	buffer[0]=(data>>8)&0xff;
	buffer[1]=data&0xff;
#endif
	return 1;
};


int mndp_buffer_init(unsigned char* buffer, int len, unsigned char* myether, u_int p_count)
{ 
    memset(buffer,0,len);

    sx_write_long(buffer,htonl(p_count));
    sx_write_short(buffer+4,htons(0x0001)); 
    sx_write_short(buffer+6,htons(0x0006)); 

    memcpy(buffer+8,myether,6);

    return 14;
};

int mndp_add_device_id(unsigned char* buffer, int len)
{ 
    static char s_hostname[256];

    if (!use_hostname)
    {
	gethostname(s_hostname,sizeof(s_hostname)-1);
	s_hostname[sizeof(s_hostname)-1] = 0;
	use_hostname = s_hostname;
    }

    if ((strlen(use_hostname)+4)>len)
	return 0;

    sx_write_short(buffer,htons(0x0005));
    sx_write_short(buffer+2,htons(strlen(use_hostname)));
    memcpy(buffer+4,use_hostname,strlen(use_hostname));

    return strlen(use_hostname)+4;
};


int mndp_add_software_version(unsigned char* buffer, int len)
{ 
	if ((strlen(use_sysname)+4)>len)
	    return 0;

	sx_write_short(buffer,htons(0x0007));
	sx_write_short(buffer+2,htons(strlen(use_sysname)));
	memcpy(buffer+4,use_sysname,strlen(use_sysname));

	return strlen(use_sysname)+4;
};

int mndp_add_platform(unsigned char* buffer, int len)
{
    if((strlen(use_machine)+4)>len)
	return 0;
    sx_write_short(buffer,htons(0x0008));
    sx_write_short(buffer+2,htons(strlen(use_machine)));
    memcpy(buffer+4,use_machine,strlen(use_machine));

    return strlen(use_machine)+4;
};

int mndp_add_uptime(unsigned char* buffer, int len)
{
    struct sysinfo s_info;
    int error;

    if(len < 8)
	return 0;
    sx_write_short(buffer,htons(0x000a));
    sx_write_short(buffer+2,htons(0x0004));

    error = sysinfo(&s_info);
    if(error != 0)
    {
	return 0;
    }

    sx_write_long(buffer+4,htonl(s_info.uptime));

    return 8;
};

int cdp_buffer_init(unsigned char* buffer, int len, unsigned char* myether)
{ 
    memset(buffer,0,len);

    buffer[0]=0x01;
    buffer[1]=0x00;
    buffer[2]=0x0c;
    buffer[3]=buffer[4]=buffer[5]=0xcc; 

    memcpy(buffer+6,myether,6);

    buffer[12]=buffer[13]=0x00; /* lenight - will calculate later */

    buffer[14]=buffer[15]=0xaa;
    buffer[16]=0x03;
    buffer[17]=buffer[18]=0x00;
    buffer[19]=0x0c;
    sx_write_short(buffer+20,htons(0x2000));

    buffer[22]=0x1; /* cdp version */
    buffer[23]=0xb4; /* cdp holdtime, 180 sec by default */
    buffer[24]=buffer[25]=0; /* checksum - will calculate later */

    return 26;
};

int cdp_add_device_id(unsigned char* buffer, int len)
{ 
    static char s_hostname[256];

    if (!use_hostname)
    {
	gethostname(s_hostname,sizeof(s_hostname)-1);
	s_hostname[sizeof(s_hostname)-1] = 0;
	use_hostname = s_hostname;
    }

    if ((strlen(use_hostname)+4)>len)
	return 0;

    *(u_int16_t*)buffer = htons(0x0001); /* type=deviceId */
    *((u_int16_t*)(buffer+2)) = htons(strlen(use_hostname)+4); /* total length */
    memcpy(buffer+4,use_hostname,strlen(use_hostname));

    return strlen(use_hostname)+4;
};

int cdp_add_interface(unsigned char* buffer, int len, char* interface)
{
    if (!interface)
	return 0;
    if (len<(strlen(interface)+4))
	return 0;

    sx_write_short(buffer,htons(0x0003)); /* type=PortId */
    sx_write_short(buffer+2,htons(strlen(interface)+4)); /* totallength*/
    memcpy(buffer+4,interface,strlen(interface));

    return strlen(interface)+4;
};

int cdp_add_capabilities(unsigned char* buffer, int len)
{
    if(len<8)
	return 0;

    sx_write_short(buffer,htons(0x0004)); /* type=Capabilities */
    sx_write_short(buffer+2,htons(8)); /* totallength*/
    sx_write_long(buffer+4,htonl(0x1));

    return 8;
};

int cdp_add_software_version(unsigned char* buffer, int len)
{
    if((strlen(use_sysname)+4)>len)
	return 0;

    sx_write_short(buffer,htons(0x0005)); /* type=software version */
    sx_write_short(buffer+2,htons(strlen(use_sysname)+4)); /* totallength*/
    memcpy(buffer+4,use_sysname,strlen(use_sysname));

    return strlen(use_sysname)+4;
};

int cdp_add_platform(unsigned char* buffer, int len)
{ 
    if((strlen(use_machine)+4)>len)
	return 0;
    sx_write_short(buffer,htons(0x0006)); /* type=platform */
    sx_write_short(buffer+2,htons(strlen(use_machine)+4)); /* totallength*/
    memcpy(buffer+4,use_machine,strlen(use_machine));

    return strlen(use_machine)+4;
};


u_int16_t cdp_checksum(const u_char *cp, int len)
{
    unsigned int sum = 0, v = 0;
    int oddbyte = 0;

    /* We compute in network byte order */
    while ((len -= 2) >= 0)
    {
	sum += *cp++ << 8;
        sum += *cp++;
    }
    if ((oddbyte = len & 1) != 0)
	v = *cp;

    /* The remaining byte seems to be handled oddly by Cisco. Any hint about
     * this is welcome. */
    if (oddbyte)
        sum += v;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;
    sum = ntohs(sum);
    return (0xffff & ~sum);
}


int usage()
{
	fprintf(stderr,
"Usage: mndp-send [options] interfaces... \n"
"  -d         debug mode\n"
"  -m mach    specify machine/platform to advertise (e.g. \"%s\")\n"
"  -n name    specify a hostname\n"
"  -o         enable oneshot mode\n"
"  -s vers    specify software/version to advertise (e.g. \"%s %s\")\n"
"  -t secs    set wait-time (default: 60 seconds)\n",
		myuname.machine,
		myuname.sysname, myuname.release
	);
	return 0;
};


void hangup(int s)
{
    exit(0);
}

/* debug only
void print_buff (unsigned char* buffer, int offset) {
    int i;
    for (i=0; i<offset; i++)
    {
	printf("%0*x ", 2,buffer[i]);
    }
    printf("\n");
}
*/

int main (int argc, char* argv[])
{
    static unsigned char	buffer[ETH_FRAME_LEN];

    int		c,i,offset,sockfd;
    int		once=0;
    int		timeout=60;
    int		broadcast=1;
    u_int	p_count=0;

    struct mndp_interface	*ifaces=NULL;
    struct sockaddr_in		sendaddr;
    struct sockaddr_in		recvaddr;
    struct sockaddr_ll		socket_address;


    uname(&myuname);
    while((c=getopt(argc,argv,"odm:n:s:t:"))!=EOF)
    {
	switch(c)
	{
	    case 'd':
		debug++;
		break;
	    case 'm':
		use_machine = strdup(optarg);
		if (!use_machine)
		{
		    perror("strdup");
		    exit(1);
		}
		break;
	    case 'n':
		use_hostname = strdup(optarg);
		if (!use_hostname)
		{
		    perror("strdup");
		    exit(1);
		}
		break;
	    case 'o':
		once=1;
		break;
	    case 's':
		use_sysname = strdup(optarg);
		if (!use_sysname)
		{
		    perror("strdup");
		    exit(1);
		}
		break;
	    case 't':
		timeout = atoi(optarg);
		if(timeout <= 0)
		{
		    fprintf(stderr, "wrong value to timeout - reverting to default 60 sec\n");
		    timeout = 60;
		};
		break;
	    default:
		usage();
		exit(1);
	};
    };

    if (optind == argc)
    {
	usage();
	exit(1);
    }

    for (i = optind; i < argc; i++)
    {
	if (mndp_interface_add(&ifaces, argv[i]) == NULL)
	{
	    fprintf(stderr, "No match interface\n");
	    exit(1);
	}
    }

    if (!use_machine)
    {
	use_machine = myuname.machine;
	if (!use_machine)
	{
	    use_machine = "(null)";
	}
    }

    if (!use_sysname)
    {
	char *qa, *qb;
	int buflen;
	qa = myuname.sysname; if (!qa) qa = "(null)";
	qb = myuname.release; if (!qb) qb = "(null)";
	buflen = strlen(qa) + strlen(qb) + 2;

	use_sysname = (char *)malloc(buflen);
	if (!use_sysname)
	{
	    perror("malloc");
	    exit(1);
	}
	sprintf(use_sysname, "%s %s", qa, qb);
	use_sysname[buflen-1] = '\0'; /* terminate if truncated */
    }

    signal(SIGHUP, hangup);
    signal(SIGINT, hangup);
    signal(SIGQUIT, hangup);
    signal(SIGTERM, hangup);

    if (!once && !debug)
    {
        daemon(1, 0);
    }

    for (;;)
    {
	struct mndp_interface* cifa=ifaces;
	while(cifa)
	{
	    // MNDP packet
	    offset=0;
	    offset=mndp_buffer_init(buffer,sizeof(buffer),cifa->eaddr, p_count);
	    offset+=mndp_add_device_id(buffer+offset,sizeof(buffer)-offset);
	    offset+=mndp_add_software_version(buffer+offset,sizeof(buffer)-offset);
	    offset+=mndp_add_platform(buffer+offset,sizeof(buffer)-offset);
	    offset+=mndp_add_uptime(buffer+offset,sizeof(buffer)-offset);

    	    if((sockfd = socket(PF_INET,SOCK_DGRAM,0)) == -1)
    	    {
                perror("sockfd");
                exit(1);
    	    }

    	    if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST, &broadcast,sizeof broadcast)) == -1)
    	    {
                perror("setsockopt - SO_SOCKET ");
                exit(1);
    	    }


    	    sendaddr.sin_family = AF_INET;
    	    sendaddr.sin_port = htons(PORT);
    	    sendaddr.sin_addr.s_addr = cifa->address.sin_addr.s_addr;
	    memset(sendaddr.sin_zero,'\0',sizeof sendaddr.sin_zero);

    	    if(bind(sockfd, (struct sockaddr*) &sendaddr, sizeof sendaddr) == -1)
    	    {
                perror("bind");
                exit(1);
    	    }

    	    recvaddr.sin_family = AF_INET;
    	    recvaddr.sin_port = htons(PORT);
    	    recvaddr.sin_addr.s_addr = inet_addr(DEST_ADDR);
    	    memset(recvaddr.sin_zero,'\0',sizeof recvaddr.sin_zero);

    	    sendto(sockfd, buffer, offset , 0, (struct sockaddr *)&recvaddr, sizeof recvaddr);
    	    close(sockfd);

	    // CDPv1 packet
	    offset=0;
	    offset=cdp_buffer_init(buffer,sizeof(buffer),cifa->eaddr);
	    offset+=cdp_add_device_id(buffer+offset,sizeof(buffer)-offset);
	    offset+=cdp_add_interface(buffer+offset,sizeof(buffer)-offset, cifa->name);
	    offset+=cdp_add_capabilities(buffer+offset,sizeof(buffer)-offset);
	    offset+=cdp_add_software_version(buffer+offset,sizeof(buffer)-offset);
	    offset+=cdp_add_platform(buffer+offset,sizeof(buffer)-offset);

	    // len packet
	    *(u_int16_t*)(buffer+12)=htons(offset-14);
	    // checksum
	    *(u_short*)(buffer+24)=cdp_checksum(buffer+22,offset-22);
//print_buff(buffer, offset);

	    socket_address.sll_family   = PF_PACKET;
	    socket_address.sll_protocol = htons(ETH_P_IP);
	    socket_address.sll_ifindex  = cifa->index;
	    socket_address.sll_hatype   = ARPHRD_ETHER;
	    socket_address.sll_pkttype  = PACKET_OTHERHOST;
	    socket_address.sll_halen    = ETH_ALEN;

	    socket_address.sll_addr[0] = buffer[0];
	    socket_address.sll_addr[1] = buffer[1];
	    socket_address.sll_addr[2] = buffer[2];
	    socket_address.sll_addr[3] = buffer[3];
	    socket_address.sll_addr[4] = buffer[4];
	    socket_address.sll_addr[5] = buffer[5];
	    socket_address.sll_addr[6] = socket_address.sll_addr[7] = 0;

	    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1)
	    {
		perror("socket");
		exit(1);
	    }
	    sendto(sockfd, buffer, offset, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
    	    close(sockfd);

	    p_count++;
	    cifa=cifa->next;
	}
	if (once)
	    return 0;
	sleep(timeout);
    }
}
