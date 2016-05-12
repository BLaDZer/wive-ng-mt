/* dnsmasq is Copyright (c) 2000-2016 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
     
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define FTABSIZ 50 /* max number of outstanding requests (default) */
#define MAX_PROCS 10 /* max no children for TCP requests */
#define CHILD_LIFETIME 150 /* secs 'till terminated (RFC1035 suggests > 120s) */
#define TCP_MAX_QUERIES 100 /* Maximum number of queries per incoming TCP connection */
#define EDNS_PKTSZ 4096 /* default max EDNS.0 UDP packet from RFC5625 */
#define SAFE_PKTSZ 1280 /* "go anywhere" UDP packet size */
#define KEYBLOCK_LEN 40 /* choose to mininise fragmentation when storing DNSSEC keys */
#define DNSSEC_WORK 50 /* Max number of queries to validate one question */
#define TIMEOUT 10 /* drop UDP queries after TIMEOUT seconds */
#define FORWARD_TEST 50 /* try all servers every 50 queries */
#define FORWARD_TIME 20 /* or 20 seconds */
#define SERVERS_LOGGED 30 /* Only log this many servers when logging state */
#define RANDOM_SOCKS 64 /* max simultaneous random ports */
#define LEASE_RETRY 60 /* on error, retry writing leasefile after LEASE_RETRY seconds */
#define CACHESIZ 150 /* default cache size */
#define TTL_FLOOR_LIMIT 3600 /* don't allow --min-cache-ttl to raise TTL above this under any circumstances */
#define MAXLEASES 100 /* maximum number of DHCP leases */
#define PING_WAIT 3 /* wait for ping address-in-use test */
#define PING_CACHE_TIME 30 /* Ping test assumed to be valid this long. */
#define DECLINE_BACKOFF 600 /* disable DECLINEd static addresses for this long */
#define DHCP_PACKET_MAX 16384 /* hard limit on DHCP packet size */
#define SMALLDNAME 50 /* most domain names are smaller than this */
#define CNAME_CHAIN 10 /* chains longer than this atr dropped for loop protection */
#define HOSTSFILE "/etc/hosts"
#define ETHERSFILE "/etc/ethers"
#define RESOLVFILE "/etc/resolv.conf"
#define LEASEFILE "/var/lib/misc/dnsmasq.leases"
#define CONFFILE "/etc/dnsmasq.conf"
#define DEFLEASE 3600 /* default lease time, 1 hour */
#define CHUSER "Admin"
#define CHGRP "Admin"
#define NAMESERVER_PORT 53
#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_ALTPORT 1067
#define DHCP_CLIENT_ALTPORT 1068
#define PXE_PORT 4011
#define TFTP_PORT 69
#define TFTP_MAX_CONNECTIONS 50 /* max simultaneous connections */
#define LOG_MAX 5 /* log-queue length */
#define RANDFILE "/dev/urandom"
#define DNSMASQ_SERVICE "uk.org.thekelleys.dnsmasq" /* Default - may be overridden by config */
#define DNSMASQ_PATH "/uk/org/thekelleys/dnsmasq"
#define AUTH_TTL 600 /* default TTL for auth DNS */
#define SOA_REFRESH 1200 /* SOA refresh default */
#define SOA_RETRY 180 /* SOA retry default */
#define SOA_EXPIRY 1209600 /* SOA expiry default */
#define LOOP_TEST_DOMAIN "test" /* domain for loop testing, "test" is reserved by RFC 2606 and won't therefore clash */
#define LOOP_TEST_TYPE T_TXT
 
/* Follows system specific switches. If you run on a
   new system, you may want to edit these.
   May replace this with Autoconf one day.

NO_NETTLE_ECC
   Don't include the ECDSA cypher in DNSSEC validation. Needed for older Nettle versions.
NO_GMP
   Don't use and link against libgmp, Useful if nettle is built with --enable-mini-gmp.

HAVE_LINUX_NETWORK
HAVE_BSD_NETWORK
HAVE_SOLARIS_NETWORK
   define exactly one of these to alter interaction with kernel networking.

HAVE_TFTP
   define this to get dnsmasq's built-in TFTP server.

HAVE_DHCP
   define this to get dnsmasq's DHCPv4 server.

HAVE_DHCP6
   define this to get dnsmasq's DHCPv6 server. (implies HAVE_DHCP).

HAVE_SCRIPT
   define this to get the ability to call scripts on lease-change.

HAVE_LUASCRIPT
   define this to get the ability to call Lua script on lease-change. (implies HAVE_SCRIPT) 

HAVE_DBUS
   define this if you want to link against libdbus, and have dnsmasq
   support some methods to allow (re)configuration of the upstream DNS 
   servers via DBus.

HAVE_IDN
   define this if you want international domain name support.
   NOTE: for backwards compatibility, IDN support is automatically 
         included when internationalisation support is built, using the 
	 *-i18n makefile targets, even if HAVE_IDN is not explicitly set.

HAVE_CONNTRACK
   define this to include code which propogates conntrack marks from
   incoming DNS queries to the corresponding upstream queries. This adds
   a build-dependency on libnetfilter_conntrack, but the resulting binary will
   still run happily on a kernel without conntrack support.

HAVE_IPSET
    define this to include the ability to selectively add resolved ip addresses
    to given ipsets.

HAVE_AUTH
   define this to include the facility to act as an authoritative DNS
   server for one or more zones.

HAVE_DNSSEC
   include DNSSEC validator.

HAVE_LOOP
   include functionality to probe for and remove DNS forwarding loops.

HAVE_INOTIFY
   use the Linux inotify facility to efficiently re-read configuration files.

NOTES:
   For Linux you should define
      HAVE_LINUX_NETWORK
      HAVE_GETOPT_LONG
  you should NOT define
      HAVE_SOCKADDR_SA_LEN

   For *BSD systems you should define
     HAVE_BSD_NETWORK
     HAVE_SOCKADDR_SA_LEN
   and you MAY define
     HAVE_ARC4RANDOM - OpenBSD and FreeBSD and NetBSD version 2.0 or later
     HAVE_GETOPT_LONG - NetBSD, later FreeBSD
                       (FreeBSD and OpenBSD only if you link GNU getopt)

*/

/* Build options which require external libraries.
   
   Defining HAVE_<opt>_STATIC as _well_ as HAVE_<opt> will link the library statically.

   You can use "make COPTS=-DHAVE_<opt>" instead of editing these.
*/

/* #define HAVE_LUASCRIPT */
/* #define HAVE_DBUS */
/* #define HAVE_IDN */
/* #define HAVE_CONNTRACK */
/* #define HAVE_DNSSEC */

/* Allow TFTP to be disabled with COPTS=-DNO_TFTP */
#ifdef NO_TFTP
#undef HAVE_TFTP
#endif

/* Allow DHCP to be disabled with COPTS=-DNO_DHCP */
#ifdef NO_DHCP
#undef HAVE_DHCP
#endif

/* Allow scripts to be disabled with COPTS=-DNO_SCRIPT */
#ifdef NO_SCRIPT
#undef HAVE_SCRIPT
#endif

#ifndef RUNFILE
#   if defined(__ANDROID__)
#      define RUNFILE "/data/dnsmasq.pid"
#    else
#      define RUNFILE "/var/run/dnsmasq.pid"
#    endif
#endif

/* platform dependent options. */

/* Must preceed __linux__ since uClinux defines __linux__ too. */
#if defined(__uClinux__)
#define HAVE_LINUX_NETWORK
#define HAVE_GETOPT_LONG
#undef HAVE_SOCKADDR_SA_LEN
/* Never use fork() on uClinux. Note that this is subtly different from the
   --keep-in-foreground option, since it also  suppresses forking new 
   processes for TCP connections and disables the call-a-script on leasechange
   system. It's intended for use on MMU-less kernels. */
#define NO_FORK

#elif defined(__UCLIBC__)
#define HAVE_LINUX_NETWORK
#if defined(__UCLIBC_HAS_GNU_GETOPT__) || \
   ((__UCLIBC_MAJOR__==0) && (__UCLIBC_MINOR__==9) && (__UCLIBC_SUBLEVEL__<21))
#    define HAVE_GETOPT_LONG
#endif
#undef HAVE_SOCKADDR_SA_LEN
#if !defined(__ARCH_HAS_MMU__) && !defined(__UCLIBC_HAS_MMU__)
#  define NO_FORK
#endif
#if defined(__UCLIBC_HAS_IPV6__)
#  ifndef IPV6_V6ONLY
#    define IPV6_V6ONLY 26
#  endif
#endif

/* This is for glibc 2.x */
#elif defined(__linux__)
#define HAVE_LINUX_NETWORK
#define HAVE_GETOPT_LONG
#undef HAVE_SOCKADDR_SA_LEN

#elif defined(__FreeBSD__) || \
      defined(__OpenBSD__) || \
      defined(__DragonFly__) || \
      defined(__FreeBSD_kernel__)
#define HAVE_BSD_NETWORK
/* Later verions of FreeBSD have getopt_long() */
#if defined(optional_argument) && defined(required_argument)
#   define HAVE_GETOPT_LONG
#endif
#define HAVE_SOCKADDR_SA_LEN

#elif defined(__APPLE__)
#define HAVE_BSD_NETWORK
#define HAVE_GETOPT_LONG
#define HAVE_SOCKADDR_SA_LEN
/* Define before sys/socket.h is included so we get socklen_t */
#define _BSD_SOCKLEN_T_
/* Select the RFC_3542 version of the IPv6 socket API. 
   Define before netinet6/in6.h is included. */
#define __APPLE_USE_RFC_3542 

#elif defined(__NetBSD__)
#define HAVE_BSD_NETWORK
#define HAVE_GETOPT_LONG
#define HAVE_SOCKADDR_SA_LEN

#elif defined(__sun) || defined(__sun__)
#define HAVE_SOLARIS_NETWORK
#define HAVE_GETOPT_LONG
#undef HAVE_SOCKADDR_SA_LEN
#define ETHER_ADDR_LEN 6 
 
#endif

/* Decide if we're going to support IPv6 */
/* IPv6 can be forced off with "make COPTS=-DNO_IPV6" */
/* We assume that systems which don't have IPv6
   headers don't have ntop and pton either */

#if !defined(NO_IPV6)
#  define HAVE_IPV6
#  define IPV6_V6ONLY  26
#  define ADDRSTRLEN INET6_ADDRSTRLEN
#  if defined(SOL_IPV6)
#    define IPV6_LEVEL SOL_IPV6
#  else
#    define IPV6_LEVEL IPPROTO_IPV6
#  endif
#else
#  if !defined(INET_ADDRSTRLEN)
#      define INET_ADDRSTRLEN 16 /* 4*3 + 3 dots + NULL */
#  endif
#  undef HAVE_IPV6
#  define ADDRSTRLEN INET_ADDRSTRLEN
#endif

/* Defining this builds a binary which handles time differently and works better on a system without a
   stable RTC (it uses uptime, not epoch time) and writes the DHCP leases file less often to avoid flash wear.
*/

/* #define HAVE_BROKEN_RTC */

/* rules to implement compile-time option dependencies and 
   the NO_XXX flags */

#ifdef NO_IPV6
#undef HAVE_IPV6
#endif

#ifdef NO_TFTP
#undef HAVE_TFTP
#endif

#ifdef NO_DHCP
#undef HAVE_DHCP
#undef HAVE_DHCP6
#endif

#ifdef NO_HAVE_DBUS
#undef HAVE_DBUS
#endif

#if defined(NO_DHCP6) || !defined(HAVE_IPV6)
#undef HAVE_DHCP6
#endif

/* DHCP6 needs DHCP too */
#ifdef HAVE_DHCP6
#define HAVE_DHCP
#endif

#if defined(NO_SCRIPT) || defined(NO_FORK)
#undef HAVE_SCRIPT
#undef HAVE_LUASCRIPT
#endif

/* Must HAVE_SCRIPT to HAVE_LUASCRIPT */
#ifdef HAVE_LUASCRIPT
#define HAVE_SCRIPT
#endif

#ifdef NO_AUTH
#undef HAVE_AUTH
#endif

#if defined(NO_IPSET)
#undef HAVE_IPSET
#endif

#ifdef NO_LOOP
#undef HAVE_LOOP
#endif

#if defined (HAVE_LINUX_NETWORK) && !defined(NO_INOTIFY)
#define HAVE_INOTIFY
#endif

/* Define a string indicating which options are in use.
   DNSMASQP_COMPILE_OPTS is only defined in dnsmasq.c */

#ifdef DNSMASQ_COMPILE_OPTS

static char *compile_opts = 
#ifndef HAVE_IPV6
"no-"
#endif
"IPv6 "
#ifndef HAVE_GETOPT_LONG
"no-"
#endif
"GNU-getopt "
#ifdef HAVE_BROKEN_RTC
"no-RTC "
#endif
#ifdef NO_FORK
"no-MMU "
#endif
#ifndef HAVE_DBUS
"no-"
#endif
"DBus "
#ifndef LOCALEDIR
"no-"
#endif
"i18n "
#if !defined(LOCALEDIR) && !defined(HAVE_IDN)
"no-"
#endif 
"IDN "
#ifndef HAVE_DHCP
"no-"
#endif
"DHCP "
#if defined(HAVE_DHCP)
#  if !defined (HAVE_DHCP6)
     "no-"
#  endif  
     "DHCPv6 "
#  if !defined(HAVE_SCRIPT)
     "no-scripts "
#  else
#    if !defined(HAVE_LUASCRIPT)
       "no-"
#    endif
     "Lua "
#  endif
#endif
#ifndef HAVE_TFTP
"no-"
#endif
"TFTP "
#ifndef HAVE_CONNTRACK
"no-"
#endif
"conntrack "
#ifndef HAVE_IPSET
"no-"
#endif
"ipset "
#ifndef HAVE_AUTH
"no-"
#endif
"auth "
#ifndef HAVE_DNSSEC
"no-"
#endif
"DNSSEC "
#ifndef HAVE_LOOP
"no-"
#endif
"loop-detect "
#ifndef HAVE_INOTIFY
"no-"
#endif
"inotify";


#endif



