/*
 * uemf.h -- GoAhead Micro Embedded Management Framework Header
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: uemf.h,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

#ifndef _h_UEMF
#define _h_UEMF 1

/******************************** Description *********************************/

/* 
 *	GoAhead Web Server header. This defines the Web public APIs
 */

/******************************* Per O/S Includes *****************************/

	#include	<stdio.h>
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<sys/param.h>
	#include	<limits.h>
	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<unistd.h>
	#include	<sys/socket.h>
	#include	<sys/select.h>
	#include	<netinet/in.h>
	#include        <netinet/tcp.h>
	#include 	<arpa/inet.h>
	#include 	<netdb.h>
	#include	<time.h>
	#include	<fcntl.h>
	#include	<errno.h>

/********************************** Includes **********************************/

#include	<ctype.h>
#include	<stdarg.h>
#include	<string.h>

#ifndef WEBS
#include	"messages.h"
#endif /* ! WEBS */

/******************************* Per O/S Defines *****************************/

#ifndef O_BINARY
#define O_BINARY 	0
#endif /* O_BINARY */
#define	SOCKET_ERROR	-1


/********************************** Unicode ***********************************/
/*
 *	Constants and limits. Also FNAMESIZE and PATHSIZE are currently defined 
 *	in param.h to be 128 and 512
 */
#define TRACE_MAX		(4096 - 48)
#define VALUE_MAX_STRING	(4096 - 48)
#define SYM_MAX			(512)
#define XML_MAX			4096			/* Maximum size for tags/tokens */
#define BUF_MAX			4096			/* General sanity check for bufs */

#define	T(s) 			s
typedef char			char_t;
#define	TSZ(x)			(sizeof(x))
#define	TASTRL(x)		(strlen(x) + 1)

/*
 *	"Boolean" constants
 */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 *	GoAhead Copyright.
 */
#define GOAHEAD_COPYRIGHT \
	T("Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.")

#define gchdir		chdir
#define gmkdir(s)	mkdir(s,0755)
#define grmdir		rmdir

#define gclose		close
#define gclosedir	closedir
#define gchmod		chmod
#define ggetcwd		getcwd
#define glseek		lseek
#define gloadModule	loadModule
#define gopen		open
#define gopendir	opendir
#define gread		read
#define greaddir	readdir
#define grename		rename
#define gstat		stat
#define gunlink		unlink
#define gwrite		write

#define gasctime	asctime
#define gsprintf	sprintf
#define gprintf		printf
#define gfprintf	fprintf
#define gsscanf		sscanf
#define gvsprintf	vsprintf

#define gstrcpy		strcpy
#define gstrncpy	strncpy
#define gstrncat	strncat
#define gstrlen		strlen
#define gstrcat		strcat
#define gstrcmp		strcmp
#define gstrncmp	strncmp
#define gstricmp	strcmpci
#define gstrchr		strchr
#define gstrrchr	strrchr
#define gstrtok		strtok
#define gstrnset	strnset
#define gstrrchr	strrchr
#define gstrspn	strspn
#define gstrcspn	strcspn
#define gstrstr		strstr
#define gstrtol		strtol

#define gfopen		fopen
#define gcreat		creat
#define gfgets		fgets
#define gfputs		fputs
#define gfscanf		fscanf
#define ggets		gets
#define gtmpnam		tmpnam
#define gtempnam	tempnam
#define gfindfirst	_findfirst
#define gfinddata_t	_finddata_t
#define gfindnext	_findnext
#define gfindclose	_findclose
#define gaccess		access

typedef struct stat gstat_t;

#define gremove		remove

#define gtolower	tolower
#define gtoupper	toupper
#define gisspace	isspace
#define gisdigit	isdigit
#define gisxdigit	isxdigit
#define gisalnum	isalnum
#define gisalpha	isalpha
#define gisupper	isupper
#define gislower	islower
#define gatoi		atoi

#define gctime		ctime
#define ggetenv		getenv
#define gexecvp		execvp
#define gmain		main

/********************************** Defines ***********************************/

#ifndef FNAMESIZE
#define FNAMESIZE			254			/* Max length of file names */
#endif /* FNAMESIZE */

#define E_MAX_ERROR			4096
#define E_MAX_REQUEST			2048			/* Request safeguard max */
#define URL_MAX				4096

/*
 * Error types
 */
#define	E_ASSERT			0x1			/* Assertion error */
#define	E_LOG				0x2			/* Log error to log file */
#define	E_USER				0x3			/* Error that must be displayed */

#define E_L				T(__FILE__), __LINE__
#define E_ARGS_DEC			char_t *file, int line
#define E_ARGS				file, line

#ifdef ASSERT
	#define a_assert(C)		if (C) ; else error(E_L, E_ASSERT, T("%s"), T(#C))
#else
	#define a_assert(C)		do{} while(0)
#endif /* ASSERT || ASSERT_CE */

/******************************************************************************/
/*                                 VALUE                                      */
/******************************************************************************/
/*
 *	These values are not prefixed so as to aid code readability
 */
typedef enum {
	tundefined	= 0,
	tbyteint	= 1,
	tshortint	= 2,
	tinteger	= 3,
	thex		= 4,
	tpercent 	= 5,
	toctal		= 6,
	tbig		= 7,
	tflag		= 8,
	tfloating	= 9,
	tdoublefloating	= 10,
	tstring 	= 11,
	tbytes 		= 12,
	tsymbol 	= 13,
	terrmsg 	= 14
} vtype_t;

#ifndef __NO_PACK
#pragma pack(2)
#endif /* _NO_PACK */
typedef struct {

	union {
		int	tundefined;
		char	tbyteint;
		short	tshortint;
		long	tinteger;
		long	thex;
		char	tpercent;
		long	toctal;
		long	tbig[2];
		char	tflag;
		float	tfloating;
		double	tdoublefloating;
		char_t	*tstring;
		char	*tbytes;
		void	*tsymbol;
		char_t	*terrmsg;
	} value;
	vtype_t		type;
	unsigned int	valid		: 8;
	unsigned int	allocated	: 8;		/* String was balloced */
} value_t;
#ifndef __NO_PACK
#pragma pack()
#endif /* __NO_PACK */

/*
 *	Allocation flags
 */
#define VALUE_ALLOCATE		0x1

#define value_numeric(t)	(t >= byteint && t <= big)
#define value_str(t) 		(t >= string && t <= bytes)
#define value_ok(t) 		(t > undefined && t <= symbol)

#define VALUE_VALID			{ {0}, integer, 1 }
#define VALUE_INVALID		{ {0}, undefined, 0 }

/******************************************************************************/
/*
 *	A ring queue allows maximum utilization of memory for data storage and is
 *	ideal for input/output buffering. This module provides a highly effecient
 *	implementation and a vehicle for dynamic strings.
 *
 *	WARNING:  This is a public implementation and callers have full access to
 *	the queue structure and pointers.  Change this module very carefully.
 *
 *	This module follows the open/close model.
 *
 *	Operation of a ringq where rq is a pointer to a ringq :
 *
 *		rq->buflen contains the size of the buffer.
 *		rq->buf will point to the start of the buffer.
 *		rq->servp will point to the first (un-consumed) data byte.
 *		rq->endp will point to the next free location to which new data is added
 *		rq->endbuf will point to one past the end of the buffer.
 *
 *	Eg. If the ringq contains the data "abcdef", it might look like :
 *
 *	+-------------------------------------------------------------------+
 *  |   |   |   |   |   |   |   | a | b | c | d | e | f |   |   |   |   |
 *	+-------------------------------------------------------------------+
 *    ^                           ^                       ^               ^
 *    |                           |                       |               |
 *  rq->buf                    rq->servp               rq->endp      rq->enduf
 *     
 *	The queue is empty when servp == endp.  This means that the queue will hold
 *	at most rq->buflen -1 bytes.  It is the fillers responsibility to ensure
 *	the ringq is never filled such that servp == endp.
 *
 *	It is the fillers responsibility to "wrap" the endp back to point to
 *	rq->buf when the pointer steps past the end. Correspondingly it is the
 *	consumers responsibility to "wrap" the servp when it steps to rq->endbuf.
 *	The ringqPutc and ringqGetc routines will do this automatically.
 */

/*
 *	Ring queue buffer structure
 */
typedef struct {
	unsigned char	*buf;				/* Holding buffer for data */
	unsigned char	*servp;				/* Pointer to start of data */
	unsigned char	*endp;				/* Pointer to end of data */
	unsigned char	*endbuf;			/* Pointer to end of buffer */
	int		buflen;				/* Length of ring queue */
	int		maxsize;			/* Maximum size */
	int		increment;			/* Growth increment */
} ringq_t;

/*
 *	Block allocation (balloc) definitions
 */
#ifdef  B_STATS
#define B_L			T(__FILE__), __LINE__
#define B_ARGS_DEC		char_t *file, int line
#define B_ARGS			file, line
#endif

/*
 *	Block classes are: 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 
 *					   16384, 32768, 65536 
 */
typedef struct {
	union {
		void	*next;						/* Pointer to next in q */
		int	size;						/* Actual requested size */
	} u;
	int	flags;							/* Per block allocation flags */
} bType;

#define B_SHIFT		4				/* Convert size to class */
#define B_ROUND		((1 << (B_SHIFT)) - 1)
#define B_MAX_CLASS	13				/* Maximum class number + 1 */
#define B_MALLOCED	0x80000000			/* Block was malloced */
#define B_DEFAULT_MEM	(64 * 1024)			/* Default memory allocation */
#define B_MAX_FILES	(512)				/* Maximum number of files */
#define B_FILL_CHAR	(0x77)				/* Fill byte for buffers */
#define B_FILL_WORD	(0x77777777)			/* Fill word for buffers */
#define B_MAX_BLOCKS	(64 * 1024)			/* Maximum allocated blocks */

/*
 *	Flags. The integrity value is used as an arbitrary value to fill the flags.
 */
#define B_INTEGRITY		0x8124000		/* Integrity value */
#define B_INTEGRITY_MASK	0xFFFF000		/* Integrity mask */
#define B_USE_MALLOC		0x1			/* Okay to use malloc if required */
#define B_USER_BUF		0x2			/* User supplied buffer for mem */

/*
 *	The symbol table record for each symbol entry
 */

typedef struct sym_t {
	struct sym_t	*forw;					/* Pointer to next hash list */
	value_t		name;					/* Name of symbol */
	value_t		content;				/* Value of symbol */
	int		arg;					/* Parameter value */
} sym_t;

typedef int sym_fd_t;						/* Returned by symOpen */

/*
 *	Script engines
 */
#define EMF_SCRIPT_JSCRIPT		0		/* javascript */
#define EMF_SCRIPT_TCL	 		1		/* tcl */
#define EMF_SCRIPT_EJSCRIPT 		2		/* Ejscript */
#define EMF_SCRIPT_MAX	 		3

#define	MAXINT		INT_MAX
#define BITSPERBYTE 8
#define BITS(type)	(BITSPERBYTE * (int) sizeof(type))
#define	STRSPACE	T("\t \n\r\t")

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

/******************************************************************************/
/*                                  CRON                                      */
/******************************************************************************/

typedef struct {
	char_t	*minute;
	char_t	*hour;
	char_t	*day;
	char_t	*month;
	char_t	*dayofweek;
} cron_t;

long	cronUntil(cron_t *cp, int period, time_t testTime);
int	cronAlloc(cron_t *cp, char_t *str);
int	cronFree(cron_t *cp);

/******************************************************************************/
/*                                 SOCKET                                     */
/******************************************************************************/
/*
 *	Socket flags
 */

#define SOCKET_EOF              	0x1     /*  Seen end of file */
#define SOCKET_CONNECTING       	0x2     /*  Connect in progress */
#define SOCKET_RESERVICE        	0x4     /*  Socket needs re-servicing */
#define SOCKET_ASYNC            	0x8     /*  Use async connect */
#define SOCKET_BLOCK            	0x10    /*  Use blocking I/O */
#define SOCKET_LISTENING        	0x20    /*  Socket is server listener */
#define SOCKET_CLOSING          	0x40    /*  Socket is closing */
#define SOCKET_CONNRESET        	0x80    /*  Socket connection was reset */
#define SOCKET_HANDSHAKING      	0x100   /*  Doing SSL handshake */
#define SOCKET_BUFFERED_READ    	0x200   /*  Message pending on this socket */
#define SOCKET_BUFFERED_WRITE   	0x400   /*  Message pending on this socket */
#define SOCKET_NODELAY          	0x800   /*  Disable Nagle algorithm */
#define SOCKET_FLUSHING			0x900	/* Background flushing */

#define SOCKET_PORT_MAX			0xffff	/* Max Port size */


/*
 *	Socket error values
 */
#define SOCKET_WOULDBLOCK		1		/* Socket would block on I/O */
#define SOCKET_RESET			2		/* Socket has been reset */
#define SOCKET_NETDOWN			3		/* Network is down */
#define SOCKET_AGAIN			4		/* Issue the request again */
#define SOCKET_INTR			5		/* Call was interrupted */
#define SOCKET_INVAL			6		/* Invalid */

/*
 *	Handler event masks
 */
#define SOCKET_READABLE			0x2		/* Make socket readable */ 
#define SOCKET_WRITABLE			0x4		/* Make socket writable */
#define SOCKET_EXCEPTION		0x8		/* Interested in exceptions */
#define EMF_SOCKET_MESSAGE		(WM_USER+13)
#define SOCKET_BUFSIZ			1024		/* Underlying buffer size */

typedef void 	(*socketHandler_t)(int sid, int mask, void* data);
typedef int	(*socketAccept_t)(int sid, char *ipaddr, int port, int listenSid);

typedef struct {
	ringq_t		inBuf;				/* Input ring queue */
	ringq_t		outBuf;				/* Output ring queue */
	ringq_t		lineBuf;			/* Line ring queue */
	socketAccept_t	accept;				/* Accept handler */
	socketHandler_t	handler;			/* User I/O handler */
	void*		handler_data;			/* User handler data */
	int		handlerMask;			/* Handler events of interest */
	int		sid;				/* Index into socket[] */
	int		port;				/* Port to listen on */
	int		flags;				/* Current state flags */
	int		sock;				/* Actual socket handle */
	int		fileHandle;			/* ID of the file handler */
	int		interestEvents;			/* Mask of events to watch for */
	int		currentEvents;			/* Mask of ready events (FD_xx) */
	int		selectEvents;			/* Events being selected */
	int		saveMask;			/* saved Mask for socketFlush */
	int		error;				/* Last error */
} socket_t;

/********************************* Prototypes *********************************/
/*
 *	Balloc module
 *	Define NO_BALLOC to turn off our balloc module altogether
 *		#define NO_BALLOC 1
 */

#ifndef B_STATS
#define bstrdup(B_ARGS, p) bstrdup(p)
#define balloc(B_ARGS, num) balloc(num)
#define bfree(B_ARGS, p) bfree(p)
#define brealloc(B_ARGS, p, size) brealloc(p, size)
#endif /* B_STATS */

int bopen(void *buf, int bufsize, int flags);
void bclose();
void bstats(int handle, void (*writefn)(int handle, char_t *fmt, ...));
void *balloc(B_ARGS_DEC, int size);
void bfree(B_ARGS_DEC, void *mp);
void *brealloc(B_ARGS_DEC, void *buf, int newsize);
char_t *bstrdup(B_ARGS_DEC, char_t *s);

/*
 *	Flags. The integrity value is used as an arbitrary value to fill the flags.
 */
#define B_USE_MALLOC		0x1				/* Okay to use malloc if required */
#define B_USER_BUF		0x2				/* User supplied buffer for mem */

#if defined (WEBS)
/*
 *	The open source webserver uses a different callback/timer mechanism
 *	than other emf derivative products such as FieldUpgrader agents
 *	so redefine those API for webserver so that they can coexist in the
 *	same address space as the others.
 */
#define emfSchedCallback	websSchedCallBack
#define emfUnschedCallback	websUnschedCallBack
#define emfReschedCallback	websReschedCallBack
#endif /* WEBS */

typedef void	(emfSchedProc)(void *data, int id);
int		emfSchedCallback(int delay, emfSchedProc *proc, void *arg);
void 	emfUnschedCallback(int id);
void 	emfReschedCallback(int id, int delay);
void		emfSchedProcess();
int		emfInstGet();
void		emfInstSet(int inst);
void		error(E_ARGS_DEC, int flags, char_t *fmt, ...);
void		(*errorSetHandler(void (*function)(int etype, char_t *msg))) \
					(int etype, char_t *msg);

#ifdef B_STATS
#define 		hAlloc(x) 				HALLOC(B_L, x)
#define			hAllocEntry(x, y, z)	HALLOCENTRY(B_L, x, y, z)
int		HALLOC(B_ARGS_DEC, void ***map);
int 		HALLOCENTRY(B_ARGS_DEC, void ***list, int *max, int size);
#else
int		hAlloc(void ***map);
int 		hAllocEntry(void ***list, int *max, int size);
#endif /* B_STATS */

int		hFree(void ***map, int handle);

int	 	ringqOpen(ringq_t *rq, int increment, int maxsize);
void 		ringqClose(ringq_t *rq);
int 		ringqLen(ringq_t *rq);

int 		ringqPutc(ringq_t *rq, char_t c);
int	 	ringqInsertc(ringq_t *rq, char_t c);
int	 	ringqPutStr(ringq_t *rq, char_t *str);
int 		ringqGetc(ringq_t *rq);

int		fmtValloc(char_t **s, int n, char_t *fmt, va_list arg);
int		fmtAlloc(char_t **s, int n, char_t *fmt, ...);
int		fmtStatic(char_t *s, int n, char_t *fmt, ...);

int 		ringqPutBlk(ringq_t *rq, unsigned char *buf, int len);
int 		ringqPutBlkMax(ringq_t *rq);
void 		ringqPutBlkAdj(ringq_t *rq, int size);
int 		ringqGetBlk(ringq_t *rq, unsigned char *buf, int len);
int 		ringqGetBlkMax(ringq_t *rq);
void 		ringqGetBlkAdj(ringq_t *rq, int size);
void 		ringqFlush(ringq_t *rq);
void 		ringqAddNull(ringq_t *rq);

int		scriptSetVar(int engine, char_t *var, char_t *value);
int		scriptEval(int engine, char_t *cmd, char_t **rslt, void* chan);
void		socketClose();
void		socketCloseConnection(int sid);
void		socketCreateHandler(int sid, int mask, socketHandler_t 
					handler, void* arg);
void		socketDeleteHandler(int sid);
int		socketEof(int sid);
int 		socketCanWrite(int sid);
void 		socketSetBufferSize(int sid, int in, int line, int out);
int		socketFlush(int sid);
int		socketGets(int sid, char_t **buf);
int		socketGetPort(int sid);
int		socketInputBuffered(int sid);
int		socketOpen();
int 		socketOpenConnection(int port, socketAccept_t accept, int flags);
void		socketProcess(int hid);
int		socketRead(int sid, char *buf, int len);
int 		socketReady(int hid);
int		socketWrite(int sid, char *buf, int len);
int 		socketSelect(int hid, int timeout);
int 		socketGetHandle(int sid);
int 		socketSetBlock(int sid, int flags);
int 		socketGetBlock(int sid);
int 		socketAlloc(int port, socketAccept_t accept, int flags);
void 		socketFree(int sid);
int		socketGetError();
			socket_t *socketPtr(int sid);
int 		socketWaitForEvent(socket_t *sp, int events, int *errCode);
void 		socketRegisterInterest(socket_t *sp, int handlerMask);
int 		socketGetInput(int sid, char *buf, int toRead, int *errCode);
void		setSocketNodelayReuse(int sock);

char_t	*strlower(char_t *string);
char_t	*strupper(char_t *string);

char_t	*stritoa(int n, char_t *string, int width);

sym_fd_t	symOpen(int hash_size);
void		symClose(sym_fd_t sd);
sym_t	*symLookup(sym_fd_t sd, char_t *name);
sym_t	*symEnter(sym_fd_t sd, char_t *name, value_t v, int arg);
int		symDelete(sym_fd_t sd, char_t *name);
void 	symWalk(sym_fd_t sd, void (*fn)(sym_t *symp));
sym_t	*symFirst(sym_fd_t sd);
sym_t	*symNext(sym_fd_t sd);
int		symSubOpen();
void 	symSubClose();

void		trace(int lev, char_t *fmt, ...);
void		traceRaw(char_t *buf);
void		(*traceSetHandler(void (*function)(int level, char_t *buf))) 
					(int level, char_t *buf);

value_t 	valueInteger(long value);
value_t	valueString(char_t *value, int flags);
value_t	valueErrmsg(char_t *value);
void 	valueFree(value_t *v);
int		vxchdir(char *dirname);

unsigned int hextoi(char_t *hexstring);
unsigned int gstrtoi(char_t *s);
extern				time_t	timeMsec();

char_t	*ballocAscToUni(char  *cp, int alen);
char		*ballocUniToAsc(char_t *unip, int ulen);

char_t	*basicGetHost();
char_t	*basicGetAddress();
char_t	*basicGetProduct();
void		basicSetHost(char_t *host);
void		basicSetAddress(char_t *addr);

int		harnessOpen(char_t **argv);
void		harnessClose(int status);
void		harnessTesting(char_t *msg, ...);
void		harnessPassed();
void		harnessFailed(int line);
int		harnessLevel();
#endif /* _h_UEMF */
