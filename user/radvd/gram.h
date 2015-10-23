/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 137 "gram.y" /* yacc.c:1909  */

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

#line 201 "gram.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAM_H_INCLUDED  */
