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
#line 15 "gram.y" /* yacc.c:339  */

#include "config.h"
#include "includes.h"
#include "radvd.h"
#include "defaults.h"

#define YYERROR_VERBOSE 1
static int countbits(int b);
static int count_mask(struct sockaddr_in6 *m);
static struct in6_addr get_prefix6(struct in6_addr const *addr, struct in6_addr const *mask);

#if 0 /* no longer necessary? */
#ifndef HAVE_IN6_ADDR_S6_ADDR
# ifdef __FreeBSD__
#  define s6_addr32 __u6_addr.__u6_addr32
#  define s6_addr16 __u6_addr.__u6_addr16
# endif
#endif
#endif

#define ADD_TO_LL(type, list, value) \
	do { \
		if (iface->list == NULL) \
			iface->list = value; \
		else { \
			type *current = iface->list; \
			while (current->next != NULL) \
				current = current->next; \
			current->next = value; \
		} \
	} while (0)


#line 100 "gram.c" /* yacc.c:339  */

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
#ifndef YY_YY_GRAM_H_INCLUDED
# define YY_YY_GRAM_H_INCLUDED
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
    T_INTERFACE = 258,
    T_PREFIX = 259,
    T_ROUTE = 260,
    T_RDNSS = 261,
    T_DNSSL = 262,
    T_CLIENTS = 263,
    T_LOWPANCO = 264,
    T_ABRO = 265,
    STRING = 266,
    NUMBER = 267,
    SIGNEDNUMBER = 268,
    DECIMAL = 269,
    SWITCH = 270,
    IPV6ADDR = 271,
    INFINITY = 272,
    T_IgnoreIfMissing = 273,
    T_AdvSendAdvert = 274,
    T_MaxRtrAdvInterval = 275,
    T_MinRtrAdvInterval = 276,
    T_MinDelayBetweenRAs = 277,
    T_AdvManagedFlag = 278,
    T_AdvOtherConfigFlag = 279,
    T_AdvLinkMTU = 280,
    T_AdvReachableTime = 281,
    T_AdvRetransTimer = 282,
    T_AdvCurHopLimit = 283,
    T_AdvDefaultLifetime = 284,
    T_AdvDefaultPreference = 285,
    T_AdvSourceLLAddress = 286,
    T_AdvOnLink = 287,
    T_AdvAutonomous = 288,
    T_AdvValidLifetime = 289,
    T_AdvPreferredLifetime = 290,
    T_DeprecatePrefix = 291,
    T_DecrementLifetimes = 292,
    T_AdvRouterAddr = 293,
    T_AdvHomeAgentFlag = 294,
    T_AdvIntervalOpt = 295,
    T_AdvHomeAgentInfo = 296,
    T_Base6Interface = 297,
    T_Base6to4Interface = 298,
    T_UnicastOnly = 299,
    T_HomeAgentPreference = 300,
    T_HomeAgentLifetime = 301,
    T_AdvRoutePreference = 302,
    T_AdvRouteLifetime = 303,
    T_RemoveRoute = 304,
    T_AdvRDNSSPreference = 305,
    T_AdvRDNSSOpenFlag = 306,
    T_AdvRDNSSLifetime = 307,
    T_FlushRDNSS = 308,
    T_AdvDNSSLLifetime = 309,
    T_FlushDNSSL = 310,
    T_AdvMobRtrSupportFlag = 311,
    T_AdvContextLength = 312,
    T_AdvContextCompressionFlag = 313,
    T_AdvContextID = 314,
    T_AdvLifeTime = 315,
    T_AdvContextPrefix = 316,
    T_AdvVersionLow = 317,
    T_AdvVersionHigh = 318,
    T_AdvValidLifeTime = 319,
    T_Adv6LBRaddress = 320,
    T_BAD_TOKEN = 321
  };
#endif
/* Tokens.  */
#define T_INTERFACE 258
#define T_PREFIX 259
#define T_ROUTE 260
#define T_RDNSS 261
#define T_DNSSL 262
#define T_CLIENTS 263
#define T_LOWPANCO 264
#define T_ABRO 265
#define STRING 266
#define NUMBER 267
#define SIGNEDNUMBER 268
#define DECIMAL 269
#define SWITCH 270
#define IPV6ADDR 271
#define INFINITY 272
#define T_IgnoreIfMissing 273
#define T_AdvSendAdvert 274
#define T_MaxRtrAdvInterval 275
#define T_MinRtrAdvInterval 276
#define T_MinDelayBetweenRAs 277
#define T_AdvManagedFlag 278
#define T_AdvOtherConfigFlag 279
#define T_AdvLinkMTU 280
#define T_AdvReachableTime 281
#define T_AdvRetransTimer 282
#define T_AdvCurHopLimit 283
#define T_AdvDefaultLifetime 284
#define T_AdvDefaultPreference 285
#define T_AdvSourceLLAddress 286
#define T_AdvOnLink 287
#define T_AdvAutonomous 288
#define T_AdvValidLifetime 289
#define T_AdvPreferredLifetime 290
#define T_DeprecatePrefix 291
#define T_DecrementLifetimes 292
#define T_AdvRouterAddr 293
#define T_AdvHomeAgentFlag 294
#define T_AdvIntervalOpt 295
#define T_AdvHomeAgentInfo 296
#define T_Base6Interface 297
#define T_Base6to4Interface 298
#define T_UnicastOnly 299
#define T_HomeAgentPreference 300
#define T_HomeAgentLifetime 301
#define T_AdvRoutePreference 302
#define T_AdvRouteLifetime 303
#define T_RemoveRoute 304
#define T_AdvRDNSSPreference 305
#define T_AdvRDNSSOpenFlag 306
#define T_AdvRDNSSLifetime 307
#define T_FlushRDNSS 308
#define T_AdvDNSSLLifetime 309
#define T_FlushDNSSL 310
#define T_AdvMobRtrSupportFlag 311
#define T_AdvContextLength 312
#define T_AdvContextCompressionFlag 313
#define T_AdvContextID 314
#define T_AdvLifeTime 315
#define T_AdvContextPrefix 316
#define T_AdvVersionLow 317
#define T_AdvVersionHigh 318
#define T_AdvValidLifeTime 319
#define T_Adv6LBRaddress 320
#define T_BAD_TOKEN 321

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 137 "gram.y" /* yacc.c:355  */

	unsigned int		num;
	int			snum;
	double			dec;
	struct in6_addr		*addr;
	char			*str;
	struct AdvPrefix	*pinfo;
	struct AdvRoute		*rinfo;
	struct AdvRDNSS		*rdnssinfo;
	struct AdvDNSSL		*dnsslinfo;
	struct Clients		*ainfo;
	struct AdvLowpanCo	*lowpancoinfo;
	struct AdvAbro		*abroinfo;

#line 287 "gram.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAM_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 152 "gram.y" /* yacc.c:358  */

extern int num_lines;
static char const * filename;
static struct Interface *iface;
static struct Interface *IfaceList;
static struct AdvPrefix *prefix;
static struct AdvRoute *route;
static struct AdvRDNSS *rdnss;
static struct AdvDNSSL *dnssl;
static struct AdvLowpanCo *lowpanco;
static struct AdvAbro  *abro;
static void cleanup(void);
#define ABORT	do { cleanup(); YYABORT; } while (0);
static void yyerror(char const * msg);

#line 319 "gram.c" /* yacc.c:358  */

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
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   234

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  113
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  249

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   321

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
       2,     2,     2,     2,     2,     2,     2,    70,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    69,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,     2,    68,     2,     2,     2,     2,
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
      65,    66
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   171,   171,   172,   175,   185,   214,   221,   222,   225,
     226,   227,   228,   229,   230,   231,   232,   235,   239,   243,
     247,   251,   255,   259,   263,   267,   271,   275,   279,   283,
     287,   291,   295,   299,   303,   307,   311,   315,   319,   323,
     327,   333,   339,   350,   365,   463,   560,   561,   562,   565,
     566,   569,   583,   597,   606,   623,   640,   646,   652,   665,
     679,   687,   711,   712,   715,   716,   720,   724,   728,   734,
     741,   742,   745,   780,   789,   790,   793,   794,   798,   802,
     806,   821,   827,   834,   835,   838,   880,   889,   890,   893,
     894,   898,   910,   916,   923,   936,   937,   940,   941,   944,
     948,   952,   956,   962,   969,   989,   990,   993,   994,   997,
    1001,  1005,  1011,  1015
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_INTERFACE", "T_PREFIX", "T_ROUTE",
  "T_RDNSS", "T_DNSSL", "T_CLIENTS", "T_LOWPANCO", "T_ABRO", "STRING",
  "NUMBER", "SIGNEDNUMBER", "DECIMAL", "SWITCH", "IPV6ADDR", "INFINITY",
  "T_IgnoreIfMissing", "T_AdvSendAdvert", "T_MaxRtrAdvInterval",
  "T_MinRtrAdvInterval", "T_MinDelayBetweenRAs", "T_AdvManagedFlag",
  "T_AdvOtherConfigFlag", "T_AdvLinkMTU", "T_AdvReachableTime",
  "T_AdvRetransTimer", "T_AdvCurHopLimit", "T_AdvDefaultLifetime",
  "T_AdvDefaultPreference", "T_AdvSourceLLAddress", "T_AdvOnLink",
  "T_AdvAutonomous", "T_AdvValidLifetime", "T_AdvPreferredLifetime",
  "T_DeprecatePrefix", "T_DecrementLifetimes", "T_AdvRouterAddr",
  "T_AdvHomeAgentFlag", "T_AdvIntervalOpt", "T_AdvHomeAgentInfo",
  "T_Base6Interface", "T_Base6to4Interface", "T_UnicastOnly",
  "T_HomeAgentPreference", "T_HomeAgentLifetime", "T_AdvRoutePreference",
  "T_AdvRouteLifetime", "T_RemoveRoute", "T_AdvRDNSSPreference",
  "T_AdvRDNSSOpenFlag", "T_AdvRDNSSLifetime", "T_FlushRDNSS",
  "T_AdvDNSSLLifetime", "T_FlushDNSSL", "T_AdvMobRtrSupportFlag",
  "T_AdvContextLength", "T_AdvContextCompressionFlag", "T_AdvContextID",
  "T_AdvLifeTime", "T_AdvContextPrefix", "T_AdvVersionLow",
  "T_AdvVersionHigh", "T_AdvValidLifeTime", "T_Adv6LBRaddress",
  "T_BAD_TOKEN", "'{'", "'}'", "';'", "'/'", "$accept", "grammar",
  "ifacedef", "ifacehead", "name", "ifaceparams", "ifaceparam", "ifaceval",
  "clientslist", "v6addrlist", "prefixdef", "prefixhead",
  "optional_prefixplist", "prefixplist", "prefixparms", "routedef",
  "routehead", "optional_routeplist", "routeplist", "routeparms",
  "rdnssdef", "rdnssaddrs", "rdnssaddr", "rdnsshead",
  "optional_rdnssplist", "rdnssplist", "rdnssparms", "dnssldef",
  "dnsslsuffixes", "dnsslsuffix", "dnsslhead", "optional_dnsslplist",
  "dnsslplist", "dnsslparms", "lowpancodef", "lowpancohead",
  "optional_lowpancoplist", "lowpancoplist", "lowpancoparms", "abrodef",
  "abrohead", "optional_abroplist", "abroplist", "abroparms",
  "number_or_infinity", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,   123,   125,    59,
      47
};
# endif

#define YYPACT_NINF -124

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-124)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      31,    27,    55,  -124,   -16,  -124,  -124,  -124,  -124,  -124,
      -4,    40,    44,    73,    76,    21,  -124,    74,    77,    78,
      49,    71,    72,    79,    80,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    98,    99,    97,    22,
    -124,  -124,  -124,  -124,    36,  -124,    37,  -124,    46,  -124,
      47,  -124,    48,  -124,    50,    51,    52,  -124,    73,  -124,
    -124,    76,  -124,   100,    53,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      75,    81,    82,    83,    95,    96,   101,   102,   103,  -124,
      11,   104,    25,   -42,    23,   -28,    18,   106,   107,  -124,
    -124,   105,    -9,   108,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,   109,   125,
      16,    16,   126,   127,   128,    27,    27,  -124,    33,  -124,
    -124,   132,    16,   131,   110,    25,  -124,   135,   133,    16,
     134,   111,   -42,  -124,    16,   138,   112,    23,  -124,   142,
     140,   144,   145,   113,   -28,  -124,   146,   147,   148,   114,
      18,  -124,  -124,  -124,  -124,   115,   116,  -124,   117,   118,
    -124,  -124,   119,   120,   121,   122,   123,   124,   129,  -124,
    -124,   130,   136,   137,   139,  -124,   141,   143,   149,   150,
     151,  -124,   152,   153,   154,  -124,   155,   156,   157,   158,
     159,  -124,   160,   161,   162,   163,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     3,     0,     6,     5,     1,     2,     8,
       0,     0,     0,     0,     0,     0,    94,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     9,    11,    10,    46,    12,     0,    13,     0,    14,
       0,    15,     0,    16,     0,     0,     0,    72,    73,    71,
      85,    86,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     0,    62,    74,    87,    95,   105,     0,     0,    70,
      83,     0,     0,     0,    23,    24,    18,    21,    17,    20,
      19,    22,    25,    26,    27,    28,    29,    32,    30,    31,
      33,    36,    34,    35,    39,    37,    38,    40,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,     0,    50,
      44,     0,     0,     0,     0,    63,    65,     0,     0,     0,
       0,     0,    75,    77,     0,     0,     0,    88,    90,     0,
       0,     0,     0,     0,    96,    98,     0,     0,     0,     0,
     106,   108,    45,    61,    42,     0,     0,   104,     0,     0,
     112,   113,     0,     0,     0,     0,     0,     0,     0,    48,
      49,     0,     0,     0,     0,    64,     0,     0,     0,     0,
       0,    76,     0,     0,     0,    89,     0,     0,     0,     0,
       0,    97,     0,     0,     0,     0,   107,    43,    41,    51,
      52,    54,    55,    56,    57,    53,    58,    59,    66,    67,
      68,    60,    78,    79,    80,    81,    69,    91,    92,    82,
      99,   100,   101,   102,    93,   109,   110,   111,   103
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -124,  -124,   164,  -124,  -123,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,    24,  -124,  -124,  -124,  -124,    30,
    -124,  -124,   175,  -124,  -124,  -124,     9,  -124,  -124,   173,
    -124,  -124,  -124,     6,  -124,  -124,  -124,  -124,     3,  -124,
    -124,  -124,  -124,    -2,   -92
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     6,    10,    40,    41,    42,   102,
      43,    44,    91,   138,   139,    45,    46,   144,   145,   146,
      47,    58,    59,    48,   151,   152,   153,    49,    61,    62,
      50,   156,   157,   158,    51,    52,   163,   164,   165,    53,
      54,   169,   170,   171,   182
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      11,    12,    13,    14,    15,    16,    17,   175,   147,   148,
     149,   150,   187,   188,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,   180,   159,
     160,   161,   162,   181,     1,    32,    33,    34,     5,   183,
      35,    36,    37,   128,   129,   130,   131,   132,   133,   134,
     192,     9,    38,   135,   136,     7,    55,   198,     1,   176,
      56,    67,   202,    68,    39,   128,   129,   130,   131,   132,
     133,   134,   141,   142,   143,   135,   136,   154,   155,   137,
     166,   167,   168,    69,    71,    70,    72,    60,    63,    57,
      64,    89,    65,    66,    73,    74,    75,    76,    77,    78,
      79,   189,    80,    90,    92,    81,    82,    83,    84,    85,
      86,    87,    88,    93,    94,    95,   101,    96,   172,   173,
     177,    97,    98,   103,   178,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     179,   184,   185,   186,   119,   191,   193,   196,   197,   199,
     120,   121,   122,   203,   206,   207,   208,   209,   212,   213,
     214,   201,   190,   205,   123,   124,     8,   211,   216,     0,
     125,   126,   127,   140,   174,   195,     0,     0,   194,   200,
     204,   210,   215,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,     0,     0,     0,     0,   227,   228,
       0,     0,     0,     0,     0,   229,   230,     0,   231,     0,
     232,     0,   233,     0,     0,     0,     0,     0,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,    99,   100
};

static const yytype_int16 yycheck[] =
{
       4,     5,     6,     7,     8,     9,    10,    16,    50,    51,
      52,    53,   135,   136,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    12,    57,
      58,    59,    60,    17,     3,    39,    40,    41,    11,   131,
      44,    45,    46,    32,    33,    34,    35,    36,    37,    38,
     142,    67,    56,    42,    43,     0,    16,   149,     3,    68,
      16,    12,   154,    14,    68,    32,    33,    34,    35,    36,
      37,    38,    47,    48,    49,    42,    43,    54,    55,    68,
      62,    63,    64,    12,    12,    14,    14,    11,    67,    16,
      16,    69,    15,    15,    15,    15,    12,    12,    12,    12,
      12,    68,    13,    67,    67,    15,    15,    15,    15,    15,
      12,    12,    15,    67,    67,    67,    16,    67,    12,    12,
      12,    70,    70,    70,    15,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      15,    15,    15,    15,    69,    13,    15,    12,    15,    15,
      69,    69,    69,    15,    12,    15,    12,    12,    12,    12,
      12,   152,   138,   157,    69,    69,     2,   164,   170,    -1,
      69,    69,    69,    69,    69,   145,    -1,    -1,    68,    68,
      68,    68,    68,    -1,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    -1,    -1,    -1,    -1,    69,    69,
      -1,    -1,    -1,    -1,    -1,    69,    69,    -1,    69,    -1,
      69,    -1,    69,    -1,    -1,    -1,    -1,    -1,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    58,    61
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    72,    73,    74,    11,    75,     0,    73,    67,
      76,     4,     5,     6,     7,     8,     9,    10,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    39,    40,    41,    44,    45,    46,    56,    68,
      77,    78,    79,    81,    82,    86,    87,    91,    94,    98,
     101,   105,   106,   110,   111,    16,    16,    16,    92,    93,
      11,    99,   100,    67,    16,    15,    15,    12,    14,    12,
      14,    12,    14,    15,    15,    12,    12,    12,    12,    12,
      13,    15,    15,    15,    15,    15,    12,    12,    15,    69,
      67,    83,    67,    67,    67,    67,    67,    70,    70,    93,
     100,    16,    80,    70,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    32,    33,
      34,    35,    36,    37,    38,    42,    43,    68,    84,    85,
      69,    47,    48,    49,    88,    89,    90,    50,    51,    52,
      53,    95,    96,    97,    54,    55,   102,   103,   104,    57,
      58,    59,    60,   107,   108,   109,    62,    63,    64,   112,
     113,   114,    12,    12,    69,    16,    68,    12,    15,    15,
      12,    17,   115,   115,    15,    15,    15,    75,    75,    68,
      85,    13,   115,    15,    68,    90,    12,    15,   115,    15,
      68,    97,   115,    15,    68,   104,    12,    15,    12,    12,
      68,   109,    12,    12,    12,    68,   114,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    71,    72,    72,    73,    74,    75,    76,    76,    77,
      77,    77,    77,    77,    77,    77,    77,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    79,    80,    80,    81,    82,    83,    83,    83,    84,
      84,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      86,    87,    88,    88,    89,    89,    90,    90,    90,    91,
      92,    92,    93,    94,    95,    95,    96,    96,    97,    97,
      97,    97,    98,    99,    99,   100,   101,   102,   102,   103,
     103,   104,   104,   105,   106,   107,   107,   108,   108,   109,
     109,   109,   109,   110,   111,   112,   112,   113,   113,   114,
     114,   114,   115,   115
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     5,     2,     1,     2,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     2,     3,     3,     4,     0,     2,     3,     2,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     4,     0,     1,     2,     1,     3,     3,     3,     5,
       2,     1,     1,     2,     0,     1,     2,     1,     3,     3,
       3,     3,     5,     2,     1,     1,     2,     0,     1,     2,
       1,     3,     3,     5,     1,     0,     1,     2,     1,     3,
       3,     3,     3,     5,     4,     0,     1,     2,     1,     3,
       3,     3,     1,     1
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
        case 4:
#line 176 "gram.y" /* yacc.c:1646  */
    {
			dlog(LOG_DEBUG, 4, "%s interface definition ok", iface->props.name);

			iface->next = IfaceList;
			IfaceList = iface;

			iface = NULL;
		}
#line 1593 "gram.c" /* yacc.c:1646  */
    break;

  case 5:
#line 186 "gram.y" /* yacc.c:1646  */
    {
			iface = IfaceList;

			while (iface)
			{
				if (!strcmp((yyvsp[0].str), iface->props.name))
				{
					flog(LOG_ERR, "duplicate interface "
						"definition for %s", (yyvsp[0].str));
					ABORT;
				}
				iface = iface->next;
			}

			iface = malloc(sizeof(struct Interface));

			if (iface == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			iface_init_defaults(iface);
			strncpy(iface->props.name, (yyvsp[0].str), IFNAMSIZ-1);
			iface->props.name[IFNAMSIZ-1] = '\0';
			iface->lineno = num_lines;
		}
#line 1624 "gram.c" /* yacc.c:1646  */
    break;

  case 6:
#line 215 "gram.y" /* yacc.c:1646  */
    {
			/* check vality */
			(yyval.str) = (yyvsp[0].str);
		}
#line 1633 "gram.c" /* yacc.c:1646  */
    break;

  case 10:
#line 226 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvPrefix, AdvPrefixList, (yyvsp[0].pinfo)); }
#line 1639 "gram.c" /* yacc.c:1646  */
    break;

  case 11:
#line 227 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct Clients, ClientList, (yyvsp[0].ainfo)); }
#line 1645 "gram.c" /* yacc.c:1646  */
    break;

  case 12:
#line 228 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvRoute, AdvRouteList, (yyvsp[0].rinfo)); }
#line 1651 "gram.c" /* yacc.c:1646  */
    break;

  case 13:
#line 229 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvRDNSS, AdvRDNSSList, (yyvsp[0].rdnssinfo)); }
#line 1657 "gram.c" /* yacc.c:1646  */
    break;

  case 14:
#line 230 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvDNSSL, AdvDNSSLList, (yyvsp[0].dnsslinfo)); }
#line 1663 "gram.c" /* yacc.c:1646  */
    break;

  case 15:
#line 231 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvLowpanCo, AdvLowpanCoList, (yyvsp[0].lowpancoinfo)); }
#line 1669 "gram.c" /* yacc.c:1646  */
    break;

  case 16:
#line 232 "gram.y" /* yacc.c:1646  */
    { ADD_TO_LL(struct AdvAbro, AdvAbroList, (yyvsp[0].abroinfo)); }
#line 1675 "gram.c" /* yacc.c:1646  */
    break;

  case 17:
#line 236 "gram.y" /* yacc.c:1646  */
    {
			iface->MinRtrAdvInterval = (yyvsp[-1].num);
		}
#line 1683 "gram.c" /* yacc.c:1646  */
    break;

  case 18:
#line 240 "gram.y" /* yacc.c:1646  */
    {
			iface->MaxRtrAdvInterval = (yyvsp[-1].num);
		}
#line 1691 "gram.c" /* yacc.c:1646  */
    break;

  case 19:
#line 244 "gram.y" /* yacc.c:1646  */
    {
			iface->MinDelayBetweenRAs = (yyvsp[-1].num);
		}
#line 1699 "gram.c" /* yacc.c:1646  */
    break;

  case 20:
#line 248 "gram.y" /* yacc.c:1646  */
    {
			iface->MinRtrAdvInterval = (yyvsp[-1].dec);
		}
#line 1707 "gram.c" /* yacc.c:1646  */
    break;

  case 21:
#line 252 "gram.y" /* yacc.c:1646  */
    {
			iface->MaxRtrAdvInterval = (yyvsp[-1].dec);
		}
#line 1715 "gram.c" /* yacc.c:1646  */
    break;

  case 22:
#line 256 "gram.y" /* yacc.c:1646  */
    {
			iface->MinDelayBetweenRAs = (yyvsp[-1].dec);
		}
#line 1723 "gram.c" /* yacc.c:1646  */
    break;

  case 23:
#line 260 "gram.y" /* yacc.c:1646  */
    {
			iface->IgnoreIfMissing = (yyvsp[-1].num);
		}
#line 1731 "gram.c" /* yacc.c:1646  */
    break;

  case 24:
#line 264 "gram.y" /* yacc.c:1646  */
    {
			iface->AdvSendAdvert = (yyvsp[-1].num);
		}
#line 1739 "gram.c" /* yacc.c:1646  */
    break;

  case 25:
#line 268 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvManagedFlag = (yyvsp[-1].num);
		}
#line 1747 "gram.c" /* yacc.c:1646  */
    break;

  case 26:
#line 272 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvOtherConfigFlag = (yyvsp[-1].num);
		}
#line 1755 "gram.c" /* yacc.c:1646  */
    break;

  case 27:
#line 276 "gram.y" /* yacc.c:1646  */
    {
			iface->AdvLinkMTU = (yyvsp[-1].num);
		}
#line 1763 "gram.c" /* yacc.c:1646  */
    break;

  case 28:
#line 280 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvReachableTime = (yyvsp[-1].num);
		}
#line 1771 "gram.c" /* yacc.c:1646  */
    break;

  case 29:
#line 284 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvRetransTimer = (yyvsp[-1].num);
		}
#line 1779 "gram.c" /* yacc.c:1646  */
    break;

  case 30:
#line 288 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvDefaultLifetime = (yyvsp[-1].num);
		}
#line 1787 "gram.c" /* yacc.c:1646  */
    break;

  case 31:
#line 292 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvDefaultPreference = (yyvsp[-1].snum);
		}
#line 1795 "gram.c" /* yacc.c:1646  */
    break;

  case 32:
#line 296 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvCurHopLimit = (yyvsp[-1].num);
		}
#line 1803 "gram.c" /* yacc.c:1646  */
    break;

  case 33:
#line 300 "gram.y" /* yacc.c:1646  */
    {
			iface->AdvSourceLLAddress = (yyvsp[-1].num);
		}
#line 1811 "gram.c" /* yacc.c:1646  */
    break;

  case 34:
#line 304 "gram.y" /* yacc.c:1646  */
    {
			iface->mipv6.AdvIntervalOpt = (yyvsp[-1].num);
		}
#line 1819 "gram.c" /* yacc.c:1646  */
    break;

  case 35:
#line 308 "gram.y" /* yacc.c:1646  */
    {
			iface->mipv6.AdvHomeAgentInfo = (yyvsp[-1].num);
		}
#line 1827 "gram.c" /* yacc.c:1646  */
    break;

  case 36:
#line 312 "gram.y" /* yacc.c:1646  */
    {
			iface->ra_header_info.AdvHomeAgentFlag = (yyvsp[-1].num);
		}
#line 1835 "gram.c" /* yacc.c:1646  */
    break;

  case 37:
#line 316 "gram.y" /* yacc.c:1646  */
    {
			iface->mipv6.HomeAgentPreference = (yyvsp[-1].num);
		}
#line 1843 "gram.c" /* yacc.c:1646  */
    break;

  case 38:
#line 320 "gram.y" /* yacc.c:1646  */
    {
			iface->mipv6.HomeAgentLifetime = (yyvsp[-1].num);
		}
#line 1851 "gram.c" /* yacc.c:1646  */
    break;

  case 39:
#line 324 "gram.y" /* yacc.c:1646  */
    {
			iface->UnicastOnly = (yyvsp[-1].num);
		}
#line 1859 "gram.c" /* yacc.c:1646  */
    break;

  case 40:
#line 328 "gram.y" /* yacc.c:1646  */
    {
			iface->mipv6.AdvMobRtrSupportFlag = (yyvsp[-1].num);
		}
#line 1867 "gram.c" /* yacc.c:1646  */
    break;

  case 41:
#line 334 "gram.y" /* yacc.c:1646  */
    {
			(yyval.ainfo) = (yyvsp[-2].ainfo);
		}
#line 1875 "gram.c" /* yacc.c:1646  */
    break;

  case 42:
#line 340 "gram.y" /* yacc.c:1646  */
    {
			struct Clients *new = calloc(1, sizeof(struct Clients));
			if (new == NULL) {
				flog(LOG_CRIT, "calloc failed: %s", strerror(errno));
				ABORT;
			}

			memcpy(&(new->Address), (yyvsp[-1].addr), sizeof(struct in6_addr));
			(yyval.ainfo) = new;
		}
#line 1890 "gram.c" /* yacc.c:1646  */
    break;

  case 43:
#line 351 "gram.y" /* yacc.c:1646  */
    {
			struct Clients *new = calloc(1, sizeof(struct Clients));
			if (new == NULL) {
				flog(LOG_CRIT, "calloc failed: %s", strerror(errno));
				ABORT;
			}

			memcpy(&(new->Address), (yyvsp[-1].addr), sizeof(struct in6_addr));
			new->next = (yyvsp[-2].ainfo);
			(yyval.ainfo) = new;
		}
#line 1906 "gram.c" /* yacc.c:1646  */
    break;

  case 44:
#line 366 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				unsigned int dst;

				if (prefix->AdvPreferredLifetime > prefix->AdvValidLifetime)
				{
					flog(LOG_ERR, "AdvValidLifeTime must be "
						"greater than AdvPreferredLifetime in %s, line %d",
						filename, num_lines);
					ABORT;
				}

				if ( prefix->if6[0] && prefix->if6to4[0]) {
					flog(LOG_ERR, "Base6Interface and Base6to4Interface are mutually exclusive at this time");
					ABORT;
				}

				if ( prefix->if6to4[0] )
				{
					if (get_v4addr(prefix->if6to4, &dst) < 0)
					{
						flog(LOG_ERR, "interface %s has no IPv4 addresses, disabling 6to4 prefix", prefix->if6to4 );
						prefix->enabled = 0;
					}
					else
					{
						*((uint16_t *)(prefix->Prefix.s6_addr)) = htons(0x2002);
						memcpy( prefix->Prefix.s6_addr + 2, &dst, sizeof( dst ) );
					}
				}

				if ( prefix->if6[0] )
				{
#ifndef HAVE_IFADDRS_H
					flog(LOG_ERR, "Base6Interface not supported in %s, line %d", filename, num_lines);
					ABORT;
#else
					struct ifaddrs *ifap = 0, *ifa = 0;
					struct AdvPrefix *next = prefix->next;

					if (prefix->PrefixLen != 64) {
						flog(LOG_ERR, "only /64 is allowed with Base6Interface.  %s:%d", filename, num_lines);
						ABORT;
					}

					if (getifaddrs(&ifap) != 0)
						flog(LOG_ERR, "getifaddrs failed: %s", strerror(errno));

					for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
						struct sockaddr_in6 *s6 = 0;
						struct sockaddr_in6 *mask = (struct sockaddr_in6 *)ifa->ifa_netmask;
						struct in6_addr base6prefix;
						char buf[INET6_ADDRSTRLEN];
						int i;

						if (strncmp(ifa->ifa_name, prefix->if6, IFNAMSIZ))
							continue;

						if (ifa->ifa_addr->sa_family != AF_INET6)
							continue;

						s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);

						if (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr))
							continue;

						base6prefix = get_prefix6(&s6->sin6_addr, &mask->sin6_addr);
						for (i = 0; i < 8; ++i) {
							prefix->Prefix.s6_addr[i] &= ~mask->sin6_addr.s6_addr[i];
							prefix->Prefix.s6_addr[i] |= base6prefix.s6_addr[i];
						}
						memset(&prefix->Prefix.s6_addr[8], 0, 8);
						prefix->AdvRouterAddr = 1;
						prefix->AutoSelected = 1;
						prefix->next = next;

						if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(prefix->Prefix), buf, sizeof(buf)) == NULL)
							flog(LOG_ERR, "%s: inet_ntop failed in %s, line %d!", ifa->ifa_name, filename, num_lines);
						else
							dlog(LOG_DEBUG, 3, "auto-selected prefix %s/%d on interface %s from interface %s",
								buf, prefix->PrefixLen, iface->props.name, ifa->ifa_name);

						/* Taking only one prefix from the Base6Interface.  Taking more than one would require allocating new
						   prefixes and building a list.  I'm not sure how to do that from here. So for now, break. */
						break;
					}

					if (ifap)
						freeifaddrs(ifap);
#endif /* ifndef HAVE_IFADDRS_H */
				}
			}
			(yyval.pinfo) = prefix;
			prefix = NULL;
		}
#line 2006 "gram.c" /* yacc.c:1646  */
    break;

  case 45:
#line 464 "gram.y" /* yacc.c:1646  */
    {
			struct in6_addr zeroaddr;
			memset(&zeroaddr, 0, sizeof(zeroaddr));

			if (!memcmp((yyvsp[-2].addr), &zeroaddr, sizeof(struct in6_addr))) {
#ifndef HAVE_IFADDRS_H
				flog(LOG_ERR, "invalid all-zeros prefix in %s, line %d", filename, num_lines);
				ABORT;
#else
				struct ifaddrs *ifap = 0, *ifa = 0;
				struct AdvPrefix *next = iface->AdvPrefixList;

				while (next) {
					if (next->AutoSelected) {
						flog(LOG_ERR, "auto selecting prefixes works only once per interface.  See %s, line %d", filename, num_lines);
						ABORT;
					}
					next = next->next;
				}
				next = 0;

				dlog(LOG_DEBUG, 5, "all-zeros prefix in %s, line %d", filename, num_lines);

				if (getifaddrs(&ifap) != 0)
					flog(LOG_ERR, "getifaddrs failed: %s", strerror(errno));

				for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
					struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
					struct sockaddr_in6 *mask = (struct sockaddr_in6 *)ifa->ifa_netmask;
					char buf[INET6_ADDRSTRLEN];

					if (strncmp(ifa->ifa_name, iface->props.name, IFNAMSIZ))
						continue;

					if (ifa->ifa_addr->sa_family != AF_INET6)
						continue;

					s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);

					if (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr))
						continue;

					prefix = malloc(sizeof(struct AdvPrefix));

					if (prefix == NULL) {
						flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
						ABORT;
					}

					prefix_init_defaults(prefix);
					prefix->Prefix = get_prefix6(&s6->sin6_addr, &mask->sin6_addr);
					prefix->AdvRouterAddr = 1;
					prefix->AutoSelected = 1;
					prefix->next = next;
					next = prefix;

					if (prefix->PrefixLen == 0)
						prefix->PrefixLen = count_mask(mask);

					if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(prefix->Prefix), buf, sizeof(buf)) == NULL)
						flog(LOG_ERR, "%s: inet_ntop failed in %s, line %d!", ifa->ifa_name, filename, num_lines);
					else
						dlog(LOG_DEBUG, 3, "auto-selected prefix %s/%d on interface %s", buf, prefix->PrefixLen, ifa->ifa_name);
				}

				if (!prefix) {
					flog(LOG_WARNING, "no auto-selected prefix on interface %s, disabling advertisements",  iface->props.name);
				}

				if (ifap)
					freeifaddrs(ifap);
#endif /* ifndef HAVE_IFADDRS_H */
			}
			else {
				prefix = malloc(sizeof(struct AdvPrefix));

				if (prefix == NULL) {
					flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
					ABORT;
				}

				prefix_init_defaults(prefix);

				if ((yyvsp[0].num) > MAX_PrefixLen)
				{
					flog(LOG_ERR, "invalid prefix length in %s, line %d", filename, num_lines);
					ABORT;
				}

				prefix->PrefixLen = (yyvsp[0].num);

				memcpy(&prefix->Prefix, (yyvsp[-2].addr), sizeof(struct in6_addr));
			}
		}
#line 2105 "gram.c" /* yacc.c:1646  */
    break;

  case 51:
#line 570 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					struct AdvPrefix *p = prefix;
					do {
						p->AdvOnLinkFlag = (yyvsp[-1].num);
						p = p->next;
					} while (p && p->AutoSelected);
				}
				else
					prefix->AdvOnLinkFlag = (yyvsp[-1].num);
			}
		}
#line 2123 "gram.c" /* yacc.c:1646  */
    break;

  case 52:
#line 584 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					struct AdvPrefix *p = prefix;
					do {
						p->AdvAutonomousFlag = (yyvsp[-1].num);
						p = p->next;
					} while (p && p->AutoSelected);
				}
				else
					prefix->AdvAutonomousFlag = (yyvsp[-1].num);
			}
		}
#line 2141 "gram.c" /* yacc.c:1646  */
    break;

  case 53:
#line 598 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected && (yyvsp[-1].num) == 0)
					flog(LOG_WARNING, "prefix automatically selected, AdvRouterAddr always enabled, ignoring config line %d", num_lines);
				else
					prefix->AdvRouterAddr = (yyvsp[-1].num);
			}
		}
#line 2154 "gram.c" /* yacc.c:1646  */
    break;

  case 54:
#line 607 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					struct AdvPrefix *p = prefix;
					do {
						p->AdvValidLifetime = (yyvsp[-1].num);
						p->curr_validlft = (yyvsp[-1].num);
						p = p->next;
					} while (p && p->AutoSelected);
				}
				else {
					prefix->AdvValidLifetime = (yyvsp[-1].num);
					prefix->curr_validlft = (yyvsp[-1].num);
				}
			}
		}
#line 2175 "gram.c" /* yacc.c:1646  */
    break;

  case 55:
#line 624 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					struct AdvPrefix *p = prefix;
					do {
						p->AdvPreferredLifetime = (yyvsp[-1].num);
						p->curr_preferredlft = (yyvsp[-1].num);
						p = p->next;
					} while (p && p->AutoSelected);
				}
				else {
					prefix->AdvPreferredLifetime = (yyvsp[-1].num);
					prefix->curr_preferredlft = (yyvsp[-1].num);
				}
			}
		}
#line 2196 "gram.c" /* yacc.c:1646  */
    break;

  case 56:
#line 641 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
			prefix->DeprecatePrefixFlag = (yyvsp[-1].num);
		}
		}
#line 2206 "gram.c" /* yacc.c:1646  */
    break;

  case 57:
#line 647 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
			prefix->DecrementLifetimesFlag = (yyvsp[-1].num);
		}
		}
#line 2216 "gram.c" /* yacc.c:1646  */
    break;

  case 58:
#line 653 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					flog(LOG_ERR, "automatically selecting the prefix and Base6Interface are mutually exclusive");
					ABORT;
				} /* fallthrough */
				dlog(LOG_DEBUG, 4, "using prefixes on interface %s for prefixes on interface %s", (yyvsp[-1].str), iface->props.name);
				strncpy(prefix->if6, (yyvsp[-1].str), IFNAMSIZ-1);
				prefix->if6[IFNAMSIZ-1] = '\0';
			}
		}
#line 2232 "gram.c" /* yacc.c:1646  */
    break;

  case 59:
#line 666 "gram.y" /* yacc.c:1646  */
    {
			if (prefix) {
				if (prefix->AutoSelected) {
					flog(LOG_ERR, "automatically selecting the prefix and Base6to4Interface are mutually exclusive");
					ABORT;
				} /* fallthrough */
				dlog(LOG_DEBUG, 4, "using interface %s for 6to4 prefixes on interface %s", (yyvsp[-1].str), iface->props.name);
				strncpy(prefix->if6to4, (yyvsp[-1].str), IFNAMSIZ-1);
				prefix->if6to4[IFNAMSIZ-1] = '\0';
			}
		}
#line 2248 "gram.c" /* yacc.c:1646  */
    break;

  case 60:
#line 680 "gram.y" /* yacc.c:1646  */
    {
			(yyval.rinfo) = route;
			route = NULL;
		}
#line 2257 "gram.c" /* yacc.c:1646  */
    break;

  case 61:
#line 688 "gram.y" /* yacc.c:1646  */
    {
			route = malloc(sizeof(struct AdvRoute));

			if (route == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			route_init_defaults(route, iface);

			if ((yyvsp[0].num) > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid route prefix length in %s, line %d", filename, num_lines);
				ABORT;
			}

			route->PrefixLen = (yyvsp[0].num);

			memcpy(&route->Prefix, (yyvsp[-2].addr), sizeof(struct in6_addr));
		}
#line 2282 "gram.c" /* yacc.c:1646  */
    break;

  case 66:
#line 721 "gram.y" /* yacc.c:1646  */
    {
			route->AdvRoutePreference = (yyvsp[-1].snum);
		}
#line 2290 "gram.c" /* yacc.c:1646  */
    break;

  case 67:
#line 725 "gram.y" /* yacc.c:1646  */
    {
			route->AdvRouteLifetime = (yyvsp[-1].num);
		}
#line 2298 "gram.c" /* yacc.c:1646  */
    break;

  case 68:
#line 729 "gram.y" /* yacc.c:1646  */
    {
			route->RemoveRouteFlag = (yyvsp[-1].num);
		}
#line 2306 "gram.c" /* yacc.c:1646  */
    break;

  case 69:
#line 735 "gram.y" /* yacc.c:1646  */
    {
			(yyval.rdnssinfo) = rdnss;
			rdnss = NULL;
		}
#line 2315 "gram.c" /* yacc.c:1646  */
    break;

  case 72:
#line 746 "gram.y" /* yacc.c:1646  */
    {
			if (!rdnss) {
				/* first IP found */
				rdnss = malloc(sizeof(struct AdvRDNSS));

				if (rdnss == NULL) {
					flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
					ABORT;
				}

				rdnss_init_defaults(rdnss, iface);
			}

			switch (rdnss->AdvRDNSSNumber) {
				case 0:
					memcpy(&rdnss->AdvRDNSSAddr1, (yyvsp[0].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				case 1:
					memcpy(&rdnss->AdvRDNSSAddr2, (yyvsp[0].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				case 2:
					memcpy(&rdnss->AdvRDNSSAddr3, (yyvsp[0].addr), sizeof(struct in6_addr));
					rdnss->AdvRDNSSNumber++;
					break;
				default:
					flog(LOG_CRIT, "too many addresses in RDNSS section");
					ABORT;
			}

		}
#line 2352 "gram.c" /* yacc.c:1646  */
    break;

  case 73:
#line 781 "gram.y" /* yacc.c:1646  */
    {
			if (!rdnss) {
				flog(LOG_CRIT, "no address specified in RDNSS section");
				ABORT;
			}
		}
#line 2363 "gram.c" /* yacc.c:1646  */
    break;

  case 78:
#line 799 "gram.y" /* yacc.c:1646  */
    {
			flog(LOG_WARNING, "ignoring deprecated RDNSS preference");
		}
#line 2371 "gram.c" /* yacc.c:1646  */
    break;

  case 79:
#line 803 "gram.y" /* yacc.c:1646  */
    {
			flog(LOG_WARNING, "ignoring deprecated RDNSS open flag");
		}
#line 2379 "gram.c" /* yacc.c:1646  */
    break;

  case 80:
#line 807 "gram.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].num) > 2*(iface->MaxRtrAdvInterval))
				flog(LOG_WARNING, "warning: AdvRDNSSLifetime <= 2*MaxRtrAdvInterval would allow stale DNS servers to be deleted faster");
			if ((yyvsp[-1].num) < iface->MaxRtrAdvInterval && (yyvsp[-1].num) != 0) {
				flog(LOG_ERR, "AdvRDNSSLifetime must be at least MaxRtrAdvInterval");
				rdnss->AdvRDNSSLifetime = iface->MaxRtrAdvInterval;
			} else {
				rdnss->AdvRDNSSLifetime = (yyvsp[-1].num);
			}
			if ((yyvsp[-1].num) > 2*(iface->MaxRtrAdvInterval))
				flog(LOG_WARNING, "warning: (%s:%d) AdvRDNSSLifetime <= 2*MaxRtrAdvInterval would allow stale DNS servers to be deleted faster", filename, num_lines);

			rdnss->AdvRDNSSLifetime = (yyvsp[-1].num);
		}
#line 2398 "gram.c" /* yacc.c:1646  */
    break;

  case 81:
#line 822 "gram.y" /* yacc.c:1646  */
    {
			rdnss->FlushRDNSSFlag = (yyvsp[-1].num);
		}
#line 2406 "gram.c" /* yacc.c:1646  */
    break;

  case 82:
#line 828 "gram.y" /* yacc.c:1646  */
    {
			(yyval.dnsslinfo) = dnssl;
			dnssl = NULL;
		}
#line 2415 "gram.c" /* yacc.c:1646  */
    break;

  case 85:
#line 839 "gram.y" /* yacc.c:1646  */
    {
			char *ch;
			for (ch = (yyvsp[0].str);*ch != '\0';ch++) {
				if (*ch >= 'A' && *ch <= 'Z')
					continue;
				if (*ch >= 'a' && *ch <= 'z')
					continue;
				if (*ch >= '0' && *ch <= '9')
					continue;
				if (*ch == '-' || *ch == '.')
					continue;

				flog(LOG_CRIT, "invalid domain suffix specified");
				ABORT;
			}

			if (!dnssl) {
				/* first domain found */
				dnssl = malloc(sizeof(struct AdvDNSSL));

				if (dnssl == NULL) {
					flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
					ABORT;
				}

				dnssl_init_defaults(dnssl, iface);
			}

			dnssl->AdvDNSSLNumber++;
			dnssl->AdvDNSSLSuffixes =
				realloc(dnssl->AdvDNSSLSuffixes,
					dnssl->AdvDNSSLNumber * sizeof(char*));
			if (dnssl->AdvDNSSLSuffixes == NULL) {
				flog(LOG_CRIT, "realloc failed: %s", strerror(errno));
				ABORT;
			}

			dnssl->AdvDNSSLSuffixes[dnssl->AdvDNSSLNumber - 1] = strdup((yyvsp[0].str));
		}
#line 2459 "gram.c" /* yacc.c:1646  */
    break;

  case 86:
#line 881 "gram.y" /* yacc.c:1646  */
    {
			if (!dnssl) {
				flog(LOG_CRIT, "no domain specified in DNSSL section");
				ABORT;
			}
		}
#line 2470 "gram.c" /* yacc.c:1646  */
    break;

  case 91:
#line 899 "gram.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].num) > 2*(iface->MaxRtrAdvInterval))
				flog(LOG_WARNING, "warning: AdvDNSSLLifetime <= 2*MaxRtrAdvInterval would allow stale DNS suffixes to be deleted faster");
			if ((yyvsp[-1].num) < iface->MaxRtrAdvInterval && (yyvsp[-1].num) != 0) {
				flog(LOG_ERR, "AdvDNSSLLifetime must be at least MaxRtrAdvInterval");
				dnssl->AdvDNSSLLifetime = iface->MaxRtrAdvInterval;
			} else {
				dnssl->AdvDNSSLLifetime = (yyvsp[-1].num);
			}

		}
#line 2486 "gram.c" /* yacc.c:1646  */
    break;

  case 92:
#line 911 "gram.y" /* yacc.c:1646  */
    {
			dnssl->FlushDNSSLFlag = (yyvsp[-1].num);
		}
#line 2494 "gram.c" /* yacc.c:1646  */
    break;

  case 93:
#line 917 "gram.y" /* yacc.c:1646  */
    {
			(yyval.lowpancoinfo) = lowpanco;
			lowpanco = NULL;
		}
#line 2503 "gram.c" /* yacc.c:1646  */
    break;

  case 94:
#line 924 "gram.y" /* yacc.c:1646  */
    {
			lowpanco = malloc(sizeof(struct AdvLowpanCo));

			if (lowpanco == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			memset(lowpanco, 0, sizeof(struct AdvLowpanCo));
		}
#line 2518 "gram.c" /* yacc.c:1646  */
    break;

  case 99:
#line 945 "gram.y" /* yacc.c:1646  */
    {
			lowpanco->ContextLength = (yyvsp[-1].num);
		}
#line 2526 "gram.c" /* yacc.c:1646  */
    break;

  case 100:
#line 949 "gram.y" /* yacc.c:1646  */
    {
			lowpanco->ContextCompressionFlag = (yyvsp[-1].num);
		}
#line 2534 "gram.c" /* yacc.c:1646  */
    break;

  case 101:
#line 953 "gram.y" /* yacc.c:1646  */
    {
			lowpanco->AdvContextID = (yyvsp[-1].num);
		}
#line 2542 "gram.c" /* yacc.c:1646  */
    break;

  case 102:
#line 957 "gram.y" /* yacc.c:1646  */
    {
			lowpanco->AdvLifeTime = (yyvsp[-1].num);
		}
#line 2550 "gram.c" /* yacc.c:1646  */
    break;

  case 103:
#line 963 "gram.y" /* yacc.c:1646  */
    {
			(yyval.abroinfo) = abro;
			abro = NULL;
		}
#line 2559 "gram.c" /* yacc.c:1646  */
    break;

  case 104:
#line 970 "gram.y" /* yacc.c:1646  */
    {
			if ((yyvsp[0].num) > MAX_PrefixLen)
			{
				flog(LOG_ERR, "invalid abro prefix length in %s, line %d", filename, num_lines);
				ABORT;
			}

			abro = malloc(sizeof(struct AdvAbro));

			if (abro == NULL) {
				flog(LOG_CRIT, "malloc failed: %s", strerror(errno));
				ABORT;
			}

			memset(abro, 0, sizeof(struct AdvAbro));
			memcpy(&abro->LBRaddress, (yyvsp[-2].addr), sizeof(struct in6_addr));
		}
#line 2581 "gram.c" /* yacc.c:1646  */
    break;

  case 109:
#line 998 "gram.y" /* yacc.c:1646  */
    {
			abro->Version[1] = (yyvsp[-1].num);
		}
#line 2589 "gram.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1002 "gram.y" /* yacc.c:1646  */
    {
			abro->Version[0] = (yyvsp[-1].num);
		}
#line 2597 "gram.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1006 "gram.y" /* yacc.c:1646  */
    {
			abro->ValidLifeTime = (yyvsp[-1].num);
		}
#line 2605 "gram.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1012 "gram.y" /* yacc.c:1646  */
    {
				(yyval.num) = (yyvsp[0].num);
			}
#line 2613 "gram.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1016 "gram.y" /* yacc.c:1646  */
    {
				(yyval.num) = (uint32_t)~0;
			}
#line 2621 "gram.c" /* yacc.c:1646  */
    break;


#line 2625 "gram.c" /* yacc.c:1646  */
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
#line 1021 "gram.y" /* yacc.c:1906  */


static int countbits(int b)
{
	int count;

	for (count = 0; b != 0; count++) {
		b &= b - 1; // this clears the LSB-most set bit
	}

	return (count);
}

static int count_mask(struct sockaddr_in6 *m)
{
	struct in6_addr *in6 = &m->sin6_addr;
	int i;
	int count = 0;

	for (i = 0; i < 16; ++i) {
		count += countbits(in6->s6_addr[i]);
	}
	return count;
}

static struct in6_addr get_prefix6(struct in6_addr const *addr, struct in6_addr const *mask)
{
	struct in6_addr prefix = *addr;
	int i = 0;

	for (; i < 16; ++i) {
		prefix.s6_addr[i] &= mask->s6_addr[i];
	}

	return prefix;
}

static void cleanup(void)
{
	if (iface) {
		free_ifaces(iface);
		iface = 0;
	}

	if (prefix) {
		free(prefix);
		prefix = 0;
	}

	if (route) {
		free(route);
		route = 0;
	}

	if (rdnss) {
		free(rdnss);
		rdnss = 0;
	}

	if (dnssl) {
		int i;
		for (i = 0;i < dnssl->AdvDNSSLNumber;i++)
			free(dnssl->AdvDNSSLSuffixes[i]);
		free(dnssl->AdvDNSSLSuffixes);
		free(dnssl);
		dnssl = 0;
	}

	if (lowpanco) {
		free(lowpanco);
		lowpanco = 0;
	}

	if (abro) {
		free(abro);
		abro = 0;
	}
}

struct Interface * readin_config(char const *path)
{
	FILE * in = fopen(path, "r");
	if (in) {
		filename = path;
		IfaceList = 0;
		num_lines = 1;
		iface = 0;

		yyset_in(in);
		if (yyparse() != 0) {
			free_ifaces(iface);
			iface = 0;
		} else {
			dlog(LOG_DEBUG, 1, "config file, %s, syntax ok", path);
		}
		yylex_destroy();
		fclose(in);
	}

	return IfaceList;
}

static void yyerror(char const * msg)
{
	fprintf(stderr, "%s:%d error: %s\n",
		filename,
		num_lines,
		msg);
}

