/* bits/types.h -- definitions of __*_t types underlying *_t types.
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/*
 * Never include this file directly; use <sys/types.h> instead.
 */

#ifndef	_BITS_TYPES_H
#define	_BITS_TYPES_H	1

#include <features.h>
#include <bits/wordsize.h>

#define __need_size_t
#include <stddef.h>

/* Convenience types.  */
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;

/* Fixed-size types, underlying types depend on word size and compiler.  */
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
#if __WORDSIZE == 64
typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;
#elif defined(__GNUC__)
__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;
#endif

/* quad_t is also 64 bits.  */
#if __WORDSIZE == 64
typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
#elif defined(__GNUC__)
__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
#else
typedef struct
{
  long __val[2];
} __quad_t;
typedef struct
{
  __u_long __val[2];
} __u_quad_t;
#endif


/* The machine-dependent file <bits/typesizes.h> defines __*_T_TYPE
   macros for each of the OS types we define below.  The definitions
   of those macros must use the following macros for underlying types.
   We define __S<SIZE>_TYPE and __U<SIZE>_TYPE for the signed and unsigned
   variants of each of the following integer types on this machine.

	16		-- "natural" 16-bit type (always short)
	32		-- "natural" 32-bit type (always int)
	64		-- "natural" 64-bit type (long or long long)
	LONG32		-- 32-bit type, traditionally long
	QUAD		-- 64-bit type, always long long
	WORD		-- natural type of __WORDSIZE bits (int or long)
	LONGWORD	-- type of __WORDSIZE bits, traditionally long

   We distinguish WORD/LONGWORD, 32/LONG32, and 64/QUAD so that the
   conventional uses of `long' or `long long' type modifiers match the
   types we define, even when a less-adorned type would be the same size.
   This matters for (somewhat) portably writing printf/scanf formats for
   these types, where using the appropriate l or ll format modifiers can
   make the typedefs and the formats match up across all GNU platforms.  If
   we used `long' when it's 64 bits where `long long' is expected, then the
   compiler would warn about the formats not matching the argument types,
   and the programmer changing them to shut up the compiler would break the
   program's portability.

   Here we assume what is presently the case in all the GCC configurations
   we support: long long is always 64 bits, long is always word/address size,
   and int is always 32 bits.  */

#define	__S16_TYPE		short int
#define __U16_TYPE		unsigned short int
#define	__S32_TYPE		int
#define __U32_TYPE		unsigned int
#define __SLONGWORD_TYPE	long int
#define __ULONGWORD_TYPE	unsigned long int
#if __WORDSIZE == 32
# define __SQUAD_TYPE		__quad_t
# define __UQUAD_TYPE		__u_quad_t
# define __SWORD_TYPE		int
# define __UWORD_TYPE		unsigned int
# define __SLONG32_TYPE		long int
# define __ULONG32_TYPE		unsigned long int
# define __S64_TYPE		__quad_t
# define __U64_TYPE		__u_quad_t
/* We want __extension__ before typedef's that use nonstandard base types
   such as `long long' in C89 mode.  */
# define __STD_TYPE		__extension__ typedef
#elif __WORDSIZE == 64
# define __SQUAD_TYPE		long int
# define __UQUAD_TYPE		unsigned long int
# define __SWORD_TYPE		long int
# define __UWORD_TYPE		unsigned long int
# define __SLONG32_TYPE		int
# define __ULONG32_TYPE		unsigned int
# define __S64_TYPE		long int
# define __U64_TYPE		unsigned long int
/* No need to mark the typedef with __extension__.   */
# define __STD_TYPE		typedef
#else
# error your machine is neither 32 bit or 64 bit ... it must be magical
#endif
#include <bits/typesizes.h>	/* Defines __*_T_TYPE macros.  */


__STD_TYPE __DEV_T_TYPE __dev_t;	/* Type of device numbers.  */
__STD_TYPE __UID_T_TYPE __uid_t;	/* Type of user identifications.  */
__STD_TYPE __GID_T_TYPE __gid_t;	/* Type of group identifications.  */
__STD_TYPE __INO_T_TYPE __ino_t;	/* Type of file serial numbers.  */
__STD_TYPE __INO64_T_TYPE __ino64_t;	/* Type of file serial numbers (LFS).*/
__STD_TYPE __MODE_T_TYPE __mode_t;	/* Type of file attribute bitmasks.  */
__STD_TYPE __NLINK_T_TYPE __nlink_t;	/* Type of file link counts.  */
__STD_TYPE __OFF_T_TYPE __off_t;	/* Type of file sizes and offsets.  */
__STD_TYPE __OFF64_T_TYPE __off64_t;	/* Type of file sizes and offsets (LFS).  */
__STD_TYPE __PID_T_TYPE __pid_t;	/* Type of process identifications.  */
__STD_TYPE __FSID_T_TYPE __fsid_t;	/* Type of file system IDs.  */
__STD_TYPE __CLOCK_T_TYPE __clock_t;	/* Type of CPU usage counts.  */
__STD_TYPE __RLIM_T_TYPE __rlim_t;	/* Type for resource measurement.  */
__STD_TYPE __RLIM64_T_TYPE __rlim64_t;	/* Type for resource measurement (LFS).  */
__STD_TYPE __ID_T_TYPE __id_t;		/* General type for IDs.  */
__STD_TYPE __TIME_T_TYPE __time_t;	/* Seconds since the Epoch.  */
__STD_TYPE __USECONDS_T_TYPE __useconds_t; /* Count of microseconds.  */
__STD_TYPE __SUSECONDS_T_TYPE __suseconds_t; /* Signed count of microseconds.  */

__STD_TYPE __DADDR_T_TYPE __daddr_t;	/* The type of a disk address.  */
__STD_TYPE __SWBLK_T_TYPE __swblk_t;	/* Type of a swap block maybe?  */
__STD_TYPE __KEY_T_TYPE __key_t;	/* Type of an IPC key.  */

/* Clock ID used in clock and timer functions.  */
__STD_TYPE __CLOCKID_T_TYPE __clockid_t;

/* Timer ID returned by `timer_create'.  */
__STD_TYPE __TIMER_T_TYPE __timer_t;

/* Type to represent block size.  */
__STD_TYPE __BLKSIZE_T_TYPE __blksize_t;

/* Types from the Large File Support interface.  */

/* Type to count number of disk blocks.  */
__STD_TYPE __BLKCNT_T_TYPE __blkcnt_t;
__STD_TYPE __BLKCNT64_T_TYPE __blkcnt64_t;

/* Type to count file system blocks.  */
__STD_TYPE __FSBLKCNT_T_TYPE __fsblkcnt_t;
__STD_TYPE __FSBLKCNT64_T_TYPE __fsblkcnt64_t;

/* Type to count file system nodes.  */
__STD_TYPE __FSFILCNT_T_TYPE __fsfilcnt_t;
__STD_TYPE __FSFILCNT64_T_TYPE __fsfilcnt64_t;

__STD_TYPE __SSIZE_T_TYPE __ssize_t; /* Type of a byte count, or error.  */

/* These few don't really vary by system, they always correspond
   to one of the other defined types.  */
typedef __off64_t __loff_t;	/* Type of file sizes and offsets (LFS).  */
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;

/* Used in XTI.  */
typedef long int __t_scalar_t;
typedef unsigned long int __t_uscalar_t;

/* Duplicates info from stdint.h but this is used in unistd.h.  */
__STD_TYPE __SWORD_TYPE __intptr_t;

/* Duplicate info from sys/socket.h.  */
__STD_TYPE __U32_TYPE __socklen_t;


#undef __STD_TYPE

/* Now add the thread types.  */
#if defined __UCLIBC_HAS_THREADS__ && (defined __USE_POSIX199506 || defined __USE_UNIX98)
# include <bits/pthreadtypes.h>
#endif

#endif /* bits/types.h */
