# ==============================================================================
# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author:       Federico Martin Casares (warptrosse@gmail.com)
# Description:  Automake input file for OS Base Providers
#               Based on cmpi-network Makefile.am by Viktor Mihajlovski.
# ==============================================================================

################################################################################
## General
################################################################################
# SBLIM Include Directory.
sblimincdir=$(includedir)/sblim

# Start from an empty extra distribution file list.
EXTRA_DIST=

# docdir defines where the documentation goes.
docdir=$(datadir)/doc/$(PACKAGE)-$(VERSION)
################################################################################


################################################################################
## Automake instructions for documentation
################################################################################
doc_DATA=README AUTHORS COPYING 

# Providerdir defines where provider libraries will be installed
# (@PROVIDERDIR@ is set by the configure script). 
providerdir=@PROVIDERDIR@
################################################################################


################################################################################
## Automake instructions for main dir.
################################################################################
# OSBase CMPI provider libraries.
provider_LTLIBRARIES=libcmpiOSBase_NextHopIPRouteProvider.la \
	             libcmpiOSBase_CSHostedRouteProvider.la \
                     libcmpiOSBase_LANEndpointProvider.la \
	             libcmpiOSBase_RouteUsesEndpointProvider.la

# NextHopIP.
libcmpiOSBase_NextHopIPRouteProvider_la_SOURCES=\
		src/cmpiOSBase_NextHopIPRouteProvider.c \
		src/cmpiOSBase_NextHopIPRoute.c \
                src/OSBase_Netlink.c \
                src/OSBase_NextHopIPRoute.c
libcmpiOSBase_NextHopIPRouteProvider_la_LIBADD=\
		-L/usr/local/lib -lcmpiOSBase_Common
libcmpiOSBase_NextHopIPRouteProvider_la_LDFLAGS=-avoid-version

# CSHostedRoute (Association).
libcmpiOSBase_CSHostedRouteProvider_la_SOURCES = \
		src/cmpiOSBase_CSHostedRouteProvider.c
libcmpiOSBase_CSHostedRouteProvider_la_LIBADD=\
		-L/usr/local/lib -lcmpiOSBase_Common
libcmpiOSBase_CSHostedRouteProvider_la_LDFLAGS = -avoid-version

# LANEndpoint.
libcmpiOSBase_LANEndpointProvider_la_SOURCES=\
		src/cmpiOSBase_LANEndpointProvider.c \
		src/cmpiOSBase_LANEndpoint.c \
                src/OSBase_Netlink.c \
                src/OSBase_LANEndpoint.c
libcmpiOSBase_LANEndpointProvider_la_LIBADD=\
		-L/usr/local/lib -lcmpiOSBase_Common
libcmpiOSBase_LANEndpointProvider_la_LDFLAGS=-avoid-version

# RouteUsesEndpointProvider (Association).
libcmpiOSBase_RouteUsesEndpointProvider_la_SOURCES = \
		src/cmpiOSBase_RouteUsesEndpointProvider.c \
		src/cmpiOSBase_RouteUsesEndpoint.c
libcmpiOSBase_RouteUsesEndpointProvider_la_LIBADD=\
		-L/usr/local/lib -lcmpiOSBase_Common
libcmpiOSBase_RouteUsesEndpointProvider_la_LDFLAGS = -avoid-version

# Installable Header Files.
#sbliminc_HEADERS = OSBase_CommonNetwork.h
INCLUDES=-I@top_srcdir@/include

# Non-Installable Header Files.
noinst_HEADERS=include/cmpiOSBase_NextHopIPRoute.h \
	include/OSBase_Netlink.h \
	include/OSBase_NextHopIPRoute.h \
	include/cmpiOSBase_LANEndpoint.h \
	include/OSBase_LANEndpoint.h \
	include/cmpiOSBase_RouteUsesEndpoint.h

# We must explicity add the RPM spec file to the distribution
# package.
EXTRA_DIST+=$(PACKAGE).spec $(PACKAGE).rh.spec
################################################################################


################################################################################
## Automake instructions for ./mof subdir
################################################################################
# cmpi-router schema.
SCHEMAS=mof/Linux_Router.mof

# cmpi-router schema registration.
REGISTRATIONS=mof/Linux_Router.registration

# We must explicity add all the schema files to the distribution
# package.
pkgdata_DATA=$(SCHEMAS) $(REGISTRATIONS)

# Script to register providers.
pkgdata_SCRIPTS=provider-register.sh

EXTRA_DIST+=mof $(pkgdata_SCRIPTS)

# Register the provider(s) and class definition(s) to the current
# CIM server/CIMOM.
postinstall:
	sh provider-register.sh -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

preuninstall:
	sh provider-register.sh -d -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

dist-hook:
	test -d "$(distdir)" && rm -rf `find $(distdir) -type d -name CVS`
################################################################################
