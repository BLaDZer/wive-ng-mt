/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 by Ralf Baechle (ralf@linux-mips.org)
 */
#ifndef __ASM_BARRIER_H
#define __ASM_BARRIER_H

#include <asm/addrspace.h>

/*
 * read_barrier_depends - Flush all pending reads that subsequents reads
 * depend on.
 *
 * No data-dependent reads from memory-like regions are ever reordered
 * over this barrier.  All reads preceding this primitive are guaranteed
 * to access memory (but not necessarily other CPUs' caches) before any
 * reads following this primitive that depend on the data return by
 * any of the preceding reads.  This primitive is much lighter weight than
 * rmb() on most CPUs, and is never heavier weight than is
 * rmb().
 *
 * These ordering constraints are respected by both the local CPU
 * and the compiler.
 *
 * Ordering is not guaranteed by anything other than these primitives,
 * not even by data dependencies.  See the documentation for
 * memory_barrier() for examples and URLs to more information.
 *
 * For example, the following code would force ordering (the initial
 * value of "a" is zero, "b" is one, and "p" is "&a"):
 *
 * <programlisting>
 *	CPU 0				CPU 1
 *
 *	b = 2;
 *	memory_barrier();
 *	p = &b;				q = p;
 *					read_barrier_depends();
 *					d = *q;
 * </programlisting>
 *
 * because the read of "*q" depends on the read of "p" and these
 * two reads are separated by a read_barrier_depends().  However,
 * the following code, with the same initial values for "a" and "b":
 *
 * <programlisting>
 *	CPU 0				CPU 1
 *
 *	a = 2;
 *	memory_barrier();
 *	b = 3;				y = b;
 *					read_barrier_depends();
 *					x = a;
 * </programlisting>
 *
 * does not enforce ordering, since there is no data dependency between
 * the read of "a" and the read of "b".  Therefore, on some CPUs, such
 * as Alpha, "y" could be set to 3 and "x" to 0.  Use rmb()
 * in cases like this where there are no data dependencies.
 */

#define read_barrier_depends()		do { } while(0)
#define smp_read_barrier_depends()	do { } while(0)

/*
 * Sync types defined by the MIPS architecture (document MD00087 table 6.5)
 * These values are used with the sync instruction to perform memory barriers.
 * Types of ordering guarantees available through the SYNC instruction:
 * - Completion Barriers
 * - Ordering Barriers
 * As compared to the completion barrier, the ordering barrier is a
 * lighter-weight operation as it does not require the specified instructions
 * before the SYNC to be already completed. Instead it only requires that those
 * specified instructions which are subsequent to the SYNC in the instruction
 * stream are never re-ordered for processing ahead of the specified
 * instructions which are before the SYNC in the instruction stream.
 * This potentially reduces how many cycles the barrier instruction must stall
 * before it completes.
 * Implementations that do not use any of the non-zero values of stype to define
 * different barriers, such as ordering barriers, must make those stype values
 * act the same as stype zero.
 */

/*
 * Completion barriers:
 * - Every synchronizable specified memory instruction (loads or stores or both)
 *   that occurs in the instruction stream before the SYNC instruction must be
 *   already globally performed before any synchronizable specified memory
 *   instructions that occur after the SYNC are allowed to be performed, with
 *   respect to any other processor or coherent I/O module.
 *
 * - The barrier does not guarantee the order in which instruction fetches are
 *   performed.
 *
 * - A stype value of zero will always be defined such that it performs the most
 *   complete set of synchronization operations that are defined.This means
 *   stype zero always does a completion barrier that affects both loads and
 *   stores preceding the SYNC instruction and both loads and stores that are
 *   subsequent to the SYNC instruction. Non-zero values of stype may be defined
 *   by the architecture or specific implementations to perform synchronization
 *   behaviors that are less complete than that of stype zero. If an
 *   implementation does not use one of these non-zero values to define a
 *   different synchronization behavior, then that non-zero value of stype must
 *   act the same as stype zero completion barrier. This allows software written
 *   for an implementation with a lighter-weight barrier to work on another
 *   implementation which only implements the stype zero completion barrier.
 *
 * - A completion barrier is required, potentially in conjunction with SSNOP (in
 *   Release 1 of the Architecture) or EHB (in Release 2 of the Architecture),
 *   to guarantee that memory reference results are visible across operating
 *   mode changes. For example, a completion barrier is required on some
 *   implementations on entry to and exit from Debug Mode to guarantee that
 *   memory effects are handled correctly.
 */

/*
 * stype 0 - A completion barrier that affects preceding loads and stores and
 * subsequent loads and stores.
 * Older instructions which must reach the load/store ordering point before the
 * SYNC instruction completes: Loads, Stores
 * Younger instructions which must reach the load/store ordering point only
 * after the SYNC instruction completes: Loads, Stores
 * Older instructions which must be globally performed when the SYNC instruction
 * completes: Loads, Stores
 */
#define STYPE_SYNC 0x0

/*
 * Ordering barriers:
 * - Every synchronizable specified memory instruction (loads or stores or both)
 *   that occurs in the instruction stream before the SYNC instruction must
 *   reach a stage in the load/store datapath after which no instruction
 *   re-ordering is possible before any synchronizable specified memory
 *   instruction which occurs after the SYNC instruction in the instruction
 *   stream reaches the same stage in the load/store datapath.
 *
 * - If any memory instruction before the SYNC instruction in program order,
 *   generates a memory request to the external memory and any memory
 *   instruction after the SYNC instruction in program order also generates a
 *   memory request to external memory, the memory request belonging to the
 *   older instruction must be globally performed before the time the memory
 *   request belonging to the younger instruction is globally performed.
 *
 * - The barrier does not guarantee the order in which instruction fetches are
 *   performed.
 */

/*
 * stype 0x10 - An ordering barrier that affects preceding loads and stores and
 * subsequent loads and stores.
 * Older instructions which must reach the load/store ordering point before the
 * SYNC instruction completes: Loads, Stores
 * Younger instructions which must reach the load/store ordering point only
 * after the SYNC instruction completes: Loads, Stores
 * Older instructions which must be globally performed when the SYNC instruction
 * completes: N/A
 */
#define STYPE_SYNC_MB 0x10


#ifdef CONFIG_CPU_HAS_SYNC
#define __sync()				\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		".set	mips2\n\t"		\
		"sync\n\t"			\
		".set	pop"			\
		: /* no output */		\
		: /* no input */		\
		: "memory")
#else
#define __sync()	do { } while(0)
#endif

#define __fast_iob()				\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		"lw	$0,%0\n\t"		\
		"nop\n\t"			\
		".set	pop"			\
		: /* no output */		\
		: "m" (*(int *)CKSEG1)		\
		: "memory")
#ifdef CONFIG_CPU_CAVIUM_OCTEON
# define OCTEON_SYNCW_STR	".set push\n.set arch=octeon\nsyncw\nsyncw\n.set pop\n"
# define __syncw() 	__asm__ __volatile__(OCTEON_SYNCW_STR : : : "memory")

# define fast_wmb()	__syncw()
# define fast_rmb()	barrier()
# define fast_mb()	__sync()
# define fast_iob()	do { } while (0)
#else /* ! CONFIG_CPU_CAVIUM_OCTEON */
# define fast_wmb()	__sync()
# define fast_rmb()	__sync()
# define fast_mb()	__sync()
# ifdef CONFIG_SGI_IP28
#  define fast_iob()				\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		"lw	$0,%0\n\t"		\
		"sync\n\t"			\
		"lw	$0,%0\n\t"		\
		".set	pop"			\
		: /* no output */		\
		: "m" (*(int *)CKSEG1ADDR(0x1fa00004)) \
		: "memory")
# else
#  define fast_iob()				\
	do {					\
		__sync();			\
		__fast_iob();			\
	} while (0)
# endif
#endif /* CONFIG_CPU_CAVIUM_OCTEON */

#ifdef CONFIG_CPU_HAS_WB

#include <asm/wbflush.h>

#define wmb()		fast_wmb()
#define rmb()		fast_rmb()
#define mb()		wbflush()
#define iob()		wbflush()

#else /* !CONFIG_CPU_HAS_WB */

#define wmb()		fast_wmb()
#define rmb()		fast_rmb()
#define mb()		fast_mb()
#define iob()		fast_iob()

#endif /* !CONFIG_CPU_HAS_WB */

#if defined(CONFIG_WEAK_ORDERING) && defined(CONFIG_SMP)
# ifdef CONFIG_CPU_CAVIUM_OCTEON
#  define smp_mb()	__sync()
#  define smp_rmb()	barrier()
#  define smp_wmb()	__syncw()
# else
#  define smp_mb()	__asm__ __volatile__("sync" : : :"memory")
#  define smp_rmb()	__asm__ __volatile__("sync" : : :"memory")
#  define smp_wmb()	__asm__ __volatile__("sync" : : :"memory")
# endif
#else
#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#endif

#if defined(CONFIG_WEAK_REORDERING_BEYOND_LLSC) && defined(CONFIG_SMP)
#define __WEAK_LLSC_MB		"       sync	\n"
#else
#define __WEAK_LLSC_MB		"		\n"
#endif

#define set_mb(var, value) \
	do { var = value; smp_mb(); } while (0)

#define smp_llsc_mb()	__asm__ __volatile__(__WEAK_LLSC_MB : : :"memory")

#ifdef CONFIG_CPU_CAVIUM_OCTEON
#define smp_mb__before_llsc() smp_wmb()
/* Cause previous writes to become visible on all CPUs as soon as possible */
#define nudge_writes() __asm__ __volatile__(".set push\n\t"		\
					    ".set arch=octeon\n\t"	\
					    "syncw\n\t"			\
					    ".set pop" : : : "memory")
#else
#define smp_mb__before_llsc() smp_llsc_mb()
#define nudge_writes() do { } while (0)
#endif

#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	smp_mb();							\
	WRITE_ONCE(*p, v);						\
} while (0)

#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	smp_mb();							\
	___p1;								\
})

#endif /* __ASM_BARRIER_H */
