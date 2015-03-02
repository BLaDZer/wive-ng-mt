/*
*	TFTP server     File: cmd_tftpServer.c
*/

#include <cmd_tftpServer.h>
#include <replace.h>

extern void gpio_init(void);

#ifdef CFG_DIRECT_FLASH_TFTP
extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];/* info for FLASH chips   */
#endif

static int	TftpServerPort;		/* The UDP port at their end		*/
static int	TftpOurPort;		/* The UDP port at our end		*/
static int	TftpTimeoutCount;
static ulong	TftpBlock;		/* packet sequence number		*/
static ulong	TftpLastBlock;		/* last packet sequence number received */
static ulong	TftpBlockWrapOffset;	/* memory offset due to wrapping	*/
static int	TftpState;

uint8_t ptr[PTR_SIZE];
uint16_t RescueAckFlag = 0;
uint32_t copysize = 0;
uint32_t offset = 0;

int rc = 0;
int MAC_FLAG = 0;
int env_loc = 0;

static void _tftpd_open(void);
static void TftpHandler(uchar * pkt, unsigned dest, unsigned src, unsigned len);
static void SolveImage(void);
extern image_header_t header;
extern int do_bootm(cmd_tbl_t *, int, int, char *[]);
extern int do_reset(cmd_tbl_t *, int, int, char *[]);

extern IPaddr_t TempServerIP;

#if (CONFIG_COMMANDS & CFG_CMD_TFTPSERVER)
int check_image(int mode)
{
	ulong addr;
	ulong data, len, checksum;
	int verify;
	char *s;

	image_header_t *hdr = &header;

	s = getenv("verify");
	verify = (s && (*s == 'n')) ? 0 : 1;

	if (mode == 0)
		addr = CFG_FLASH_BASE + 0x50000;
	else
		addr = load_addr;

	/* YJ, 5/16/2006 */
	if (addr == 0x8A200000)
	   ((void(*) (void)) (0x8A200000U))();
	else if(addr == 0x80200000)
	   ((void(*) (void)) (0x80200000U))();
	else if(addr == 0x8A300000)
	   ((void(*) (void)) (0x8A300000U))();
	else if(addr == 0x88001000)
	   ((void(*) (void)) (0x88001000U))();
	else if(addr == 0x8B800000)
	   ((void(*) (void)) (0x8B800000U))();

	printf("## Checking image at %08lx ...\n", addr);

	/* Copy header so we can blank CRC field for re-calculation */
#ifdef CONFIG_HAS_DATAFLASH
	if (addr_dataflash(addr))
	{
		read_dataflash(addr, sizeof(image_header_t), (char *)&header);
	}
	else
#elif defined (CFG_ENV_IS_IN_NAND)
	if (addr >= CFG_FLASH_BASE)
		ranand_read(&header, (char *)(addr - CFG_FLASH_BASE), sizeof(image_header_t));
	else
#elif defined (CFG_ENV_IS_IN_SPI)
	if (addr >= CFG_FLASH_BASE)
		raspi_read(&header, (char *)(addr - CFG_FLASH_BASE), sizeof(image_header_t));
	else
#endif
		memmove(&header, (char *)addr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC)
	{
		printf("Bad Magic Number,%08X \n", ntohl(hdr->ih_magic));
		return 1;
	}

	data = (ulong)&header;
	len  = sizeof(image_header_t);

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;

	if (crc32(0, (char *)data, len) != checksum)
	{
		puts("Bad Header Checksum\n");
		return 1;
	}

	/* for multi-file images we need the data part, too */
	print_image_hdr ((image_header_t *)hdr);

	data = addr + sizeof(image_header_t);
	len  = ntohl(hdr->ih_size);

#ifdef CONFIG_HAS_DATAFLASH
	if (addr_dataflash(addr))
	{
		read_dataflash(data, len, (char *)CFG_LOAD_ADDR);
		data = CFG_LOAD_ADDR;
	}
#elif defined (CFG_ENV_IS_IN_NAND)
	if (addr >= CFG_FLASH_BASE) {
		ulong load_addr = CFG_SPINAND_LOAD_ADDR;
		ranand_read(load_addr, data - CFG_FLASH_BASE, len);
		data = load_addr;
	}
#elif defined (CFG_ENV_IS_IN_SPI)
	if (addr >= CFG_FLASH_BASE) {
		ulong load_addr = CFG_SPINAND_LOAD_ADDR;
		raspi_read(load_addr, data - CFG_FLASH_BASE, len);
		data = load_addr;
	}
#endif

	if (verify)
	{
		puts("   Verifying Checksum ... ");
		if (crc32(0, (char *)data, len) != ntohl(hdr->ih_dcrc)) 
		{
			printf("Bad Data CRC\n");
			return 1;
		}
	}
	return 0;
}

static void TftpdSend(void)
{
	volatile uchar *pkt;
	volatile uchar *xp;
	int	len = 0;
	/*
	*	We will always be sending some sort of packet, so
	*	cobble together the packet headers now.
	*/
	pkt = NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE;

	switch (TftpState)
	{
	case STATE_RRQ:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_DATA);
		*((ushort *)pkt)++ = htons(TftpBlock); /* fullfill the data part */
		len = pkt - xp;
		break;

	case STATE_WRQ:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_ACK);
		*((ushort *)pkt)++ = htons(TftpBlock);
		len = pkt - xp;
		break;

#ifdef ET_DEBUG
		printf("send option \"timeout %s\"\n", (char *)pkt);
#endif
		pkt += strlen((char *)pkt) + 1;
		len = pkt - xp;
		break;

	case STATE_DATA:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_ACK);
		*((ushort *)pkt)++ = htons(TftpBlock);
		len = pkt - xp;
		break;

	case STATE_FINISHACK:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_FINISHACK);
		*((ushort *)pkt)++ = htons(RescueAckFlag);
		len = pkt - xp;
		break;

	case STATE_TOO_LARGE:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_ERROR);
		*((ushort *)pkt)++ = htons(3);
		strcpy((char *)pkt, "File too large");
		pkt += 14 /*strlen("File too large")*/ + 1;
		len = pkt - xp;
		break;

	case STATE_BAD_MAGIC:
		xp = pkt;
		*((ushort *)pkt)++ = htons(TFTP_ERROR);
		*((ushort *)pkt)++ = htons(2);
		strcpy((char *)pkt, "File has bad magic");
		pkt += 18 /*strlen("File has bad magic")*/ + 1;
		len = pkt - xp;
		break;
	}

	NetSendUDPPacket(NetServerEther, NetServerIP, TftpServerPort, TftpOurPort, len);
}

static void TftpTimeout(void)
{
	puts("T ");
	NetSetTimeout(TIMEOUT * CFG_HZ, TftpTimeout);
	TftpdSend();
}

U_BOOT_CMD(
	tftpd, 1, 1, do_tftpd,
	"tftpd\t -load the data by tftp protocol\n",
	NULL
);

int do_tftpd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	gpio_init();

	if(DETECT())
	{
		printf(" \n## Enter Rescue Mode ##\n");
		printf("   \n3: System Boot system code via TFTP.\n");
		setenv("autostart", "no");
		/* Wait forever for an image */
		if (NetLoop(TFTPD) < 0)
			return 1;
	}
	else
	{
		if(check_image(0))
		{
			printf(" \nEnter Recuse Mode for image error\n");
			printf("   \n3: System Boot system code via TFTP.\n");
			if (NetLoop(TFTPD) < 0)
				return 1;
		}

		/* show LED POWER after success image integrity check */
		LEDON();

		printf("   \n3: System Boot system code via Flash.\n");
		do_bootm(cmdtp, 0, argc, argv);
	}
	return 0;
}



void TftpdStart(void)
{

#if defined(CONFIG_NET_MULTI)
	printf("Using %s device\n", eth_get_name());
#endif
	puts("\nOur IP address is:(");
	print_IPaddr(NetOurIP);
	puts(")\nWait for TFTP request...\n");

	/* Check if we need to send across this subnet */
	if (NetOurGatewayIP && NetOurSubnetMask)
	{
		IPaddr_t OurNet 	= NetOurIP    & NetOurSubnetMask;
		IPaddr_t ServerNet 	= NetServerIP & NetOurSubnetMask;

		if (OurNet != ServerNet)
		{
			puts("; sending through gateway ");
			print_IPaddr(NetOurGatewayIP) ;
		}
	}

	memset(ptr,0,sizeof(ptr));
	_tftpd_open();
}

static void _tftpd_open()
{
	NetSetTimeout(TIMEOUT * CFG_HZ * 2, TftpTimeout);
    	NetSetHandler(TftpHandler);

	TftpOurPort = PORT_TFTP;
	TftpTimeoutCount = 0;
	TftpState = STATE_RRQ;
	TftpBlock = 0;

	/* zero out server ether in case the server ip has changed */
	memset(NetServerEther, 0, 6);
}


static void TftpHandler(uchar * pkt, unsigned dest, unsigned src, unsigned len)
{
	ushort proto;
	int i;

	if (dest != TftpOurPort)
	{
		return;
	}
	/* don't care the packets that donot send to TFTP port */

	if (TftpState != STATE_RRQ && src != TftpServerPort)
	{
		return;
	}

	if (len < 2)
	{
		return;
	}

	len -= 2;
	/* warning: don't use increment (++) in ntohs() macros!! */
	proto = *((ushort *)pkt)++;

	switch (ntohs(proto))
	{
	case TFTP_RRQ:

		printf("\n Get read request from:(");
		print_IPaddr(TempServerIP);
		printf(")\n");
		NetCopyIP(&NetServerIP,&TempServerIP);
		TftpServerPort = src;
		TftpBlock = 1;
		TftpBlockWrapOffset = 0;
		TftpState = STATE_RRQ;
		TftpdSend();//send a vacant Data packet as a ACK
		break;

	case TFTP_WRQ:
		TftpServerPort = src;
		TftpBlock = 0;
		TftpState = STATE_WRQ;
		TftpdSend();
		break;

	case TFTP_DATA:
		if (len < 2)
			return;
		len -= 2;
		TftpBlock = ntohs(*(ushort *)pkt);
		/*
		* RFC1350 specifies that the first data packet will
		* have sequence number 1. If we receive a sequence
		* number of 0 this means that there was a wrap
		* around of the (16 bit) counter.
		*/
		if (TftpBlock == 0)
		{
			printf("\n\t %lu MB reveived\n\t ", TftpBlockWrapOffset>>20);
		}
		else
		{
			if (((TftpBlock - 1) % 10) == 0)
			{/* print out progress bar */
				puts("#");
			}
			else
				if ((TftpBlock % (10 * HASHES_PER_LINE)) == 0)
				{
					puts("\n");
				}
		}
		if (TftpState == STATE_WRQ)
		{
			/* first block received */
			TftpState = STATE_DATA;
			TftpServerPort = src;
			TftpLastBlock = 0;
			TftpBlockWrapOffset = 0;

			if (TftpBlock != 1)
			{	/* Assertion */
				printf("\nTFTP error: "
					"First block is not block 1 (%ld)\n"
					"Starting again\n\n",
					TftpBlock);
				NetStartAgain();
				break;
			}
		}

		if (TftpBlock == TftpLastBlock)
		{	/* Same block again; ignore it. */
			printf("\n Same block again; ignore it \n"); 
			break;
		}
		TftpLastBlock = TftpBlock;
		NetSetTimeout(TIMEOUT * CFG_HZ, TftpTimeout);

      		offset = (TftpBlock - 1) * TFTP_BLOCK_SIZE;
		copysize = offset + len; /* the total length of the data */

		if(copysize > PTR_SIZE)
		{
			printf("!!OVER PTR SIZE\n");
		}

		(void)memcpy((void *)(ptr+ offset), pkt+2, len);/* store the data part to RAM */

		/*
		*	Acknowledge the block just received, which will prompt
		*	the Server for the next one.
		*/
		TftpdSend();

		if (len < TFTP_BLOCK_SIZE)
		{
		/*
		*	We received the whole thing.  Try to run it.
		*/
			puts("\ndone\n");
			TftpState = STATE_FINISHACK;
			NetState = NETLOOP_SUCCESS;
			printf("Check image and write it to FLASH \n");
            		SolveImage();
		}
		break;

	case TFTP_ERROR:
		printf("\nTFTP error: '%s' (%d)\n",
			pkt + 2, ntohs(*(ushort *)pkt));
		puts("Starting again\n\n");
		NetStartAgain();
		break;

	default:
		break;

	}
}

static void SolveImage(void)
{
	int  i = 0;
	int rrc = 0;
	ulong	dest = 0, count = 0;
	int	size;

    	load_addr = ptr;
	if (check_image(1))
	{
		printf("Check image error! SYSTEM RESET!!!\n\n");
		udelay(500);
		do_reset (NULL, 0, 0, NULL);
	}
	else
	{
		printf("\n Copy linux image[%d byte] to Flash[0x%08X].... \n", copysize, dest);
		count = copysize;
		size = 1;

		if (count == 0)
		{
			puts ("Zero length ???\n");
			return;
		}

		puts ("Copy to Flash... ");
#if defined (CFG_ENV_IS_IN_NAND)
		printf ("\n Copy %d byte to Flash... \n", count*size);
		rrc = ranand_erase_write((uchar *)ptr, CFG_KERN_ADDR-CFG_FLASH_BASE, count*size);
#elif defined (CFG_ENV_IS_IN_SPI)
		printf ("\n Copy %d byte to Flash... \n", count*size);
		rrc = raspi_erase_write((uchar *)ptr,  CFG_KERN_ADDR-CFG_FLASH_BASE, count*size);
#else /* NOR */
		printf("\n Erase File System block !!\n From 0xBC450000 To 0xBC7FFFFF\n");
		flash_sect_protect(0, CFG_FLASH_BASE + 0x50000, CFG_FLASH_BASE + 0x7fffff);
		flash_sect_erase(CFG_FLASH_BASE + 0x50000, CFG_FLASH_BASE + 0x7FFFFF);
		printf ("\n Copy %d byte to Flash... ", count*size);
		rrc = flash_write((uchar *)ptr, (ulong)(CFG_FLASH_BASE + 0x50000), count*size);
		flash_sect_protect(1, CFG_FLASH_BASE + 0x50000, CFG_FLASH_BASE + 0x7fffff);
#endif

		if (rrc)
		{
			printf("rescue failed!\n");
#if !defined (CFG_ENV_IS_IN_NAND) && ! defined (CFG_ENV_IS_IN_SPI)
			flash_perror(rrc);
#endif
			NetState = NETLOOP_FAIL;
			TftpState = STATE_FINISHACK;
			RescueAckFlag= 0x0000;
			for (i=0; i<6; i++)
				TftpdSend();
			return;
		}
		else
		{
			printf("done. %d bytes written\n", count*size);
			TftpState = STATE_FINISHACK;
			RescueAckFlag= 0x0001;
			for (i=0; i<6; i++)
				TftpdSend();
			printf("\nSYSTEM RESET!!!\n\n");
			udelay(500);
			do_reset(NULL, 0, 0, NULL);
			return;
		}
	}
}

#endif
