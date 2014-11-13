#
# lldp_CHECK_SNMP
#
AC_DEFUN([lldp_CHECK_SNMP], [
   AC_PATH_TOOL([NETSNMP_CONFIG], [net-snmp-config], [no])
   if test x"$NETSNMP_CONFIG" = x"no"; then
      AC_MSG_ERROR([*** unable to find net-snmp-config])
   fi
   NETSNMP_LIBS=`${NETSNMP_CONFIG} --agent-libs`
   NETSNMP_CFLAGS="`${NETSNMP_CONFIG} --base-cflags` -DNETSNMP_NO_INLINE"

   _save_flags="$CFLAGS"
   _save_libs="$LIBS"
   CFLAGS="$CFLAGS ${NETSNMP_CFLAGS}"
   LIBS="$LIBS ${NETSNMP_LIBS}"
   AC_MSG_CHECKING([whether C compiler supports flag "${NETSNMP_CFLAGS} ${NETSNMP_LIBS}" from Net-SNMP])
   AC_LINK_IFELSE([AC_LANG_PROGRAM([
int main(void);
],
[
{
  return 0;
}
])],[AC_MSG_RESULT(yes)],[
AC_MSG_RESULT(no)
AC_MSG_ERROR([*** incorrect CFLAGS from net-snmp-config])])
   # Is Net-SNMP usable?
   AC_CHECK_LIB([netsnmp], [snmp_register_callback], [:],
       [AC_MSG_ERROR([*** unable to use net-snmp])], ${NETSNMP_LIBS})
   # Do we have subagent support?
   AC_CHECK_FUNCS([netsnmp_enable_subagent], [:],
       [AC_MSG_ERROR([*** no subagent support in net-snmp])])

   AC_SUBST([NETSNMP_LIBS])
   AC_SUBST([NETSNMP_CFLAGS])
   AC_DEFINE_UNQUOTED([USE_SNMP], 1, [Define to indicate to enable SNMP support])

   # Should we use f_create_from_tstring_new or f_create_from_tstring?
   AC_CHECK_MEMBERS([netsnmp_tdomain.f_create_from_tstring_new],,,
	[
@%:@include <net-snmp/net-snmp-config.h>
@%:@include <net-snmp/net-snmp-includes.h>
@%:@include <net-snmp/library/snmp_transport.h>
])
   # Do we have a usable <net-snmp/agent/util_funcs.h> header?
   AC_CHECK_HEADERS([net-snmp/agent/util_funcs.h],,,[
@%:@include <net-snmp/net-snmp-config.h>
@%:@include <net-snmp/net-snmp-includes.h>
@%:@include <net-snmp/library/snmp_transport.h>
@%:@include <net-snmp/agent/net-snmp-agent-includes.h>
@%:@include <net-snmp/agent/snmp_vars.h>
   ])

   CFLAGS="$_save_flags"
   LIBS="$_save_libs"
])
