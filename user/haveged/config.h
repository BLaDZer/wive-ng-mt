/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 for clock_gettime */
#define ENABLE_CLOCK_GETTIME 1

/* Library version */
#define HAVEGE_LIB_VERSION "1:1:0"

/* Define to 1 if you have the <cpuid.h> header file. */
/* #undef HAVE_CPUID_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `floor' function. */
/* #undef HAVE_FLOOR */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define generic host */
#define HAVE_ISA_GENERIC "mipsel-unknown-linux-gnu"

/* Define to 1 for ia64 */
/* #undef HAVE_ISA_IA64 */

/* Define to 1 for ppc */
/* #undef HAVE_ISA_PPC */

/* Define to 1 for s390 */
/* #undef HAVE_ISA_S390 */

/* Define to 1 for sparc */
/* #undef HAVE_ISA_SPARC */

/* Define to 1 for sparclite */
/* #undef HAVE_ISA_SPARCLITE */

/* Define to 1 for x86 */
/* #undef HAVE_ISA_X86 */

/* Define to 1 if you have the <linux/random.h> header file. */
#define HAVE_LINUX_RANDOM_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `pow' function. */
/* #undef HAVE_POW */

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the `sched_yield' function. */
#define HAVE_SCHED_YIELD 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the <semaphore.h> header file. */
#define HAVE_SEMAPHORE_H 1

/* Define to 1 if you have the `sqrt' function. */
/* #undef HAVE_SQRT */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if the system has the type `uint32_t'. */
#define HAVE_UINT32_T 1

/* Define to 1 if the system has the type `uint8_t'. */
#define HAVE_UINT8_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <x86intrin.h> header file. */
/* #undef HAVE_X86INTRIN_H */

/* Define to 1 if you have the `__rdtsc' function. */
/* #undef HAVE___RDTSC */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 to suppress daemon interface */
/* #undef NO_DAEMON */

/* Define to single collection thread */
#define NUMBER_CORES 1

/* Define to 1 for online test features */
/* #undef ONLINE_TESTS_ENABLE */

/* Name of package */
#define PACKAGE "haveged"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "haveged"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "haveged 1.9.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "haveged"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.9.2"

/* Define to 1 for injection diagnostic */
/* #undef RAW_IN_ENABLE */

/* Define to 1 for capture diagnostic */
/* #undef RAW_OUT_ENABLE */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 to tune with host cpuid */
#define TUNING_CPUID_ENABLE 1

/* Define to 1 to tune wiht host vfs */
#define TUNING_VFS_ENABLE 1

/* Version number of package */
#define VERSION "1.9.2"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
