/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 31 "cfparse.y" /* yacc.c:339  */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/time.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>

#include "dhcp6.h"
#include "config.h"
#include "common.h"

extern int lineno;
extern int cfdebug;

extern void yywarn __P((char *, ...))
	__attribute__((__format__(__printf__, 1, 2)));
extern void yyerror __P((char *, ...))
	__attribute__((__format__(__printf__, 1, 2)));

#define MAKE_NAMELIST(l, n, p) do { \
	(l) = (struct cf_namelist *)malloc(sizeof(*(l))); \
	if ((l) == NULL) { \
		yywarn("can't allocate memory"); \
		if (p) cleanup_cflist(p); \
		return (-1); \
	} \
	memset((l), 0, sizeof(*(l))); \
	l->line = lineno; \
	l->name = (n); \
	l->params = (p); \
	} while (0)

#define MAKE_CFLIST(l, t, pp, pl) do { \
	(l) = (struct cf_list *)malloc(sizeof(*(l))); \
	if ((l) == NULL) { \
		yywarn("can't allocate memory"); \
		if (pp) free(pp); \
		if (pl) cleanup_cflist(pl); \
		return (-1); \
	} \
	memset((l), 0, sizeof(*(l))); \
	l->line = lineno; \
	l->type = (t); \
	l->ptr = (pp); \
	l->list = (pl); \
	l->tail = (l); \
	} while (0)

static struct cf_namelist *iflist_head, *hostlist_head, *iapdlist_head;
static struct cf_namelist *profilelist_head;
static struct cf_namelist *addrpoollist_head;
static struct cf_namelist *authinfolist_head, *keylist_head;
static struct cf_namelist *ianalist_head;
struct cf_list *cf_dns_list, *cf_dns_name_list, *cf_ntp_list;
struct cf_list *cf_sip_list, *cf_sip_name_list;
struct cf_list *cf_nis_list, *cf_nis_name_list;
struct cf_list *cf_nisp_list, *cf_nisp_name_list;
struct cf_list *cf_bcmcs_list, *cf_bcmcs_name_list;
long long cf_refreshtime = -1;

extern int yylex __P((void));
extern int cfswitch_buffer __P((char *));
static int add_namelist __P((struct cf_namelist *, struct cf_namelist **));
static void cleanup __P((void));
static void cleanup_namelist __P((struct cf_namelist *));
static void cleanup_cflist __P((struct cf_list *));

#line 140 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTERFACE = 258,
    IFNAME = 259,
    PROFILE = 260,
    PROFILENAME = 261,
    PREFIX_INTERFACE = 262,
    SLA_ID = 263,
    SLA_LEN = 264,
    IFID = 265,
    DUID_ID = 266,
    ID_ASSOC = 267,
    IA_PD = 268,
    IAID = 269,
    IA_NA = 270,
    ADDRESS = 271,
    REQUEST = 272,
    SEND = 273,
    ALLOW = 274,
    PREFERENCE = 275,
    HOST = 276,
    HOSTNAME = 277,
    DUID = 278,
    OPTION = 279,
    RAPID_COMMIT = 280,
    DNS_SERVERS = 281,
    DNS_NAME = 282,
    NTP_SERVERS = 283,
    REFRESHTIME = 284,
    SIP_SERVERS = 285,
    SIP_NAME = 286,
    NIS_SERVERS = 287,
    NIS_NAME = 288,
    NISP_SERVERS = 289,
    NISP_NAME = 290,
    BCMCS_SERVERS = 291,
    BCMCS_NAME = 292,
    INFO_ONLY = 293,
    SCRIPT = 294,
    DELAYEDKEY = 295,
    AUTHENTICATION = 296,
    PROTOCOL = 297,
    ALGORITHM = 298,
    DELAYED = 299,
    RECONFIG = 300,
    HMACMD5 = 301,
    MONOCOUNTER = 302,
    AUTHNAME = 303,
    RDM = 304,
    KEY = 305,
    KEYINFO = 306,
    REALM = 307,
    KEYID = 308,
    SECRET = 309,
    KEYNAME = 310,
    EXPIRE = 311,
    ADDRPOOL = 312,
    POOLNAME = 313,
    RANGE = 314,
    TO = 315,
    ADDRESS_POOL = 316,
    INCLUDE = 317,
    NUMBER = 318,
    SLASH = 319,
    EOS = 320,
    BCL = 321,
    ECL = 322,
    STRING = 323,
    QSTRING = 324,
    PREFIX = 325,
    INFINITY = 326,
    COMMA = 327
  };
#endif
/* Tokens.  */
#define INTERFACE 258
#define IFNAME 259
#define PROFILE 260
#define PROFILENAME 261
#define PREFIX_INTERFACE 262
#define SLA_ID 263
#define SLA_LEN 264
#define IFID 265
#define DUID_ID 266
#define ID_ASSOC 267
#define IA_PD 268
#define IAID 269
#define IA_NA 270
#define ADDRESS 271
#define REQUEST 272
#define SEND 273
#define ALLOW 274
#define PREFERENCE 275
#define HOST 276
#define HOSTNAME 277
#define DUID 278
#define OPTION 279
#define RAPID_COMMIT 280
#define DNS_SERVERS 281
#define DNS_NAME 282
#define NTP_SERVERS 283
#define REFRESHTIME 284
#define SIP_SERVERS 285
#define SIP_NAME 286
#define NIS_SERVERS 287
#define NIS_NAME 288
#define NISP_SERVERS 289
#define NISP_NAME 290
#define BCMCS_SERVERS 291
#define BCMCS_NAME 292
#define INFO_ONLY 293
#define SCRIPT 294
#define DELAYEDKEY 295
#define AUTHENTICATION 296
#define PROTOCOL 297
#define ALGORITHM 298
#define DELAYED 299
#define RECONFIG 300
#define HMACMD5 301
#define MONOCOUNTER 302
#define AUTHNAME 303
#define RDM 304
#define KEY 305
#define KEYINFO 306
#define REALM 307
#define KEYID 308
#define SECRET 309
#define KEYNAME 310
#define EXPIRE 311
#define ADDRPOOL 312
#define POOLNAME 313
#define RANGE 314
#define TO 315
#define ADDRESS_POOL 316
#define INCLUDE 317
#define NUMBER 318
#define SLASH 319
#define EOS 320
#define BCL 321
#define ECL 322
#define STRING 323
#define QSTRING 324
#define PREFIX 325
#define INFINITY 326
#define COMMA 327

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 128 "cfparse.y" /* yacc.c:355  */

	long long num;
	char* str;
	struct cf_list *list;
	struct dhcp6_prefix *prefix;
	struct dhcp6_range *range;
	struct dhcp6_poolspec *pool;

#line 333 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 350 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   252

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  241

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   327

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   151,   151,   153,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   169,   181,   193,   205,   214,   229,   238,
     247,   262,   271,   286,   295,   310,   319,   334,   363,   372,
     386,   395,   412,   424,   436,   447,   459,   460,   478,   501,
     502,   520,   528,   536,   544,   552,   560,   568,   576,   585,
     593,   601,   609,   620,   624,   634,   642,   650,   658,   666,
     674,   682,   690,   698,   706,   714,   722,   730,   738,   746,
     754,   765,   796,   823,   856,   883,   916,   941,   972,   976,
     983,   984,  1002,  1003,  1014,  1024,  1025,  1043,  1051,  1059,
    1070,  1071,  1089,  1100,  1101,  1119,  1127,  1135,  1143,  1154,
    1155,  1159,  1163,  1167,  1168,  1186,  1194,  1202,  1210
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTERFACE", "IFNAME", "PROFILE",
  "PROFILENAME", "PREFIX_INTERFACE", "SLA_ID", "SLA_LEN", "IFID",
  "DUID_ID", "ID_ASSOC", "IA_PD", "IAID", "IA_NA", "ADDRESS", "REQUEST",
  "SEND", "ALLOW", "PREFERENCE", "HOST", "HOSTNAME", "DUID", "OPTION",
  "RAPID_COMMIT", "DNS_SERVERS", "DNS_NAME", "NTP_SERVERS", "REFRESHTIME",
  "SIP_SERVERS", "SIP_NAME", "NIS_SERVERS", "NIS_NAME", "NISP_SERVERS",
  "NISP_NAME", "BCMCS_SERVERS", "BCMCS_NAME", "INFO_ONLY", "SCRIPT",
  "DELAYEDKEY", "AUTHENTICATION", "PROTOCOL", "ALGORITHM", "DELAYED",
  "RECONFIG", "HMACMD5", "MONOCOUNTER", "AUTHNAME", "RDM", "KEY",
  "KEYINFO", "REALM", "KEYID", "SECRET", "KEYNAME", "EXPIRE", "ADDRPOOL",
  "POOLNAME", "RANGE", "TO", "ADDRESS_POOL", "INCLUDE", "NUMBER", "SLASH",
  "EOS", "BCL", "ECL", "STRING", "QSTRING", "PREFIX", "INFINITY", "COMMA",
  "$accept", "statements", "statement", "interface_statement",
  "profile_statement", "host_statement", "option_statement",
  "ia_statement", "authentication_statement", "key_statement",
  "include_statement", "addrpool_statement", "address_list",
  "address_list_ent", "declarations", "declaration", "dhcpoption_list",
  "dhcpoption", "rangeparam", "addressparam", "prefixparam", "poolparam",
  "duration", "iapdconf_list", "iapdconf", "prefix_interface", "ifparams",
  "ifparam", "ianaconf_list", "ianaconf", "authparam_list", "authparam",
  "authproto", "authalg", "authrdm", "keyparam_list", "keyparam", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327
};
# endif

#define YYPACT_NINF -158

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-158)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -158,     7,  -158,    19,     3,    46,    22,   127,   -21,   -10,
     -28,   -30,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,     9,    11,    -6,     8,    16,  -158,    35,  -158,
      10,  -158,    37,  -158,    38,  -158,    39,  -158,    40,    20,
      31,    47,    49,  -158,  -158,    50,  -158,    51,  -158,  -158,
     -27,    53,   -22,    54,   -16,    63,   -15,    64,   -14,    70,
      28,    79,  -158,  -158,  -158,  -158,    17,    72,  -158,    -5,
    -158,     0,    82,  -158,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,   -25,    27,   107,
      69,   154,   154,   154,    52,    74,    83,    81,    97,   102,
     104,    86,   105,  -158,   106,    -4,   171,   111,   105,  -158,
    -158,     4,    69,   113,  -158,   128,   -40,   146,   147,   129,
     131,  -158,   130,   135,   132,   133,   138,  -158,   139,   -42,
     140,   137,   143,  -158,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,   159,   144,   136,   145,
     148,   149,   150,  -158,   151,   152,   158,   155,   -42,   156,
    -158,   160,   157,  -158,   161,   153,  -158,   162,   163,   164,
    -158,  -158,  -158,  -158,   165,  -158,   166,  -158,   167,   168,
    -158,   169,   170,   172,   173,  -158,  -158,  -158,  -158,   -42,
    -158,  -158,  -158,  -158,  -158,   154,  -158,  -158,  -158,  -158,
    -158,  -158,   174,  -158,   -42,  -158,   176,  -158,  -158,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,  -158,  -158,  -158,   -42,     5,   -42,   177,   178,
     180,   179,  -158,  -158,   181,   182,   183,  -158,  -158,  -158,
    -158
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     3,     4,     5,     6,     7,     8,     9,    10,
      12,    11,     0,     0,     0,     0,     0,    36,     0,    36,
       0,    36,     0,    36,     0,    36,     0,    36,     0,     0,
       0,     0,     0,    39,    39,     0,    80,     0,    90,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    93,   103,    39,    34,     0,     0,    80,     0,
      90,     0,     0,    16,    38,    37,    17,    18,    27,    19,
      20,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,     0,     0,     0,    81,
      82,     0,     0,     0,    91,     0,     0,     0,     0,     0,
       0,    94,     0,     0,     0,     0,     0,   104,     0,     0,
       0,     0,     0,    55,    61,    62,    63,    64,    59,    60,
      65,    66,    67,    68,    69,    70,     0,     0,    53,     0,
       0,     0,     0,    43,     0,     0,     0,     0,     0,     0,
      13,     0,     0,    14,     0,     0,    29,     0,     0,     0,
      31,    15,    99,   100,     0,   101,     0,   102,     0,     0,
      32,     0,     0,     0,     0,    33,    35,    79,    78,    72,
      46,    57,    58,    56,    42,     0,    41,    44,    48,    45,
      49,    50,     0,    51,    76,    52,     0,    47,    28,    85,
      83,    30,    92,    95,    96,    97,    98,   105,   106,   107,
     108,    73,    54,    71,    77,     0,     0,    74,     0,     0,
       0,     0,    86,    75,     0,     0,     0,    84,    87,    88,
      89
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,   103,  -158,   -38,  -158,   -92,   118,  -158,   100,
     115,  -158,  -157,   184,  -158,  -158,  -158,  -158,   175,  -158,
    -158,  -158,  -158,  -158,  -158,  -158,  -158
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    50,    75,    66,   103,   147,   148,   157,   130,
     162,   159,   189,    69,   109,   110,   226,   232,    71,   114,
      87,   121,   174,   176,   178,    88,   127
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
     149,   204,   106,   106,   172,   173,    67,     2,    45,    23,
       3,    72,     4,   228,   229,   230,   112,   116,   117,     5,
     112,   187,    47,    22,   118,   119,    89,    39,     6,   188,
      41,     7,   221,    90,    91,    92,    93,    94,    73,    42,
      95,    74,   120,    77,    26,    40,    74,   224,     8,    79,
      81,    83,    74,    74,    74,    96,    97,    98,     9,    24,
      46,    25,   107,   164,    10,   108,   108,   113,   227,    11,
     233,   168,   231,    53,    48,    43,    99,    44,   100,   122,
     123,   124,    49,   125,   101,   152,    62,   102,    90,    91,
      92,    93,    94,    85,   126,    95,    74,    63,    90,    91,
      92,    93,    94,   222,    51,    95,    55,    57,    59,    61,
      96,    97,    98,    64,    65,   151,    68,    70,    76,    78,
      96,    97,    98,    90,    91,    92,    93,    94,    80,    82,
      95,    99,    52,   100,    54,    84,    56,   129,    58,   104,
      60,    99,   102,   100,    86,    96,    97,    98,   153,   115,
     154,   160,   102,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,   155,    99,   131,   100,   132,
     156,   163,   158,   161,   128,   165,   166,   102,   170,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   175,   171,   177,   146,   180,   179,   182,   181,
     191,   183,   184,   185,   186,   190,   192,   193,   195,   194,
     196,   150,   169,   197,   198,   199,   200,   201,   202,   209,
     203,   205,   207,   167,   206,     0,   208,   210,   211,   212,
     213,   214,   215,   216,   217,   218,     0,   219,   220,   225,
     234,   235,   223,   236,   237,   111,   238,   239,   240,     0,
       0,     0,   105
};

static const yytype_int16 yycheck[] =
{
      92,   158,     7,     7,    44,    45,    44,     0,    14,     6,
       3,    49,     5,     8,     9,    10,    16,    42,    43,    12,
      16,    63,    14,     4,    49,    50,    64,    48,    21,    71,
      58,    24,   189,    16,    17,    18,    19,    20,    65,    69,
      23,    68,    67,    65,    22,    55,    68,   204,    41,    65,
      65,    65,    68,    68,    68,    38,    39,    40,    51,    13,
      66,    15,    67,    67,    57,    70,    70,    67,   225,    62,
     227,    67,    67,    63,    66,    66,    59,    66,    61,    52,
      53,    54,    66,    56,    67,    11,    66,    70,    16,    17,
      18,    19,    20,    65,    67,    23,    68,    66,    16,    17,
      18,    19,    20,   195,    69,    23,    69,    69,    69,    69,
      38,    39,    40,    66,    65,    63,    66,    66,    65,    65,
      38,    39,    40,    16,    17,    18,    19,    20,    65,    65,
      23,    59,    29,    61,    31,    65,    33,    68,    35,    67,
      37,    59,    70,    61,    65,    38,    39,    40,    65,    67,
      69,    65,    70,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    68,    59,    13,    61,    15,
      68,    65,    68,    68,    67,     4,    65,    70,    65,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    46,    65,    47,    41,    65,    68,    63,    69,
      63,    69,    69,    65,    65,    65,    63,    48,    72,    65,
      65,    93,   112,    65,    65,    65,    65,    65,    60,    66,
      65,    65,    65,   108,    64,    -1,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    -1,    65,    65,    63,
      63,    63,    68,    63,    65,    70,    65,    65,    65,    -1,
      -1,    -1,    68
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,     0,     3,     5,    12,    21,    24,    41,    51,
      57,    62,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,     4,     6,    13,    15,    22,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    48,
      55,    58,    69,    66,    66,    14,    66,    14,    66,    66,
      85,    69,    85,    63,    85,    69,    85,    69,    85,    69,
      85,    69,    66,    66,    66,    65,    87,    87,    66,    96,
      66,   101,    87,    65,    68,    86,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,   103,   108,    87,
      16,    17,    18,    19,    20,    23,    38,    39,    40,    59,
      61,    67,    70,    88,    67,    96,     7,    67,    70,    97,
      98,   101,    16,    67,   102,    67,    42,    43,    49,    50,
      67,   104,    52,    53,    54,    56,    67,   109,    67,    68,
      92,    13,    15,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    41,    89,    90,    89,
      90,    63,    11,    65,    69,    68,    68,    91,    68,    94,
      65,    68,    93,    65,    67,     4,    65,    93,    67,    92,
      65,    65,    44,    45,   105,    46,   106,    47,   107,    68,
      65,    69,    63,    69,    69,    65,    65,    63,    71,    95,
      65,    63,    63,    48,    65,    72,    65,    65,    65,    65,
      65,    65,    60,    65,    95,    65,    64,    65,    65,    66,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    95,    89,    68,    95,    63,    99,    95,     8,     9,
      10,    67,   100,    95,    63,    63,    63,    65,    65,    65,
      65
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    74,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    76,    77,    78,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    80,    80,
      80,    80,    81,    82,    83,    84,    85,    85,    86,    87,
      87,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    89,    89,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    91,    92,    92,    93,    93,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    99,    99,   100,   100,   100,
     101,   101,   102,   103,   103,   104,   104,   104,   104,   105,
     105,   106,   107,   108,   108,   109,   109,   109,   109
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     6,     6,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     7,     6,
       7,     6,     6,     6,     3,     6,     0,     2,     1,     0,
       2,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     3,     4,     5,     2,     3,     1,     1,
       0,     2,     1,     3,     6,     0,     2,     3,     3,     3,
       0,     2,     3,     0,     2,     3,     3,     3,     3,     1,
       1,     1,     1,     0,     2,     3,     3,     3,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 13:
#line 170 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *ifl;

		MAKE_NAMELIST(ifl, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(ifl, &iflist_head))
			return (-1);
	}
#line 1613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 182 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *profilelist;

		MAKE_NAMELIST(profilelist, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(profilelist, &profilelist_head))
			return (-1);
	}
#line 1626 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 194 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *host;

		MAKE_NAMELIST(host, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(host, &hostlist_head))
			return (-1);
	}
#line 1639 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 206 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_dns_list == NULL)
				cf_dns_list = (yyvsp[-1].list);
			else {
				cf_dns_list->tail->next = (yyvsp[-1].list);
				cf_dns_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1652 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 215 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, CFLISTENT_GENERIC, (yyvsp[-1].str), NULL);

			if (cf_dns_name_list == NULL) {
				cf_dns_name_list = l;
				cf_dns_name_list->tail = l;
				cf_dns_name_list->next = NULL;
			} else {
				cf_dns_name_list->tail->next = l;
				cf_dns_name_list->tail = l->tail;
			}
		}
#line 1671 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 230 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_ntp_list == NULL)
				cf_ntp_list = (yyvsp[-1].list);
			else {
				cf_ntp_list->tail->next = (yyvsp[-1].list);
				cf_ntp_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1684 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 239 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_sip_list == NULL)
				cf_sip_list = (yyvsp[-1].list);
			else {
				cf_sip_list->tail->next = (yyvsp[-1].list);
				cf_sip_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 248 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, CFLISTENT_GENERIC, (yyvsp[-1].str), NULL);

			if (cf_sip_name_list == NULL) {
				cf_sip_name_list = l;
				cf_sip_name_list->tail = l;
				cf_sip_name_list->next = NULL;
			} else {
				cf_sip_name_list->tail->next = l;
				cf_sip_name_list->tail = l->tail;
			}
		}
#line 1716 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 263 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_nis_list == NULL)
				cf_nis_list = (yyvsp[-1].list);
			else {
				cf_nis_list->tail->next = (yyvsp[-1].list);
				cf_nis_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 272 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, CFLISTENT_GENERIC, (yyvsp[-1].str), NULL);

			if (cf_nis_name_list == NULL) {
				cf_nis_name_list = l;
				cf_nis_name_list->tail = l;
				cf_nis_name_list->next = NULL;
			} else {
				cf_nis_name_list->tail->next = l;
				cf_nis_name_list->tail = l->tail;
			}
		}
#line 1748 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 287 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_nisp_list == NULL)
				cf_nisp_list = (yyvsp[-1].list);
			else {
				cf_nisp_list->tail->next = (yyvsp[-1].list);
				cf_nisp_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1761 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 296 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, CFLISTENT_GENERIC, (yyvsp[-1].str), NULL);

			if (cf_nisp_name_list == NULL) {
				cf_nisp_name_list = l;
				cf_nisp_name_list->tail = l;
				cf_nisp_name_list->next = NULL;
			} else {
				cf_nisp_name_list->tail->next = l;
				cf_nisp_name_list->tail = l->tail;
			}
		}
#line 1780 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 311 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_bcmcs_list == NULL)
				cf_bcmcs_list = (yyvsp[-1].list);
			else {
				cf_bcmcs_list->tail->next = (yyvsp[-1].list);
				cf_bcmcs_list->tail = (yyvsp[-1].list)->tail;
			}
		}
#line 1793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 320 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, CFLISTENT_GENERIC, (yyvsp[-1].str), NULL);

			if (cf_bcmcs_name_list == NULL) {
				cf_bcmcs_name_list = l;
				cf_bcmcs_name_list->tail = l;
				cf_bcmcs_name_list->next = NULL;
			} else {
				cf_bcmcs_name_list->tail->next = l;
				cf_bcmcs_name_list->tail = l->tail;
			}
		}
#line 1812 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 335 "cfparse.y" /* yacc.c:1646  */
    {
			if (cf_refreshtime == -1) {
				cf_refreshtime = (yyvsp[-1].num);
				if (cf_refreshtime < -1 ||
				    cf_refreshtime > 0xffffffff) {
					/*
					 * refresh time should not be negative
					 * according to the lex definition,
					 * but check it for safety.
					 */
					yyerror("refresh time is out of range");
				}
				if (cf_refreshtime < DHCP6_IRT_MINIMUM) {
					/*
					 * the value MUST NOT be smaller than
					 * IRT_MINIMUM.
					 */
					yyerror("refresh time is too small "
					    "(must not be smaller than %d)",
					    DHCP6_IRT_MINIMUM);
				}
			} else {
				yywarn("multiple refresh times (ignored)");
			}
		}
#line 1842 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 364 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_namelist *iapd;

			MAKE_NAMELIST(iapd, (yyvsp[-4].str), (yyvsp[-2].list));

			if (add_namelist(iapd, &iapdlist_head))
				return (-1);
		}
#line 1855 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 373 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_namelist *iapd;
			char *zero;

			if ((zero = strdup("0")) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			MAKE_NAMELIST(iapd, zero, (yyvsp[-2].list));

			if (add_namelist(iapd, &iapdlist_head))
				return (-1);
		}
#line 1873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 387 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_namelist *iana;

			MAKE_NAMELIST(iana, (yyvsp[-4].str), (yyvsp[-2].list));

			if (add_namelist(iana, &ianalist_head))
				return (-1);
		}
#line 1886 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 396 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_namelist *iana;
			char *zero;

			if ((zero = strdup("0")) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			MAKE_NAMELIST(iana, zero, (yyvsp[-2].list));

			if (add_namelist(iana, &ianalist_head))
				return (-1);
		}
#line 1904 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 413 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *authinfo;

		MAKE_NAMELIST(authinfo, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(authinfo, &authinfolist_head))
			return (-1);
	}
#line 1917 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 425 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *key;

		MAKE_NAMELIST(key, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(key, &keylist_head))
			return (-1);
	}
#line 1930 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 437 "cfparse.y" /* yacc.c:1646  */
    {
		if (cfswitch_buffer((yyvsp[-1].str))) {
			free((yyvsp[-1].str));
			return (-1);
		}
		free((yyvsp[-1].str));
	}
#line 1942 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 448 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_namelist *pool;

		MAKE_NAMELIST(pool, (yyvsp[-4].str), (yyvsp[-2].list));

		if (add_namelist(pool, &addrpoollist_head))
			return (-1);
	}
#line 1955 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 459 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 1961 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 461 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 1980 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 479 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_list *l;
		struct in6_addr a0, *a;

		if (inet_pton(AF_INET6, (yyvsp[0].str), &a0) != 1) {
			yywarn("invalid IPv6 address: %s", (yyvsp[0].str));
			free((yyvsp[0].str));
			return (-1);
		}
		if ((a = malloc(sizeof(*a))) == NULL) {
			yywarn("can't allocate memory");
			return (-1);
		}
		*a = a0;

		MAKE_CFLIST(l, CFLISTENT_GENERIC, a, NULL);

		(yyval.list) = l;
	}
#line 2004 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 501 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2010 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 503 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2029 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 521 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_SEND, NULL, (yyvsp[-1].list));

			(yyval.list) = l;
		}
#line 2041 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 529 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_REQUEST, NULL, (yyvsp[-1].list));

			(yyval.list) = l;
		}
#line 2053 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 537 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_INFO_ONLY, NULL, NULL);
			/* no value */
			(yyval.list) = l;
		}
#line 2065 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 545 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_ALLOW, NULL, (yyvsp[-1].list));

			(yyval.list) = l;
		}
#line 2077 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 553 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_DUID, (yyvsp[-1].str), NULL);

			(yyval.list) = l;
		}
#line 2089 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 561 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_ADDRESS, (yyvsp[-1].prefix),NULL);

			(yyval.list) = l;
		}
#line 2101 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 569 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_PREFIX, (yyvsp[-1].prefix), NULL);

			(yyval.list) = l;
		}
#line 2113 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 577 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_PREFERENCE, NULL, NULL);
			l->num = (yyvsp[-1].num);

			(yyval.list) = l;
		}
#line 2126 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 586 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_SCRIPT, (yyvsp[-1].str), NULL);

			(yyval.list) = l;
		}
#line 2138 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 594 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_DELAYEDKEY, (yyvsp[-1].str), NULL);

			(yyval.list) = l;
		}
#line 2150 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 602 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_RANGE, (yyvsp[-1].range), NULL);

			(yyval.list) = l;
		}
#line 2162 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 610 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DECL_ADDRESSPOOL, (yyvsp[-1].pool), NULL);

			(yyval.list) = l;
		}
#line 2174 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 621 "cfparse.y" /* yacc.c:1646  */
    {
			(yyval.list) = (yyvsp[0].list);
		}
#line 2182 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 625 "cfparse.y" /* yacc.c:1646  */
    {
			(yyvsp[-2].list)->next = (yyvsp[0].list);
			(yyvsp[-2].list)->tail = (yyvsp[0].list)->tail;

			(yyval.list) = (yyvsp[-2].list);
		}
#line 2193 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 635 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_RAPID_COMMIT, NULL, NULL);
			/* no value */
			(yyval.list) = l;
		}
#line 2205 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 643 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_AUTHINFO, NULL, NULL);
			l->ptr = (yyvsp[0].str);
			(yyval.list) = l;
		}
#line 2217 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 651 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_IA_PD, NULL, NULL);
			l->num = (yyvsp[0].num);
			(yyval.list) = l;
		}
#line 2229 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 659 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_IA_NA, NULL, NULL);
			l->num = (yyvsp[0].num);
			(yyval.list) = l;
		}
#line 2241 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 667 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_SIP, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2253 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 675 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_SIPNAME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2265 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 683 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_DNS, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2277 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 691 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_DNSNAME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2289 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 699 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_NTP, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2301 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 707 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_REFRESHTIME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2313 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 715 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_NIS, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2325 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 723 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_NISNAME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2337 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 731 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_NISP, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2349 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 739 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_NISPNAME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2361 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 747 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_BCMCS, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 755 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, DHCPOPT_BCMCSNAME, NULL, NULL);
			/* currently no value */
			(yyval.list) = l;
		}
#line 2385 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 766 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_range range0, *range;		

			memset(&range0, 0, sizeof(range0));
			if (inet_pton(AF_INET6, (yyvsp[-2].str), &range0.min) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[-2].str));
				free((yyvsp[-2].str));
				free((yyvsp[0].str));
				return (-1);
			}
			if (inet_pton(AF_INET6, (yyvsp[0].str), &range0.max) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[0].str));
				free((yyvsp[-2].str));
				free((yyvsp[0].str));
				return (-1);
			}
			free((yyvsp[-2].str));
			free((yyvsp[0].str));

			if ((range = malloc(sizeof(*range))) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			*range = range0;

			(yyval.range) = range;
		}
#line 2417 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 797 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_prefix pconf0, *pconf;		

			memset(&pconf0, 0, sizeof(pconf0));
			if (inet_pton(AF_INET6, (yyvsp[-1].str), &pconf0.addr) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[-1].str));
				free((yyvsp[-1].str));
				return (-1);
			}
			free((yyvsp[-1].str));
			/* validate other parameters later */
			pconf0.plen = 128; /* XXX this field is ignored */
			if ((yyvsp[0].num) < 0)
				pconf0.pltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.pltime = (u_int32_t)(yyvsp[0].num);
			pconf0.vltime = pconf0.pltime;

			if ((pconf = malloc(sizeof(*pconf))) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			*pconf = pconf0;

			(yyval.prefix) = pconf;
		}
#line 2448 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 824 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_prefix pconf0, *pconf;		

			memset(&pconf0, 0, sizeof(pconf0));
			if (inet_pton(AF_INET6, (yyvsp[-2].str), &pconf0.addr) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[-2].str));
				free((yyvsp[-2].str));
				return (-1);
			}
			free((yyvsp[-2].str));
			/* validate other parameters later */
			pconf0.plen = 128; /* XXX */
			if ((yyvsp[-1].num) < 0)
				pconf0.pltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.pltime = (u_int32_t)(yyvsp[-1].num);
			if ((yyvsp[0].num) < 0)
				pconf0.vltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.vltime = (u_int32_t)(yyvsp[0].num);

			if ((pconf = malloc(sizeof(*pconf))) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			*pconf = pconf0;

			(yyval.prefix) = pconf;
		}
#line 2482 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 857 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_prefix pconf0, *pconf;		

			memset(&pconf0, 0, sizeof(pconf0));
			if (inet_pton(AF_INET6, (yyvsp[-3].str), &pconf0.addr) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[-3].str));
				free((yyvsp[-3].str));
				return (-1);
			}
			free((yyvsp[-3].str));
			/* validate other parameters later */
			pconf0.plen = (yyvsp[-1].num);
			if ((yyvsp[0].num) < 0)
				pconf0.pltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.pltime = (u_int32_t)(yyvsp[0].num);
			pconf0.vltime = pconf0.pltime;

			if ((pconf = malloc(sizeof(*pconf))) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			*pconf = pconf0;

			(yyval.prefix) = pconf;
		}
#line 2513 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 884 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_prefix pconf0, *pconf;		

			memset(&pconf0, 0, sizeof(pconf0));
			if (inet_pton(AF_INET6, (yyvsp[-4].str), &pconf0.addr) != 1) {
				yywarn("invalid IPv6 address: %s", (yyvsp[-4].str));
				free((yyvsp[-4].str));
				return (-1);
			}
			free((yyvsp[-4].str));
			/* validate other parameters later */
			pconf0.plen = (yyvsp[-2].num);
			if ((yyvsp[-1].num) < 0)
				pconf0.pltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.pltime = (u_int32_t)(yyvsp[-1].num);
			if ((yyvsp[0].num) < 0)
				pconf0.vltime = DHCP6_DURATION_INFINITE;
			else
				pconf0.vltime = (u_int32_t)(yyvsp[0].num);

			if ((pconf = malloc(sizeof(*pconf))) == NULL) {
				yywarn("can't allocate memory");
				return (-1);
			}
			*pconf = pconf0;

			(yyval.prefix) = pconf;
		}
#line 2547 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 917 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_poolspec* pool;		

			if ((pool = malloc(sizeof(*pool))) == NULL) {
				yywarn("can't allocate memory");
				free((yyvsp[-1].str));
				return (-1);
			}
			if ((pool->name = strdup((yyvsp[-1].str))) == NULL) {
				yywarn("can't allocate memory");
				free((yyvsp[-1].str));
				return (-1);
			}
			free((yyvsp[-1].str));

			/* validate other parameters later */
			if ((yyvsp[0].num) < 0)
				pool->pltime = DHCP6_DURATION_INFINITE;
			else
				pool->pltime = (u_int32_t)(yyvsp[0].num);
			pool->vltime = pool->pltime;

			(yyval.pool) = pool;
		}
#line 2576 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 942 "cfparse.y" /* yacc.c:1646  */
    {
			struct dhcp6_poolspec* pool;		

			if ((pool = malloc(sizeof(*pool))) == NULL) {
				yywarn("can't allocate memory");
				free((yyvsp[-2].str));
				return (-1);
			}
			if ((pool->name = strdup((yyvsp[-2].str))) == NULL) {
				yywarn("can't allocate memory");
				free((yyvsp[-2].str));
				return (-1);
			}
			free((yyvsp[-2].str));

			/* validate other parameters later */
			if ((yyvsp[-1].num) < 0)
				pool->pltime = DHCP6_DURATION_INFINITE;
			else
				pool->pltime = (u_int32_t)(yyvsp[-1].num);
			if ((yyvsp[0].num) < 0)
				pool->vltime = DHCP6_DURATION_INFINITE;
			else
				pool->vltime = (u_int32_t)(yyvsp[0].num);

			(yyval.pool) = pool;
		}
#line 2608 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 973 "cfparse.y" /* yacc.c:1646  */
    {
			(yyval.num) = -1;
		}
#line 2616 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 977 "cfparse.y" /* yacc.c:1646  */
    {
			(yyval.num) = (yyvsp[0].num);
		}
#line 2624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 983 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2630 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 985 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 1002 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = (yyvsp[0].list); }
#line 2655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 1004 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, IACONF_PREFIX, (yyvsp[-1].prefix), NULL);

			(yyval.list) = l;
		}
#line 2667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 1015 "cfparse.y" /* yacc.c:1646  */
    {
		struct cf_list *ifl;

		MAKE_CFLIST(ifl, IACONF_PIF, (yyvsp[-4].str), (yyvsp[-2].list));
		(yyval.list) = ifl;
	}
#line 2678 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 1024 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2684 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 1026 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2703 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1044 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, IFPARAM_SLA_ID, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1052 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, IFPARAM_SLA_LEN, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1060 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, IFPARAM_IFID, NULL, NULL);
			l->num = (u_int64_t)(yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1070 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1072 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2764 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1090 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, IACONF_ADDR, (yyvsp[-1].prefix), NULL);

			(yyval.list) = l;
		}
#line 2776 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1100 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2782 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1102 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1120 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, AUTHPARAM_PROTO, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2813 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1128 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, AUTHPARAM_ALG, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2825 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1136 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, AUTHPARAM_RDM, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1144 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, AUTHPARAM_KEY, NULL, NULL);
			l->ptr = (yyvsp[-1].str);
			(yyval.list) = l;
		}
#line 2849 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1154 "cfparse.y" /* yacc.c:1646  */
    { (yyval.num) = DHCP6_AUTHPROTO_DELAYED; }
#line 2855 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1155 "cfparse.y" /* yacc.c:1646  */
    { (yyval.num) = DHCP6_AUTHPROTO_RECONFIG; }
#line 2861 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1159 "cfparse.y" /* yacc.c:1646  */
    { (yyval.num) = DHCP6_AUTHALG_HMACMD5; }
#line 2867 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1163 "cfparse.y" /* yacc.c:1646  */
    { (yyval.num) = DHCP6_AUTHRDM_MONOCOUNTER; }
#line 2873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1167 "cfparse.y" /* yacc.c:1646  */
    { (yyval.list) = NULL; }
#line 2879 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1169 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *head;

			if ((head = (yyvsp[-1].list)) == NULL) {
				(yyvsp[0].list)->next = NULL;
				(yyvsp[0].list)->tail = (yyvsp[0].list);
				head = (yyvsp[0].list);
			} else {
				head->tail->next = (yyvsp[0].list);
				head->tail = (yyvsp[0].list)->tail;
			}

			(yyval.list) = head;
		}
#line 2898 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1187 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, KEYPARAM_REALM, NULL, NULL);
			l->ptr = (yyvsp[-1].str);
			(yyval.list) = l;
		}
#line 2910 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1195 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, KEYPARAM_KEYID, NULL, NULL);
			l->num = (yyvsp[-1].num);
			(yyval.list) = l;
		}
#line 2922 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1203 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, KEYPARAM_SECRET, NULL, NULL);
			l->ptr = (yyvsp[-1].str);
			(yyval.list) = l;
		}
#line 2934 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1211 "cfparse.y" /* yacc.c:1646  */
    {
			struct cf_list *l;

			MAKE_CFLIST(l, KEYPARAM_EXPIRE, NULL, NULL);
			l->ptr = (yyvsp[-1].str);
			(yyval.list) = l;
		}
#line 2946 "y.tab.c" /* yacc.c:1646  */
    break;


#line 2950 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1220 "cfparse.y" /* yacc.c:1906  */

/* supplement routines for configuration */
static int
add_namelist(new, headp)
	struct cf_namelist *new, **headp;
{
	struct cf_namelist *n;
	
	/* check for duplicated configuration */
	for (n = *headp; n; n = n->next) {
		if (strcmp(n->name, new->name) == 0) {
			yywarn("duplicated name: %s (ignored)",
			       new->name);
			cleanup_namelist(new);
			return (0);
		}
	}

	new->next = *headp;
	*headp = new;

	return (0);
}

/* free temporary resources */
static void
cleanup()
{
	cleanup_namelist(iflist_head);
	iflist_head = NULL;
	cleanup_namelist(profilelist_head);
	profilelist_head = NULL;
	cleanup_namelist(hostlist_head);
	hostlist_head = NULL;
	cleanup_namelist(iapdlist_head);
	iapdlist_head = NULL;
	cleanup_namelist(ianalist_head);
	ianalist_head = NULL;
	cleanup_namelist(authinfolist_head);
	authinfolist_head = NULL;
	cleanup_namelist(keylist_head);
	keylist_head = NULL;
	cleanup_namelist(addrpoollist_head);
	addrpoollist_head = NULL;

	cleanup_cflist(cf_sip_list);
	cf_sip_list = NULL;
	cleanup_cflist(cf_sip_name_list);
	cf_sip_name_list = NULL;
	cleanup_cflist(cf_dns_list);
	cf_dns_list = NULL;
	cleanup_cflist(cf_dns_name_list);
	cf_dns_name_list = NULL;
	cleanup_cflist(cf_ntp_list);
	cf_ntp_list = NULL;
	cleanup_cflist(cf_nis_list);
	cf_nis_list = NULL;
	cleanup_cflist(cf_nis_name_list);
	cf_nis_name_list = NULL;
	cleanup_cflist(cf_nisp_list);
	cf_nisp_list = NULL;
	cleanup_cflist(cf_nisp_name_list);
	cf_nisp_name_list = NULL;
	cleanup_cflist(cf_bcmcs_list);
	cf_bcmcs_list = NULL;
	cleanup_cflist(cf_bcmcs_name_list);
	cf_bcmcs_name_list = NULL;
}

static void
cleanup_namelist(head)
	struct cf_namelist *head;
{
	struct cf_namelist *ifp, *ifp_next;

	for (ifp = head; ifp; ifp = ifp_next) {
		ifp_next = ifp->next;
		cleanup_cflist(ifp->params);
		free(ifp->name);
		free(ifp);
	}
}

static void
cleanup_cflist(p)
	struct cf_list *p;
{
	struct cf_list *n;

	if (p == NULL)
		return;

	n = p->next;
	if (p->type == DECL_ADDRESSPOOL) {
		free(((struct dhcp6_poolspec *)p->ptr)->name);
	}
	if (p->ptr)
		free(p->ptr);
	if (p->list)
		cleanup_cflist(p->list);
	free(p);

	cleanup_cflist(n);
}

#define config_fail() \
	do { cleanup(); configure_cleanup(); return (-1); } while(0)

int
cf_post_config()
{
	if (configure_keys(keylist_head))
		config_fail();

	if (configure_authinfo(authinfolist_head))
		config_fail();

	if (configure_ia(iapdlist_head, IATYPE_PD))
		config_fail();

	if (configure_ia(ianalist_head, IATYPE_NA))
		config_fail();

	if (configure_pool(addrpoollist_head))
		config_fail();

	if (configure_profile(profilelist_head))
		config_fail();

	if (configure_interface(iflist_head))
		config_fail();

	if (configure_host(hostlist_head))
		config_fail();

	if (configure_global_option())
		config_fail();

	configure_commit();
	cleanup();
	return (0);
}
#undef config_fail

void
cf_init()
{
	iflist_head = NULL;
	profilelist_head = NULL;
}
