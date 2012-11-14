#ifndef _OSBASE_NEXTHOPIPROUTE_H_
#define _OSBASE_NEXTHOPIPROUTE_H_

/**
 * @file
 * OSBase_NextHopIPRoute.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * @author       Federico Martin Casares (warptrosse@gmail.com)
 *
 * @contributors
 *
 * @description
 * This file defines the interfaces for the resource access implementation
 * of the CIM class Linux_NextHopIPRoute.
 */

#include "OSBase_Netlink.h"

#ifdef __cplusplus
extern "C" {
#endif

    ////////////////////////////////////////////////////////////////////////////
    // NEXT-HOP IP ROUTE
    ////////////////////////////////////////////////////////////////////////////

#define NH_OK   0 /**< Operation executed succesful. */
#define NH_FAIL 1 /**< An error occurs when executing desired operation. */

    /**
     * Address type as string max length.
     */
#define ADDR_TYPE_STR_MAXLEN 7

    /**
     * Maximum hostName length.
     */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif //MAXHOSTNAMELEN

    /**
     * InstanceID format. It is supposed that it will be not changed.
     * hostName|dstAddr|dstLen|table|outputIf|scope|gw
     */
#define INSTANCEID_FORMAT "%s|%s|%d|%d|%d|%d|%s"

    /**
     * InstanceID format for parse. It is supposed that it will be not changed.
     */
#define INSTANCEID_FORMAT_PARSE "%[^|]|%[^|]|%d|%d|%d|%d|%[^|]"

    /**
     * CIM - Describes the format of the address properties.
     */
    enum {
        NH_AT_UNKNOWN, /**< Unidentified. */
        NH_AT_IPV4,    /**< IP version 4. */
        NH_AT_IPV6     /**< IP version 6. */
    };

    /**
     * CIM - How the route was derived.
     */
    enum {
        NH_RTD_UNKNOWN,      /**< Unknown. */
        NH_RTD_OTHER,        /**< Other. */
        NH_RTD_CONNECTED,    /**< Connected. */
        NH_RTD_USER_DEFINED, /**< User-Defined. */
        NH_RTD_IGRP,         /**< IGRP. */
        NH_RTD_EIGRP,        /**< EIGRP. */
        NH_RTD_RIP,          /**< RIP. */
        NH_RTD_HELLO,        /**< HELLO. */
        NH_RTD_EGP,          /**< EGP. */
        NH_RTD_BGP,          /**< BGP. */
        NH_RTD_ISIS,         /**< ISIS. */
        NH_RTD_OSPF          /**< OSPF. */
    };

    /**
     * CIM - Route scope.
     */
    enum {
        NH_RTS_UNIVERSE = 0,   /**< Everywhere in the "Universe". */
        NH_RTS_SITE     = 200, /**< Internal routes (only IPv6). */
        NH_RTS_LINK     = 253, /**< Destination located on directly attached
                                  link. */
        NH_RTS_HOST     = 254, /**< Local address. */
        NH_RTS_NOWHERE  = 255  /**< Not existing destination. */
    };

    /**
     * CIM - Routing table.
     */
    enum {
        NH_RTT_UNSPEC  = 0,   /**< Operate on all routing tables
                                 simultaneously. */
        NH_RTT_COMPAT  = 252, /**< ?. */
        NH_RTT_DEFAULT = 253, /**< ?. */
        NH_RTT_MAIN    = 254, /**< Main kernel table. */
        NH_RTT_LOCAL   = 255  /**< Special routing table maintained by the
                                 kernel. */
    };

    /**
     * CIM - Route Type.
     */
    enum {
        NH_RTTY_UNSPEC,      /**< Type not identified. */
        NH_RTTY_UNICAST,     /**< Gateway or direct route. */
        NH_RTTY_LOCAL,       /**< Accept locally. */
        NH_RTTY_BROADCAST,   /**< Accept locally as broadcast, send
                                as broadcast. */
        NH_RTTY_ANYCAST,     /**< Accept locally as broadcast, but send
                                as unicast. */
        NH_RTTY_MULTICAST,   /**< Multicast route. */
        NH_RTTY_BLACKHOLE,   /**< Drop. */
        NH_RTTY_UNREACHABLE, /**< Destination is unreachable. */
        NH_RTTY_PROHIBIT,    /**< Administratively prohibited. */
        NH_RTTY_THROW,       /**< Not in this table. */
        NH_RTTY_NAT,         /**< Translate this address. */
        NH_RTTY_XRESOLVE     /**< Use external resolver. */
    };

    /**
     * CIM - Type of route.
     */
    enum {
        NH_TOR_ADMINISTRATOR = 2, /**< Administrator-defined. */
        NH_TOR_COMPUTED,          /**< Computed via a routing
                                     protocol/algorithm. */
        NH_TOR_ACTUAL             /**< Actual route implemented in the
                                     network. */
    };

    /**
     * This structure stores the route's properties.
     */
    struct nextHopIP {
        unsigned short addressType;         /**< The format of the address
                                               properties. */
        unsigned short adminDistance;       /**< Administrative distance of this
                                               route. */
        char           caption[64];         /**< Short textual description of
                                               the object. */
        char           description[256];    /**< Textual description of the
                                               object. */
        char           dstAddress[64];      /**< Destination address to be
                                               reached. */
        char           dstMask[64];         /**< The mask for the destination
                                               address. */
        char           elementName[64];     /**< User-friendly name for the
                                               object. */
        char           instanceID[64];      /**< Opaquely and uniquely identify
                                               an instance of this class. */
        unsigned short isStatic;            /**< TRUE indicates that this is
                                               a static route. */
        char           otherDerivation[64]; /**< A string describing how the
                                               route was derived. */
        unsigned char  prefixLength;        /**< The prefix length for the IPv6
                                               destination address. */
        unsigned short routeDerivation;     /**< How the route was derived. */
        char           routeGateway[64];    /**< The gateway of the route. */
        unsigned short routeMetric;         /**< Numeric indication as to the
                                               preference of this route. */
        unsigned short routeOutputIf;       /**< Output interface index. */
        unsigned short routeScope;          /**< Sort of distance to the
                                               destination. */
        unsigned short routeTable;          /**< Routing table id. */
        unsigned short routeType;           /**< Type of route. */
        unsigned short typeOfRoute;         /**< Administrator Defined Route,
                                               Computed Route or Actual
                                               Route. */
    };

    /**
     * This structure is used as NextHopIPRoutes list.
     */
    struct nextHopIPList {
        struct nextHopIP     *sptr;  /**< Pointer to current position
                                        in the list. */
        struct nextHopIPList *next;  /**< Pointer to next position
                                        in the list. */
    };

    /**
     * This functions is used to get a list of routes based on nlRtInfo
     * filter.
     * @param nHopIPList [out] routes struct to be filled.
     * @param nlRtInfo [in] route info.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int getIPRoutes(struct nextHopIPList **nHopIPList,
                    struct nlRouteInfo *nlRtInfo);

    /**
     * This functions is used to get all ipv4 and ipv6 routes from fib.
     * @param nHopIPList [out] routes struct to be filled.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int getAllIPRoutes(struct nextHopIPList **nHopIPList);

    /**
     * This functions is used to get a route base on specified InstanceID.
     * @param nHopIPList [out] routes struct to be filled.
     * @param instanceId [in] route InstanceID.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int getIPRouteIId(struct nextHopIPList **nHopIPList, const char *instanceId);

    /**
     * This function is used to add a new route to FIB.
     * @param nHopIP [in] route to be added.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int addIPRoute(struct nextHopIP *nHopIP);

    /**
     * This function is used to delete a route from FIB.
     * @param nHopIP [in] route to be deleted.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int delIPRoute(const struct nextHopIP *nHopIP);

    /**
     * Converts a nlRouteInfoList structure to nextHopIPList strucutre.
     * @param nlRtInfoList [in] list to be converted.
     * @param nHopIPList [out] converted list.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int nlListTOnhList(struct nlRouteInfoList **nlRtInfoList,
                       struct nextHopIPList **nHopIPList);

    /**
     * Converts a nextHopIP structure to nlRouteInfo structure.
     * @param nHopIP [in] structure to be converted.
     * @param nlRtInfo [out] converted structure.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int nhTOnlInfo(struct nextHopIP *nHopIP, struct nlRouteInfo *nlRtInfo);

    /**
     * Converts a nlRouteInfo structure to nextHopIP structure.
     * @param nlRtInfo [in] structure to be converted.
     * @param nHopIP [out] converted structure.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int nlInfoTOnh(const struct nlRouteInfo *nlRtInfo, struct nextHopIP *nHopIP);

    /**
     * Used to get CIM address type from specified route address.
     * @param addr [int] route address.
     * @return CIM address type.
     */
    int getAddrType(const char *addr);

    /**
     * Used to get CIM address type in string format.
     * @param addressType [in] address type identifier.
     * @param addrTypeStr [out] address type string.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int getAddrTypeStr(int addressType, char *addrTypeStr);

    /**
     * This function is used to clean a nextHopIP list.
     * @param lptr [in] list to be cleaned.
     * @return NH_OK=succesful | NH_FAIL=fail
     */
    int freeNextHopIPList(struct nextHopIPList *lptr);

    ////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif //_OSBASE_NEXTHOPIPROUTE_H_
