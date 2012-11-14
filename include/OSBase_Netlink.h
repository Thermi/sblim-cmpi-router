#ifndef _OSBASE_NETLINK_H_
#define _OSBASE_NETLINK_H_

/**
 * @file
 * OSBase_NetLink.h
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
 * @description Netlink support lib.
 */

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    ////////////////////////////////////////////////////////////////////////////
    // COMMON
    ////////////////////////////////////////////////////////////////////////////

#define NL_OK   0 /**< Operation executed succesful. */
#define NL_FAIL 1 /**< An error occurs when executing desired operation. */
#define NL_SKIP 2 /**< Indicates that the operation has fail, but it was skipped
                     due to their optional condition. */

#define NL_SOCK_SND_BUFF_LEN 32768 /**< Netlink socket send buffer length. */
#define NL_SOCK_RCV_BUFF_LEN 32768 /**< Netlink socket receive buffer length. */

#define NL_ADDRESS_MAXLEN 64 /**< Address string maximum length. */

    /**
     * Get the size of an array.
     */
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

    /**
     * Safe clean pointers.
     */
#define FREE_SAFE(ptr) { if(ptr != NULL) { free(ptr); ptr = NULL; } }

    /**
     * Netlink attributes.
     */
#define NLMSG_TAIL(nmsg)                                                \
    ((struct rtattr*) (((void*) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

    /**
     * Netlink socket handler.
     */
    struct nlSockHandle {
        int                fd;    /**< File descriptor for the new socket. */
        struct sockaddr_nl local; /**< Netlink client in user-space. */
    };

    /**
     * Used to open a Netlink socket.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlOpenSocket();

    /**
     * Used to close a previously open Netlink socket.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlCloseSocket();

    /**
     * This function is used to add an attribute to a netlink message.
     * @param h [out] message header.
     * @param maxlen [in] message maximum length.
     * @param attrType [in] attribute type.
     * @param attrData [in] attribute data.
     * @param attrBytelen [in] attribute length.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlAddAttrToMsg(struct nlmsghdr *h, int maxlen, int attrType,
                       const void *attrData, int attrBytelen);

    /**
     * This function is used to add an attribute to a netlink message.
     * @param h [out] message header.
     * @param maxlen [in] message length.
     * @param attrType [in] attribute type.
     * @param attrData [in] attribute data.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlAddAttrToMsg32(struct nlmsghdr *h, int maxlen,
                         int attrType, unsigned int attrData);

    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    // ROUTES
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Information of a single route.
     */
    struct nlRouteInfo {
        int  family;                       /**< IPv4=AF_INET ; IPv6=AF_INET6
                                              (from rtmsg). */
        int  type;                         /**< Route type (from rtmsg). */
        int  protocol;                     /**< Route origin (from rtmsg). */
        int  scope;                        /**< Distance to the destination
                                              (from rtmsg). */
        int  srcLen;                       /**< Source Route mask prefix
                                              (from rtmsg). */
        int  dstLen;                       /**< Destination Route mask prefix
                                              (from rtmsg). */
        int  tos;                          /**< Type of service (from rtmsg). */
        char dstAddr[NL_ADDRESS_MAXLEN+1]; /**< Destination address to be
                                              reached (RTA_DST). */
        char srcAddr[NL_ADDRESS_MAXLEN+1]; /**< Source address (RTA_SRC). */
        int  inputIf;                      /**< Input interface index
                                              (RTA_IIF). */
        int  outputIf;                     /**< Output interface index
                                              (RTA_OIF). */
        char gw[64];                       /**< Gateway of the route
                                              (RTA_GATEWAY). */
        int  priority;                     /**< Priority of the route
                                              (RTA_PRIORITY). */
        char prefSrc[NL_ADDRESS_MAXLEN+1]; /**< Prefered source
                                              (RTA_PREFSRC). */
        int  metrics;                      /**< Route metrics (RTA_METRICS). */
        int  table;                        /**< Route table (RTA_TABLE). */
    };

    /**
     * Routes container.
     */
    struct nlRouteInfoList {
        struct nlRouteInfo     *sptr; /**< Pointer to current position
                                         in the list. */
        struct nlRouteInfoList *next; /**< Pointer to next position
                                         in the list. */
    };

    /**
     * Struct used as route filter. If a field is deactivated (= 0),
     * it will be skiped.
     */
    struct {
        unsigned int family   : 1;
        unsigned int type     : 1;
        unsigned int protocol : 1;
        unsigned int scope    : 1;
        unsigned int srcLen   : 1;
        unsigned int dstLen   : 1;
        unsigned int tos      : 1;
        unsigned int dstAddr  : 1;
        unsigned int srcAddr  : 1;
        unsigned int inputIf  : 1;
        unsigned int outputIf : 1;
        unsigned int gw       : 1;
        unsigned int priority : 1;
        unsigned int prefSrc  : 1;
        unsigned int metrics  : 1;
        unsigned int table    : 1;
    } rtFlt;

    /**
     * This function is used to get routes using a nlRouteInfo
     * structure as filter.
     * @param nlRtInfoList [out] routes list (one or more entries).
     * @param nlRtInfo [in] route info used as filter.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlGetRoutes(struct nlRouteInfoList **nlRtInfoList,
                    struct nlRouteInfo *nlRtInfo);

    /**
     * This function is used to ADD/DELETE/MODIFY a route.
     * @param nlRtInfo [in] route info.
     * @param hType [in] type of message (RTM_NEWROUTE || RTM_DELROUTE).
     * @param hFlags [in] message flags.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlModifyRoute(struct nlRouteInfo *nlRtInfo, int hType,
                      unsigned int hFlags);

    /**
     * This function is used to add a route to specified routes list.
     * @param rcvH [in] route message header.
     * @param nlRtInfoList [out] list where route will be added.
     * @param nlRtInfo [in] route info.
     * @return NL_OK=succesful | NL_FAIL=fail | NL_SKIP=skip
     */
    int nlAddRouteToList(const struct nlmsghdr *rcvH,
                         struct nlRouteInfoList **nlRtInfoList,
                         const struct nlRouteInfo *nlRtInfo);

    /**
     * Used to generate a filter that will be applied to a list of routes.
     * @param nlRtInfo [in] route info.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlGenRouteFilter(struct nlRouteInfo *nlRtInfo);

    /**
     * Used to reset the route filter handler.
     */
    void nlResetRouteFilter();

    /**
     * Used to create a nlRouteInfo structure with default values.
     * @param nlRtInfo [out] route structure.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlCreateDefaultRtInfo(struct nlRouteInfo *nlRtInfo);

    /**
     * Used to free a route info list.
     * @param nlRtInfoList [out] routes list (one or more entries).
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlFreeRouteInfoList(struct nlRouteInfoList *nlRtInfoList);

    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    // LINKS
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Information of a single link.
     */
    struct nlLinkInfo {
        unsigned char           family;            /**< AF_UNSPEC (from
                                                      ifinfomsg). */
        unsigned short          type;              /**< Link type - ARPHRD_*
                                                      (from ifinfomsg). */
        int                     index;             /**< Unique interface
                                                      index. */
        unsigned int            flags;             /**< Device flags
                                                      (see netdevice(7)). */
        unsigned int            change;            /**< Reserved for future
                                                      use. */
        unsigned char           address[32];       /**< Interface L2 address
                                                      (IFLA_ADDRESS). */
        int                     addressLen;        /**< Used to store the length
                                                      of the address. */
        unsigned char           broadcast[32];     /**< L2 broadcast address
                                                      (IFLA_BROADCAST). */
        int                     broadcastLen;      /**< Used to store the length
                                                      of the broadcast. */
        char                    ifname[IFNAMSIZ];  /**< Device name
                                                      (IFLA_IFNAME). */
        unsigned int            mtu;               /**< MTU of the device
                                                      (IFLA_MTU). */
        int                     link;              /**< Link type
                                                      (IFLA_LINK). */
        char                    qdisc[64];         /**< Queueing discipline
                                                      (IFLA_QDISC). */
        struct net_device_stats stats;             /**< Interface statistics
                                                      (IFLA_STATS). */
        // IFLA_COST
        // IFLA_PRIORITY
        // IFLA_MASTER
        // IFLA_WIRELESS
        // IFLA_PROTINFO
        int                     txqlen;            /**< Tx queue length. */
        struct ifmap            map;               /**< Device mapping
                                                      structure. */
        // IFLA_WEIGHT
        unsigned char           operstate;         /**< Operational state
                                                      (rfc 2863). */
        unsigned char           linkmode;          /**< Link mode. */
        // IFLA_LINKINFO
        // IFLA_NET_NS_PID
#ifdef IFLA_IFALIAS
        char                    ifalias[IFALIASZ]; /**< Interface alias. */
#endif //IFLA_IFALIAS
    };

    /**
     * Links container.
     */
    struct nlLinkInfoList {
        struct nlLinkInfo     *sptr; /**< Pointer to current position
                                        in the list. */
        struct nlLinkInfoList *next; /**< Pointer to next position
                                        in the list. */
    };

    /**
     * Struct used as link filter. If a field is deactivated (= 0),
     * it will be skiped.
     */
    struct {
        unsigned int family    : 1;
        unsigned int type      : 1;
        unsigned int index     : 1;
        unsigned int flags     : 1;
        unsigned int change    : 1;
        unsigned int address   : 1;
        unsigned int broadcast : 1;
        unsigned int ifname    : 1;
        unsigned int mtu       : 1;
        unsigned int link      : 1;
        unsigned int qdisc     : 1;
        unsigned int stats     : 1;
        // IFLA_COST
        // IFLA_PRIORITY
        // IFLA_MASTER
        // IFLA_WIRELESS
        // IFLA_PROTINFO
        unsigned int txqlen    : 1;
        unsigned int map       : 1;
        // IFLA_WEIGHT
        unsigned int operstate : 1;
        unsigned int linkmode  : 1;
        // IFLA_LINKINFO
        // IFLA_NET_NS_PID
#ifdef IFLA_IFALIAS
        unsigned int ifalias   : 1;
#endif //IFLA_IFALIAS
    } linkFlt;

    /**
     * The list of link types.
     * (from kernel/net/core/dev.c)
     */
    static const unsigned short linkType[] = {
        ARPHRD_NETROM, ARPHRD_ETHER, ARPHRD_EETHER, ARPHRD_AX25,
        ARPHRD_PRONET, ARPHRD_CHAOS, ARPHRD_IEEE802, ARPHRD_ARCNET,
        ARPHRD_APPLETLK, ARPHRD_DLCI, ARPHRD_ATM, ARPHRD_METRICOM,
        ARPHRD_IEEE1394, ARPHRD_EUI64, ARPHRD_INFINIBAND, ARPHRD_SLIP,
        ARPHRD_CSLIP, ARPHRD_SLIP6, ARPHRD_CSLIP6, ARPHRD_RSRVD,
        ARPHRD_ADAPT, ARPHRD_ROSE, ARPHRD_X25, ARPHRD_HWX25, ARPHRD_CAN,
        ARPHRD_PPP, ARPHRD_CISCO, ARPHRD_HDLC, ARPHRD_LAPB, ARPHRD_DDCMP,
        ARPHRD_RAWHDLC, ARPHRD_TUNNEL, ARPHRD_TUNNEL6, ARPHRD_FRAD,
        ARPHRD_SKIP, ARPHRD_LOOPBACK, ARPHRD_LOCALTLK, ARPHRD_FDDI,
        ARPHRD_BIF, ARPHRD_SIT, ARPHRD_IPDDP, ARPHRD_IPGRE,
        ARPHRD_PIMREG, ARPHRD_HIPPI, ARPHRD_ASH, ARPHRD_ECONET,
        ARPHRD_IRDA, ARPHRD_FCPP, ARPHRD_FCAL, ARPHRD_FCPL,
        ARPHRD_FCFABRIC, ARPHRD_IEEE802_TR, ARPHRD_IEEE80211,
        ARPHRD_IEEE80211_PRISM, ARPHRD_IEEE80211_RADIOTAP, ARPHRD_VOID,
        ARPHRD_NONE
    };

    /**
     * The list of link types string representations.
     * (from kernel/net/core/dev.c)
     */
    static const char *linkTypeName[] = {
        "NETROM", "ETHER", "EETHER", "AX25",
        "PRONET", "CHAOS", "IEEE802", "ARCNET",
        "APPLETLK", "DLCI", "ATM", "METRICOM",
        "IEEE1394", "EUI64", "INFINIBAND", "SLIP",
        "CSLIP", "SLIP6", "CSLIP6", "RSRVD",
        "ADAPT", "ROSE", "X25", "HWX25", "CAN"
        "PPP", "CISCO", "HDLC", "LAPB", "DDCMP",
        "RAWHDLC", "TUNNEL", "TUNNEL6", "FRAD",
        "SKIP", "LOOPBACK", "LOCALTLK", "FDDI",
        "BIF", "SIT", "IPDDP", "IPGRE",
        "PIMREG", "HIPPI", "ASH", "ECONET",
        "IRDA", "FCPP", "FCAL", "FCPL",
        "FCFABRIC", "IEEE802_TR", "IEEE80211",
        "IEEE80211_PRISM", "IEEE80211_RADIOTAP",
        "VOID", "NONE"
    };

    /**
     * This function is used to get links using nlLinkInfo
     * structure as filter.
     * @param nlLinkInfoList [out] links list (one or more entries).
     * @param nlLinkInfo [in] link info used as filter.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlGetLinks(struct nlLinkInfoList **nlLinkInfoList,
                   const struct nlLinkInfo *nlLinkInfo);

    /**
     * This function is used to ADD/DELETE/MODIFY a link.
     * @param nlLinkInfo [in] link info.
     * @param hType [in] message type (RTM_NEWLINK || RTM_DELLINK).
     * @param hFlags [in] message flags.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlModifyLink(const struct nlLinkInfo *nlLinkInfo, int hType,
                     unsigned int hFlags);

    /**
     * This function is used to add a link to specified links list.
     * @param rcvH [in] link message header.
     * @param nlLinkInfoList [out] list where link will be added.
     * @param nlLinkInfo [in] link info.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlAddLinkToList(const struct nlmsghdr *rcvH,
                        struct nlLinkInfoList **nlLinkInfoList,
                        const struct nlLinkInfo *nlLinkInfo);

    /**
     * Used to generate a filter that will be applied to a list of links.
     * @param nlLinkInfo [in] link info.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlGenLinkFilter(const struct nlLinkInfo *nlLinkInfo);

    /**
     * Used to reset link filter handler.
     */
    void nlResetLinkFilter();

    /**
     * Used to create a nlLinkInfo structure with default values.
     * @param nlLinkInfo [out] link structure.
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlCreateDefaultLinkInfo(struct nlLinkInfo *nlLinkInfo);

    /**
     * Used to free a link info list.
     * @param nlLinkInfoList [out] links list (one or more entries).
     * @return NL_OK=succesful | NL_FAIL=fail
     */
    int nlFreeLinkInfoList(struct nlLinkInfoList *nlLinkInfoList);

    /**
     * Function to convert a L2 address from network representation to
     * string representation (user friendly).
     * @param addr [in] L2 address to convert.
     * @param alen [in] L2 address length.
     * @param type [in] link type.
     * @param buf [out] converted address.
     * @param blen [in] converted address maximum length.
     */
    void nlAddr_n2a(const unsigned char *addr, int alen, int type,
                    char *buf, int blen);

    /**
     * Lookup the position of the passed type into linkType array.
     * @param devType [in] type to find.
     * @return position into linkType array.
     */
    unsigned short nlGetLinkTypePos(unsigned short devType);

    ////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_OSBASE_NETLINK_H_
