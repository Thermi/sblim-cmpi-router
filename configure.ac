# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author:       Federico Martin Casares (warptrosse@gmail.com)
# Description:  Process this file with autoconf to produce a
#               configure script.

# Autoconf version required.
AC_PREREQ(2.59)

# Package information.
AC_INIT(SBLIM Router Providers, 2.0.0, warptrosse@gmail.com, sblim-cmpi-router)

# Identify the source directory.
AC_CONFIG_SRCDIR([src/OSBase_NextHopIPRoute.c])

# Autogenerate the autoconf header file to store build settings.
AC_CONFIG_HEADER([config.h])

# Autogenerate the Makefile.
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# Configure command line options.
AC_ARG_VAR([TESTSUITEDIR], [the directory where the SBLIM testsuite resides.])
AC_ARG_VAR([PROVIDERDIR], [the directory where the CMPI providers will be installed.])
AC_ARG_VAR([CIMSERVER], [the target CIM server (pegasus|sfcb|openwbem|sniacimom).])

# Check for the required C compiler.
AC_PROG_CC

# Check for the required install program.
AC_PROG_INSTALL

# Check for the required libtool.
AC_PROG_LIBTOOL

# Check if the C compiler supports 'const'.
AC_C_CONST

# Check for required CMPI header files (defined in acinclude.m4).
CHECK_CMPI

# Check for required libraries (defined in acinclude.m4).
CHECK_PROVIDERDIR

# Check for CIM Server (defined in acinclude.m4).
CHECK_CIMSERVER

# Check for SBLIM Base.
CHECK_SBLIM_BASE

# Check for the SBLIM testsuite (this macro is defined in acinclude.m4).
CHECK_TESTSUITE

# Check if the system headers conform to ANSI C
AC_HEADER_STDC

AC_HEADER_DIRENT

AC_HEADER_SYS_WAIT

# Check for some common required headers.
AC_CHECK_HEADERS([stdlib.h stdio.h string.h unistd.h signal.h errno.h time.h sys/socket.h sys/param.h sys/types.h netinet/in.h linux/netlink.h linux/rtnetlink.h linux/netdevice.h linux/if_arp.h linux/if.h net/if.h])

# Checks for library functions.
AC_FUNC_GETMNTENT

# Check malloc function.
AC_FUNC_MALLOC

# Check realloc function.
AC_FUNC_REALLOC

# Check required functions.
AC_CHECK_FUNCS([calloc malloc memset memcpy free getdomainname gethostname snprintf sscanf atoi inet_ntop inet_pton strlen socket setsockopt bind getsockname close sendmsg recvmsg if_indextoname])

# Config files.
AC_CONFIG_FILES([Makefile])

# Generate configure scripts for the Makefile
if test x"$TESTSUITEDIR" == x; then
   TESTSUITE_SUPPORT=Disabled
else	
   TESTSUITE_SUPPORT=Enabled
fi

# Display configuration options
echo "-------------------------------------------------------"
echo "Configuration for $PACKAGE complete."
echo ""
echo "The following configuration options have been selected:"
echo "CIMSERVER: " $CIMSERVER
echo "PROVIDERDIR: " $PROVIDERDIR
echo "CPPFLAGS:" $CPPFLAGS
echo "Test Suite Support" $TESTSUITE_SUPPORT
if test x"$TESTSUITEDIR" != x; then
echo "TESTSUITEDIR " $TESTSUITEDIR
fi
echo "-------------------------------------------------------"

# Generate configure scripts for the Makefile.
AC_OUTPUT

echo "You may now run make"
