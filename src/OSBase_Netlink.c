/**
 * @file
 * OSBase_NetLink.c
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
 *              Based on iproute2 package:
 *              (http://www.linuxfoundation.org/en/Net:Iproute2)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#include "OSBase_Common.h"
#include "OSBase_Netlink.h"


////////////////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
/**
 * Netlink socket handler instance.
 */
struct nlSockHandle nlSH = { .fd = -1 };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlOpenSocket()
{
    _OSBASE_TRACE(3, ("--- nlOpenSocket() called"));

    socklen_t addrLen = 0;                    // Socket address length.
    int       sndbuf  = NL_SOCK_SND_BUFF_LEN; // Send buffer length.
    int       rcvbuf  = NL_SOCK_RCV_BUFF_LEN; // Receive buffer length.
    int       rc      = NL_OK;                // Result handler.

    // Create netlink endpoint.
    // AF_NETLINK    = Kernel user interface device.
    // SOCK_RAW      = Raw network protocol access.
    // NETLINK_ROUTE = Receives routing and link updates and may be used
    //                 to modify the routing tables (both IPv4 and IPv6).
    nlSH.fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if(nlSH.fd < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot open netlink socket. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Changes the send socket buffer size.
    if(setsockopt(nlSH.fd, SOL_SOCKET, SO_SNDBUF, &sndbuf,
                  sizeof(sndbuf)) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not change netlink send socket " \
                          "buffer size. %s", strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Changes the receive socket buffer size.
    if(setsockopt(nlSH.fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf,
                  sizeof(rcvbuf)) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not change netlink receive socket " \
                          "buffer size. %s", strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Setup local address and bind it to socket.
    // Address where we will receive messages from kernel.
    memset(&nlSH.local, 0, sizeof(nlSH.local));
    nlSH.local.nl_family = AF_NETLINK; // Netlink family.
    nlSH.local.nl_pid    = getpid();   // Destination local.
    nlSH.local.nl_groups = 0;          // Not multicast will be received.
    if(bind(nlSH.fd, (struct sockaddr*)&nlSH.local, sizeof(nlSH.local)) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot bind local address to netlink " \
                          "socket. %s", strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Ensure that everything is ok.
    // Get locally-bound name of the socket.
    addrLen = sizeof(nlSH.local);
    if(getsockname(nlSH.fd, (struct sockaddr*)&nlSH.local, &addrLen) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot get socket name. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Checking address length.
    if(addrLen != sizeof(nlSH.local)) {
        _OSBASE_TRACE(1, ("[ERROR] - Wrong address length %u.", addrLen));
        rc = NL_FAIL;
        goto out;
    }

    // Checking family.
    if(nlSH.local.nl_family != AF_NETLINK) {
        _OSBASE_TRACE(1, ("[ERROR] - Wrong address family %hu.",
                          nlSH.local.nl_family));
        rc = NL_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- nlOpenSocket() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlCloseSocket()
{
    _OSBASE_TRACE(3, ("--- nlCloseSocket() called"));

    int rc = NL_OK; // Result handler.

    // Close Netlink socket.
    if(nlSH.fd >= 0) {
        if(close(nlSH.fd) < 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Unable to close socket. %s",
                              strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }
        nlSH.fd = -1;
        memset(&nlSH.local, 0, sizeof(nlSH.local));
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Netlink socket has not been created " \
                          "previously."));
        rc = NL_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- nlCloseSocket() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlAddAttrToMsg(struct nlmsghdr *h, int maxlen, int attrType,
                   const void *attrData, int attrBytelen)
{
    _OSBASE_TRACE(3, ("--- nlAddAttrToMsg() called"));

    struct rtattr *rta = NULL;  // Attribute handler
    int            len = 0;     // Attribute length.
    int            rc  = NL_OK; // Result handler.

    // Check received parameters.
    if((h == NULL) || (attrData == NULL)) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters."));
        rc = NL_FAIL;
        goto out;
    }

    len = RTA_LENGTH(attrBytelen);
    if((NLMSG_ALIGN(h->nlmsg_len) + RTA_ALIGN(len)) > maxlen) {
        _OSBASE_TRACE(1, ("[ERROR] - Max allowed bound %d exceeded.",
                          maxlen));
        rc = NL_FAIL;
        goto out;
    }

    // Add attribute.
    rta = NLMSG_TAIL(h);
    rta->rta_type = attrType;
    rta->rta_len  = len;
    memcpy(RTA_DATA(rta), attrData, attrBytelen);
    h->nlmsg_len = NLMSG_ALIGN(h->nlmsg_len) + RTA_ALIGN(len);

 out:
    _OSBASE_TRACE(3, ("--- nlAddAttrToMsg() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlAddAttrToMsg32(struct nlmsghdr *h, int maxlen,
                     int attrType, unsigned int attrData)
{
    _OSBASE_TRACE(3, ("--- nlAddAttrToMsg32() called"));

    struct rtattr *rta = NULL;  // Attribute handler
    int            len = 0;     // Attribute length.
    int            rc  = NL_OK; // Result handler.

    // Check received parameters.
    if(h == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters."));
        rc = NL_FAIL;
        goto out;
    }

    len = RTA_LENGTH(4);
    if(NLMSG_ALIGN(h->nlmsg_len) + len > maxlen) {
        _OSBASE_TRACE(1, ("[ERROR] - Max allowed bound %d exceeded.", maxlen));
        rc = NL_FAIL;
        goto out;
    }

    // Add attribute.
    rta = NLMSG_TAIL(h);
    rta->rta_type = attrType;
    rta->rta_len = len;
    memcpy(RTA_DATA(rta), &attrData, 4);
    h->nlmsg_len = NLMSG_ALIGN(h->nlmsg_len) + len;

 out:
    _OSBASE_TRACE(3, ("--- nlAddAttrToMsg32() exited"));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ROUTES
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int nlGetRoutes(struct nlRouteInfoList **nlRtInfoList,
                struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- nlGetRoutes() called"));

    struct sockaddr_nl nlSockAddrKern;                  // Netlink client in
                                                        // kernel.
    struct iovec       iov;                             // Scatter-and-gather
                                                        // buffer.
    struct msghdr      msg;                             // Final message
                                                        // structure.
    char               rcvBuffer[NL_SOCK_SND_BUFF_LEN]; // Received message
                                                        // buffer.
    struct nlmsghdr   *rcvH   = NULL;                   // Received Netlink
                                                        // message header.
    struct nlmsgerr   *errmsg = NULL;                   // Netlink errors
                                                        // container.
    int                status = 0;                      // Send/Receive message
                                                        // status.
    int                rc     = NL_OK;                  // Result handler.

    // Buffer to hold the RTNETLINK request.
    struct {
        struct nlmsghdr h; // Netlink message header.
        struct rtmsg    m; // Message.
    } req;

    // Check received parameters.
    if(nlRtInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters."));
        rc = NL_FAIL;
        goto out;
    }

    // Generate filter.
    if(nlGenRouteFilter(nlRtInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Problem generating route filter."));
        rc = NL_FAIL;
        goto out;
    }

    // Fill request message.
    memset(&req, 0, sizeof(req));
    // Length of message including header.
    req.h.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    // Receive information about a network route.
    req.h.nlmsg_type = RTM_GETROUTE;
    // Return the complete table matching criteria passed in message content.
    req.h.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    // Sequence number (opaque for netlink core).
    req.h.nlmsg_seq = time(NULL);
    // PID of the sending process (opaque for netlink core).
    req.h.nlmsg_pid = getpid();
    // Address family of route.
    req.m.rtm_family = nlRtInfo->family;
    // Length of source.
    req.m.rtm_src_len = 0;
    // Length of destination.
    req.m.rtm_dst_len = 0;
    // TOS filter.
    req.m.rtm_tos = 0;
    // Routing table.
    req.m.rtm_table = RT_TABLE_UNSPEC;
    // Route origin.
    req.m.rtm_protocol = RTN_UNSPEC;
    // Distance to the destination.
    req.m.rtm_scope = RT_SCOPE_UNIVERSE;
    // Route type.
    req.m.rtm_type = RTA_UNSPEC;

    // Setup kernel address. Address to where we will send messages.
    memset(&nlSockAddrKern, 0, sizeof(nlSockAddrKern));
    nlSockAddrKern.nl_family = AF_NETLINK; // Netlink family.
    nlSockAddrKern.nl_pid    = 0;          // Destination kernel.
    nlSockAddrKern.nl_groups = 0;          // Not multicast will be received.

    // Init iovec structure.
    iov.iov_base = (void*)&(req.h);
    iov.iov_len  = req.h.nlmsg_len;

    // Init final message structure.
    msg.msg_name       = &nlSockAddrKern;        // Socket name.
    msg.msg_namelen    = sizeof(nlSockAddrKern); // Length of name.
    msg.msg_iov        = &iov;                   // Data blocks.
    msg.msg_iovlen     = 1;                      // Number of blocks.
    msg.msg_control    = NULL;                   // Ancillary data.
    msg.msg_controllen = 0;                      // Ancillary data buffer len.
    msg.msg_flags      = 0;                      // Flags on received message.

    // Send request message to kernel.
    if(sendmsg(nlSH.fd, &msg, 0) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot talk to rtnetlink. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Receive kernel messages (route table).
    memset(rcvBuffer, 0, NL_SOCK_SND_BUFF_LEN*sizeof(*rcvBuffer));
    iov.iov_base = rcvBuffer;
    while(1) {
        rcvH        = NULL; // Received Netlink message header.
        iov.iov_len = sizeof(rcvBuffer);
        status      = recvmsg(nlSH.fd, &msg, 0);

        // Check if received message is correct.
        if(status < 0) {
            // EINTR  = Was interrupted by a signal before any data was
            //          available.
            // EAGAIN = The socket's file descriptor is marked O_NONBLOCK
            //          and no data is waiting to be received.
            if((errno == EINTR) || (errno == EAGAIN)) {
                continue;
            }
            _OSBASE_TRACE(1, ("[ERROR] - Netlink receive error - %s (%d).",
                              strerror(errno), errno));
            rc = NL_FAIL;
            goto out;
        }
        if(status == 0) {
            _OSBASE_TRACE(1, ("[ERROR] - No messages are available to be "
                              "received. %s", strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }

        // Parse message.
        rcvH = (struct nlmsghdr*)rcvBuffer;
        while(NLMSG_OK(rcvH, status)) {
            // Check pid and seq.
            if((nlSockAddrKern.nl_pid != 0) ||
               (rcvH->nlmsg_pid != nlSH.local.nl_pid) ||
               (rcvH->nlmsg_seq != req.h.nlmsg_seq)) {
                rcvH = NLMSG_NEXT(rcvH, status);
                continue;
            }

            // If done, exit.
            if(rcvH->nlmsg_type == NLMSG_DONE) {
                goto out;
            }

            // Error received.
            if(rcvH->nlmsg_type == NLMSG_ERROR) {
                errmsg = (struct nlmsgerr*)NLMSG_DATA(rcvH);
                if(rcvH->nlmsg_len < NLMSG_LENGTH(sizeof(*errmsg))) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated."));
                } else {
                    _OSBASE_TRACE(1, ("[ERROR] - RTNETLINK answers."));
                }
                rc = NL_FAIL;
                goto out;
            }

            // We need to pass route info to filter routes because
            // NLM_F_MATCH is not implemented yet (in kernel), so we are
            // receiving all routes from all tables.
            if(nlAddRouteToList(rcvH, nlRtInfoList, nlRtInfo) == NL_FAIL) {
                rc = NL_FAIL;
                goto out;
            }

            // Next route.
            rcvH = NLMSG_NEXT(rcvH, status);
        }

        // MSG_TRUNC = Normal data truncated.
        if(msg.msg_flags & MSG_TRUNC) {
            _OSBASE_TRACE(2, ("[INFO] - Message truncated."));
            continue;
        }
        if(status) {
            _OSBASE_TRACE(1, ("[ERROR] - Remnant of size %d.", status));
            rc = NL_FAIL;
            goto out;
        }
    }

 out:
    if(rc != NL_OK) {
        nlFreeRouteInfoList(*nlRtInfoList);
    }

    _OSBASE_TRACE(3, ("--- nlGetRoutes() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlModifyRoute(struct nlRouteInfo *nlRtInfo, int hType,
                  unsigned int hFlags)
{
    _OSBASE_TRACE(3, ("--- nlModifyRoute() called"));

    struct sockaddr_nl nlSockAddrKern;                  // Netlink client in
                                                        // kernel.
    struct iovec       iov;                             // Scatter-and-gather
                                                        // buffer.
    struct msghdr      msg;                             // Final message
                                                        // structure.
    char               rcvBuffer[NL_SOCK_SND_BUFF_LEN]; // Received message
                                                        // buffer.
    struct nlmsghdr   *rcvH   = NULL;                   // Received Netlink
                                                        // message header.
    struct nlmsgerr   *errmsg = NULL;                   // Netlink errors
                                                        // container.
    uint32_t           attrData[8];                     // Attribute data.
    uint8_t            attrBytelen;                     // Attribute byte
                                                        // length.
    int                status = 0;                      // Send/Receive message
                                                        // status.
    int                rc     = NL_OK;                  // Result handler.

    // Buffer to hold the RTNETLINK request.
    struct {
        struct nlmsghdr h;         // Netlink message header.
        struct rtmsg    m;         // Message.
        char            buf[1024]; // Attributes buffer.
    } req;

    // Check received parameters.
    if(nlRtInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters."));
        rc = NL_FAIL;
        goto out;
    }

    // Check hType.
    if((hType != RTM_NEWROUTE) && (hType != RTM_DELROUTE)) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid message type."));
        rc = NL_FAIL;
        goto out;
    }

    // Generate filter.
    if(nlGenRouteFilter(nlRtInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Problem generating filter."));
        rc = NL_FAIL;
        goto out;
    }

    // Fill message.
    memset(&req, 0, sizeof(req));
    // Length of message including header.
    req.h.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    // Type of message content.
    req.h.nlmsg_type = hType;
    // Flags.
    req.h.nlmsg_flags = NLM_F_ACK | NLM_F_REQUEST | hFlags;
    // Sequence number (opaque for netlink core).
    req.h.nlmsg_seq = time(NULL);
    // PID of the sending process (opaque for netlink core).
    req.h.nlmsg_pid = getpid();
    // Address family of route.
    req.m.rtm_family = nlRtInfo->family;
    // Length of source.
    req.m.rtm_src_len = nlRtInfo->srcLen;
    // Length of destination.
    req.m.rtm_dst_len = nlRtInfo->dstLen;
    // TOS filter.
    req.m.rtm_tos = nlRtInfo->tos;
    // Routing table.
    req.m.rtm_table = nlRtInfo->table;
    // Route origin.
    req.m.rtm_protocol = nlRtInfo->protocol;
    // Distance to the destination.
    req.m.rtm_scope = nlRtInfo->scope;
    // Route type.
    req.m.rtm_type = nlRtInfo->type;

    // Destination address to be reached.
    if(rtFlt.dstAddr) {
        if(inet_pton(nlRtInfo->family, nlRtInfo->dstAddr, attrData,
                     ((nlRtInfo->family == AF_INET) ? (INET_ADDRSTRLEN) :
                      (INET6_ADDRSTRLEN))) <= 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                              "presentation format to numeric format. %s",
                              strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }
        (nlRtInfo->family == AF_INET) ? (attrBytelen = 4) : (attrBytelen = 16);
        nlAddAttrToMsg(&req.h, sizeof(req), RTA_DST, &attrData, attrBytelen);
    }

    // Source address.
    if(rtFlt.srcAddr) {
        if(inet_pton(nlRtInfo->family, nlRtInfo->srcAddr, attrData,
                     ((nlRtInfo->family == AF_INET) ? (INET_ADDRSTRLEN) :
                      (INET6_ADDRSTRLEN))) <= 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from "\
                              "presentation format to numeric format. %s",
                              strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }
        (nlRtInfo->family == AF_INET) ? (attrBytelen = 4) : (attrBytelen = 16);
        nlAddAttrToMsg(&req.h, sizeof(req), RTA_SRC, &attrData, attrBytelen);
    }

    // Input interface index.
    if(rtFlt.inputIf) {
        nlAddAttrToMsg32(&req.h, sizeof(req), RTA_IIF,
                         (uint32_t)(nlRtInfo->inputIf));
    }

    // Output interface index.
    if(rtFlt.outputIf) {
        nlAddAttrToMsg32(&req.h, sizeof(req), RTA_OIF,
                         (uint32_t)(nlRtInfo->outputIf));
    }

    // Gateway of the route.
    if(rtFlt.gw) {
        if(inet_pton(nlRtInfo->family, nlRtInfo->gw, attrData,
                     ((nlRtInfo->family == AF_INET) ? (INET_ADDRSTRLEN) :
                      (INET6_ADDRSTRLEN))) <= 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                              "presentation format to numeric format. %s",
                              strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }
        (nlRtInfo->family == AF_INET) ? (attrBytelen = 4) : (attrBytelen = 16);
        nlAddAttrToMsg(&req.h, sizeof(req), RTA_GATEWAY, &attrData,
                       attrBytelen);
    }

    // Priority of the route.
    if(rtFlt.priority) {
        nlAddAttrToMsg32(&req.h, sizeof(req), RTA_PRIORITY,
                         (uint32_t)nlRtInfo->priority);
    }

    // Prefered source.
    if(rtFlt.prefSrc) {
        if(inet_pton(nlRtInfo->family, nlRtInfo->prefSrc, attrData,
                     ((nlRtInfo->family == AF_INET) ? (INET_ADDRSTRLEN) :
                      (INET6_ADDRSTRLEN))) <= 0) {
            _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                              "presentation format to numeric format. %s",
                              strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }
        (nlRtInfo->family == AF_INET) ? (attrBytelen = 4) : (attrBytelen = 16);
        nlAddAttrToMsg(&req.h, sizeof(req), RTA_PREFSRC, &attrData,
                       attrBytelen);
    }

    // Setup kernel address. Address where we will send messages.
    memset(&nlSockAddrKern, 0, sizeof(nlSockAddrKern));
    nlSockAddrKern.nl_family = AF_NETLINK; // Netlink family.
    nlSockAddrKern.nl_pid    = 0;          // Destination kernel.
    nlSockAddrKern.nl_groups = 0;          // Not multicast will be received.

    // Init iovec structure.
    iov.iov_base = (void*)&(req.h);
    iov.iov_len  = req.h.nlmsg_len;

    // Init final message structure.
    msg.msg_name       = &nlSockAddrKern;        // Socket name.
    msg.msg_namelen    = sizeof(nlSockAddrKern); // Length of name.
    msg.msg_iov        = &iov;                   // Data blocks.
    msg.msg_iovlen     = 1;                      // Number of blocks.
    msg.msg_control    = NULL;                   // Ancillary data.
    msg.msg_controllen = 0;                      // Ancillary data buffer len.
    msg.msg_flags      = 0;                      // Flags on received message.

    // Send request message to kernel.
    if(sendmsg(nlSH.fd, &msg, 0) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot talk to rtnetlink. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Receive kernel messages.
    memset(rcvBuffer, 0, NL_SOCK_SND_BUFF_LEN*sizeof(*rcvBuffer));
    iov.iov_base = rcvBuffer;
    while(1) {
        rcvH = NULL; // Received Netlink message header.
        iov.iov_len = sizeof(rcvBuffer);
        status = recvmsg(nlSH.fd, &msg, 0);

        // Check if received message is correct.
        if(status < 0) {
            // EINTR  = Was interrupted by a signal before any data was
            //          available.
            // EAGAIN = The socket's file descriptor is marked O_NONBLOCK
            //          and no data is waiting to be received.
            if((errno == EINTR) || (errno == EAGAIN)) {
                continue;
            }
            _OSBASE_TRACE(1, ("[ERROR] - Netlink receive error - %s (%d).",
                              strerror(errno), errno));
            rc = NL_FAIL;
            goto out;
        }
        if(status == 0) {
            _OSBASE_TRACE(1, ("[ERROR] - No messages are available to be "
                              "received. %s", strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }

        // Check sender address.
        if(msg.msg_namelen != sizeof(nlSockAddrKern)) {
            _OSBASE_TRACE(1, ("[ERROR] - Wrong sender address length == %d.",
                              msg.msg_namelen));
            rc = NL_FAIL;
            goto out;
        }

        // Parse receive message.
        for(rcvH=(struct nlmsghdr*)rcvBuffer ; status>=sizeof(*rcvH) ; ) {
            int len = rcvH->nlmsg_len;
            int l = len - sizeof(*rcvH);

            // Check message length.
            if((l<0) || (len>status)) {
                // MSG_TRUNC = Normal data truncated.
                if(msg.msg_flags & MSG_TRUNC) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated message."));
                    rc = NL_FAIL;
                    goto out;
                }
                _OSBASE_TRACE(1, ("[ERROR] - Malformed message: len=%d.", len));
                rc = NL_FAIL;
                goto out;
            }

            // Check pid and seq.
            if((nlSockAddrKern.nl_pid != 0) ||
               (rcvH->nlmsg_pid != req.h.nlmsg_pid) ||
               (rcvH->nlmsg_seq != req.h.nlmsg_seq)) {
                // Don't forget to skip that message.
                status -= NLMSG_ALIGN(len);
                rcvH = (struct nlmsghdr*)((char*)rcvH + NLMSG_ALIGN(len));
                continue;
            }

            // Error / ACK received.
            if(rcvH->nlmsg_type == NLMSG_ERROR) {
                errmsg = (struct nlmsgerr*)NLMSG_DATA(rcvH);
                if(l < sizeof(struct nlmsgerr)) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated message."));
                    rc = NL_FAIL;
                    goto out;
                } else { // ACK
                    errno = -errmsg->error;
                    goto out;
                }
            }

            _OSBASE_TRACE(1, ("[ERROR] - Unexpected reply!!!"));
            status -= NLMSG_ALIGN(len);
            rcvH = (struct nlmsghdr*)((char*)rcvH + NLMSG_ALIGN(len));
        }

        // MSG_TRUNC = Normal data truncated.
        if(msg.msg_flags & MSG_TRUNC) {
            _OSBASE_TRACE(2, ("[INFO] - Message truncated."));
            continue;
        }
        if(status) {
            _OSBASE_TRACE(1, ("[ERROR] - Remnant of size %d.", status));
            rc = NL_FAIL;
            goto out;
        }
    }

 out:
    _OSBASE_TRACE(3, ("--- nlModifyRoute() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlAddRouteToList(const struct nlmsghdr *rcvH,
                     struct nlRouteInfoList **nlRtInfoList,
                     const struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- nlAddRouteToList() called"));

    struct nlRouteInfoList *nlRtInfoListCurr = NULL;  // Current routes list.
    struct rtmsg           *rMsg             = NULL;  // Message content.
    struct rtattr          *rAttr            = NULL;  // Message attributes.
    int                     rAttrCount       = 0;     // Number of message
                                                      // attributes.
    int                     rc               = NL_OK; // Result handler.

    // Check netlink message header.
    if(rcvH == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid netlink message."));
        rc = NL_FAIL;
        goto out;
    }

    // Check Route filter info.
    if(nlRtInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid route filter info."));
        rc = NL_FAIL;
        goto out;
    }

    // Get message content.
    rMsg = (struct rtmsg*)NLMSG_DATA(rcvH);
    if(rMsg == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get message content."));
        rc = NL_FAIL;
        goto out;
    }

    // Create routes list entry.
    nlRtInfoListCurr = malloc(sizeof(*nlRtInfoListCurr));
    if(nlRtInfoListCurr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for "       \
                          "\'nlRtInfoListCurr\'."));
        rc = NL_FAIL;
        goto out;
    }
    memset(nlRtInfoListCurr, 0, sizeof(*nlRtInfoListCurr));

    // Alloc first entry.
    nlRtInfoListCurr->sptr = malloc(sizeof(*(nlRtInfoListCurr->sptr)));
    if(nlRtInfoListCurr->sptr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for "       \
                          "\'nlRtInfoListCurr->sptr\'."));
        rc = NL_FAIL;
        goto out;
    }
    memset(nlRtInfoListCurr->sptr, 0, sizeof(*(nlRtInfoListCurr->sptr)));

    // Route family.
    if((rtFlt.family) && (rMsg->rtm_family != nlRtInfo->family)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Route family\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->family = rMsg->rtm_family;

    // Route type.
    if((rtFlt.type) && (rMsg->rtm_type != nlRtInfo->type)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Route type\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->type = rMsg->rtm_type;

    // Route origin.
    if((rtFlt.protocol) && (rMsg->rtm_protocol != nlRtInfo->protocol)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Route origin\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->protocol = rMsg->rtm_protocol;

    // Distance to the destination.
    if((rtFlt.scope) && (rMsg->rtm_scope != nlRtInfo->scope)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Distance to the destination\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->scope = rMsg->rtm_scope;

    // Source Route mask prefix.
    if((rtFlt.srcLen) && (rMsg->rtm_src_len != nlRtInfo->srcLen)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Source Route mask prefix\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->srcLen = rMsg->rtm_src_len;

    // Destination Route mask prefix.
    if((rtFlt.dstLen) && (rMsg->rtm_dst_len != nlRtInfo->dstLen)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Destination Route mask prefix\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->dstLen = rMsg->rtm_dst_len;

    // Type of service.
    if((rtFlt.tos) && (rMsg->rtm_tos != nlRtInfo->tos)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Type of service\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlRtInfoListCurr->sptr->tos = rMsg->rtm_tos;

    // Parse attributes.
    rAttr = (struct rtattr*)RTM_RTA(rMsg);
    if(rAttr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not retrieve attributes."));
        rc = NL_FAIL;
        goto out;
    }
    rAttrCount = RTM_PAYLOAD(rcvH);
    for( ; RTA_OK(rAttr, rAttrCount) ; rAttr=RTA_NEXT(rAttr, rAttrCount)) {
        switch(rAttr->rta_type) {
        case RTA_DST: // Destination address to be reached.
            if(inet_ntop(rMsg->rtm_family, RTA_DATA(rAttr),
                         nlRtInfoListCurr->sptr->dstAddr,
                         ((rMsg->rtm_family == AF_INET) ? (INET_ADDRSTRLEN) :
                          (INET6_ADDRSTRLEN))) <= 0) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                                  "numeric format to presentation format."));
                rc = NL_SKIP;
                goto out;
            }
            if((rtFlt.dstAddr) && (strcmp(nlRtInfoListCurr->sptr->dstAddr,
                                          nlRtInfo->dstAddr) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Destination address to be " \
                                  "reached\'."));
                rc = NL_SKIP;
                goto out;
            }
            break;
        case RTA_SRC: // Source address.
            if(inet_ntop(rMsg->rtm_family, RTA_DATA(rAttr),
                         nlRtInfoListCurr->sptr->srcAddr,
                         ((rMsg->rtm_family == AF_INET) ? (INET_ADDRSTRLEN) :
                          (INET6_ADDRSTRLEN))) <= 0) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                                  "numeric format to presentation format."));
                rc = NL_SKIP;
                goto out;
            }
            if((rtFlt.srcAddr) && (strcmp(nlRtInfoListCurr->sptr->srcAddr,
                                          nlRtInfo->srcAddr) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Source address\'."));
                rc = NL_SKIP;
                goto out;
            }
            break;
        case RTA_IIF: // Input interface index.
            if((rtFlt.inputIf) &&
               ((*((int*)RTA_DATA(rAttr))) != nlRtInfo->inputIf)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Input interface index\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlRtInfoListCurr->sptr->inputIf = *((int*)RTA_DATA(rAttr));
            break;
        case RTA_OIF: // Output interface index.
            if((rtFlt.outputIf) &&
               ((*((int*)RTA_DATA(rAttr))) != nlRtInfo->outputIf)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Output interface index\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlRtInfoListCurr->sptr->outputIf = *((int*)RTA_DATA(rAttr));
            break;
        case RTA_GATEWAY: // Gateway of the route.
            if(inet_ntop(rMsg->rtm_family, RTA_DATA(rAttr),
                         nlRtInfoListCurr->sptr->gw,
                         ((rMsg->rtm_family == AF_INET) ? (INET_ADDRSTRLEN) :
                          (INET6_ADDRSTRLEN))) <= 0) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                                  "numeric format to presentation format."));
                rc = NL_SKIP;
                goto out;
            }
            if((rtFlt.gw) && (strcmp(nlRtInfoListCurr->sptr->gw,
                                     nlRtInfo->gw) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Gateway of the route\'."));
                rc = NL_SKIP;
                goto out;
            }
            break;
        case RTA_PRIORITY: // Priority of the route.
            if((rtFlt.priority) &&
               ((*((int*)RTA_DATA(rAttr))) != nlRtInfo->priority)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Priority of the route\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlRtInfoListCurr->sptr->priority = *((int*)RTA_DATA(rAttr));
            break;
        case RTA_PREFSRC: // Prefered source.
            if(inet_ntop(rMsg->rtm_family, RTA_DATA(rAttr),
                         nlRtInfoListCurr->sptr->prefSrc,
                         ((rMsg->rtm_family == AF_INET) ? (INET_ADDRSTRLEN) :
                          (INET6_ADDRSTRLEN))) <= 0) {
                _OSBASE_TRACE(1, ("[ERROR] - Could not convert address from " \
                                  "numeric format to presentation format."));
                rc = NL_SKIP;
                goto out;
            }
            if((rtFlt.prefSrc) && (strcmp(nlRtInfoListCurr->sptr->prefSrc,
                                          nlRtInfo->prefSrc) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Prefered source\'."));
                rc = NL_SKIP;
                goto out;
            }
            break;
        case RTA_METRICS: // Route metrics.
            if((rtFlt.metrics) &&
               ((*((int*)RTA_DATA(rAttr))) != nlRtInfo->metrics)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Route metrics\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlRtInfoListCurr->sptr->metrics = *((int*)RTA_DATA(rAttr));
            break;
        case RTA_TABLE: // Route table.
            if((rtFlt.table) &&
               ((*((int*)RTA_DATA(rAttr))) != nlRtInfo->table)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Route table\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlRtInfoListCurr->sptr->table = *((int*)RTA_DATA(rAttr));
            break;
        default:
            break;
        }
    }

    // Set next route.
    nlRtInfoListCurr->next = *nlRtInfoList;
    *nlRtInfoList = nlRtInfoListCurr;

 out:
    if(rc != NL_OK) {
        nlFreeRouteInfoList(nlRtInfoListCurr);
    }

    _OSBASE_TRACE(3, ("--- nlAddRouteToList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlGenRouteFilter(struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- nlGenRouteFilter() called"));

    char dstAddrLoc[NL_ADDRESS_MAXLEN]; // Destination address tmp container.
    char srcAddrLoc[NL_ADDRESS_MAXLEN]; // Source address tmp container.
    char gwLoc[NL_ADDRESS_MAXLEN];      // Gateway address tmp container.
    char prefSrcLoc[NL_ADDRESS_MAXLEN]; // Preferred source address tmp
                                        // container.
    int  newFamily;                     // Used in family address check
                                        // process.
    int  rc = NL_OK;                    // Result handler.

    // Reset filter.
    nlResetRouteFilter();

    // Address family.
    if((nlRtInfo->family == AF_INET) || (nlRtInfo->family == AF_INET6)) {
        rtFlt.family = 1;
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid family: %d.", nlRtInfo->family));
        rc = NL_FAIL;
        goto out;
    }

    // Route type.
    if((nlRtInfo->type > RTN_UNSPEC) && (nlRtInfo->type < __RTN_MAX)) {
        rtFlt.type = 1;
    } else if(nlRtInfo->type < RTN_UNSPEC) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid type: %d.", nlRtInfo->type));
        rc = NL_FAIL;
        goto out;
    }

    // Route origin.
    if((nlRtInfo->protocol > RTPROT_UNSPEC) &&
       (nlRtInfo->protocol <= RTPROT_NTK)) {
        rtFlt.protocol = 1;
    } else if(nlRtInfo->protocol < RTPROT_UNSPEC) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid protocol: %d.",
                          nlRtInfo->protocol));
        rc = NL_FAIL;
        goto out;
    }

    // Distance to the destination.
    if((nlRtInfo->scope > RT_SCOPE_UNIVERSE) &&
       (nlRtInfo->scope <= RT_SCOPE_NOWHERE)) {
        rtFlt.scope = 1;
    } else if(nlRtInfo->scope < RT_SCOPE_UNIVERSE) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid scope: %d.", nlRtInfo->scope));
        rc = NL_FAIL;
        goto out;
    }

    // Source Route mask prefix.
    if(nlRtInfo->srcLen > 0) {
        rtFlt.srcLen = 1;
    } else if(nlRtInfo->srcLen < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid srcLen: %d.", nlRtInfo->srcLen));
        rc = NL_FAIL;
        goto out;
    }

    // Destination Route mask prefix.
    if(nlRtInfo->dstLen > 0) {
        rtFlt.dstLen = 1;
    } else if(nlRtInfo->dstLen < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid dstLen: %d.", nlRtInfo->dstLen));
        rc = NL_FAIL;
        goto out;
    }

    // Type of service.
    if(nlRtInfo->tos > 0) {
        rtFlt.tos = 1;
    } else if(nlRtInfo->tos < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid tos: %d.", nlRtInfo->tos));
        rc = NL_FAIL;
        goto out;
    }

    // Destination address to be reached.
    if(strlen(nlRtInfo->dstAddr) > 0) {
        memset(dstAddrLoc, 0, (NL_ADDRESS_MAXLEN)*sizeof(*dstAddrLoc));
        if(inet_pton(nlRtInfo->family, nlRtInfo->dstAddr, dstAddrLoc) > 0) {
            rtFlt.dstAddr = 1;
        } else {
            // Trying the other one.
            newFamily = (nlRtInfo->family==AF_INET)?(AF_INET6):(AF_INET);
            if(inet_pton(newFamily, nlRtInfo->dstAddr, dstAddrLoc) > 0) {
                rtFlt.dstAddr = 1;

                // Fixing wrong route family.
                nlRtInfo->family = newFamily;
            } else {
                _OSBASE_TRACE(1, ("[ERROR] - Invalid dstAddr: %s.",
                                  nlRtInfo->dstAddr));
                rc = NL_FAIL;
                goto out;
            }
        }
    }

    // Source address.
    if(strlen(nlRtInfo->srcAddr) > 0) {
        memset(srcAddrLoc, 0, (NL_ADDRESS_MAXLEN)*sizeof(*srcAddrLoc));
        if(inet_pton(nlRtInfo->family, nlRtInfo->srcAddr, srcAddrLoc) > 0) {
            rtFlt.srcAddr = 1;
        } else {
            // Trying the other one.
            newFamily = (nlRtInfo->family==AF_INET)?(AF_INET6):(AF_INET);
            if(inet_pton(newFamily, nlRtInfo->srcAddr, srcAddrLoc) > 0) {
                rtFlt.srcAddr = 1;

                // Fixing wrong route family.
                nlRtInfo->family = newFamily;
            } else {
                _OSBASE_TRACE(1, ("[ERROR] - Invalid srcAddr: %s.",
                                  nlRtInfo->srcAddr));
                rc = NL_FAIL;
                goto out;
            }
        }
    }

    // Input interface index.
    if(nlRtInfo->inputIf >= 0) {
        rtFlt.inputIf = 1;
    }

    // Output interface index.
    if(nlRtInfo->outputIf >= 0) {
        rtFlt.outputIf = 1;
    }

    // Gateway of the route.
    if(strlen(nlRtInfo->gw) > 0) {
        memset(gwLoc, 0, (NL_ADDRESS_MAXLEN)*sizeof(*gwLoc));
        if(inet_pton(nlRtInfo->family, nlRtInfo->gw, gwLoc) > 0) {
            rtFlt.gw = 1;
        } else {
            // Trying the other one.
            newFamily = (nlRtInfo->family==AF_INET)?(AF_INET6):(AF_INET);
            if(inet_pton(newFamily, nlRtInfo->gw, gwLoc) > 0) {
                rtFlt.gw = 1;

                // Fixing wrong route family.
                nlRtInfo->family = newFamily;
            } else {
                _OSBASE_TRACE(1, ("[ERROR] - Invalid gw: %s.", nlRtInfo->gw));
                rc = NL_FAIL;
                goto out;
            }
        }
    }

    // Priority of the route.
    if(nlRtInfo->priority >= 0) {
        rtFlt.priority = 1;
    }

    // Prefered source.
    if(strlen(nlRtInfo->prefSrc) > 0) {
        memset(prefSrcLoc, 0, (NL_ADDRESS_MAXLEN)*sizeof(*prefSrcLoc));
        if(inet_pton(nlRtInfo->family, nlRtInfo->prefSrc, prefSrcLoc) > 0) {
            rtFlt.prefSrc = NL_FAIL;
        } else {
            // Trying the other one.
            newFamily = (nlRtInfo->family==AF_INET)?(AF_INET6):(AF_INET);
            if(inet_pton(newFamily, nlRtInfo->prefSrc, prefSrcLoc) > 0) {
                rtFlt.prefSrc = NL_FAIL;

                // Fixing wrong route family.
                nlRtInfo->family = newFamily;
            } else {
                _OSBASE_TRACE(1, ("[ERROR] - Invalid prefered source: %s.",
                                  nlRtInfo->prefSrc));
                rc = NL_FAIL;
                goto out;
            }
        }
    }

    // Route metrics.
    if(nlRtInfo->metrics >= 0) {
        rtFlt.metrics = 1;
    }

    // Route table.
    if((nlRtInfo->table > RT_TABLE_UNSPEC) &&
       (nlRtInfo->table <= RT_TABLE_MAX)) {
        rtFlt.table = 1;
    } else if(nlRtInfo->table < RT_TABLE_UNSPEC) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid table: %d.", nlRtInfo->table));
        rc = NL_FAIL;
        goto out;
    }

 out:
    _OSBASE_TRACE(3, ("--- nlGenRouteFilter() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void nlResetRouteFilter()
{
    _OSBASE_TRACE(3, ("--- nlResetRouteFilter() called"));

    rtFlt.family   = 0;
    rtFlt.type     = 0;
    rtFlt.protocol = 0;
    rtFlt.scope    = 0;
    rtFlt.srcLen   = 0;
    rtFlt.dstLen   = 0;
    rtFlt.tos      = 0;
    rtFlt.dstAddr  = 0;
    rtFlt.srcAddr  = 0;
    rtFlt.inputIf  = 0;
    rtFlt.outputIf = 0;
    rtFlt.gw       = 0;
    rtFlt.priority = 0;
    rtFlt.prefSrc  = 0;
    rtFlt.metrics  = 0;
    rtFlt.table    = 0;

    _OSBASE_TRACE(3, ("--- nlResetRouteFilter() exited"));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlCreateDefaultRtInfo(struct nlRouteInfo *nlRtInfo)
{
    _OSBASE_TRACE(3, ("--- nlCreateDefaultRtInfo() called"));

    int rc = NL_OK; // Result handler.

    // Check received parameters.
    if(nlRtInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid link filter struct."));
        rc = NL_FAIL;
        goto out;
    }

    nlRtInfo->family   = AF_INET;
    nlRtInfo->type     = RTN_UNSPEC;
    nlRtInfo->protocol = RTPROT_UNSPEC;
    nlRtInfo->scope    = RT_SCOPE_UNIVERSE;
    nlRtInfo->srcLen   = 0;
    nlRtInfo->dstLen   = 0;
    nlRtInfo->tos      = 0;
    snprintf(nlRtInfo->dstAddr, NL_ADDRESS_MAXLEN, "%s", "");
    snprintf(nlRtInfo->srcAddr, NL_ADDRESS_MAXLEN, "%s", "");
    nlRtInfo->inputIf  = -1;
    nlRtInfo->outputIf = -1;
    snprintf(nlRtInfo->gw, NL_ADDRESS_MAXLEN, "%s", "");
    nlRtInfo->priority = -1;
    snprintf(nlRtInfo->prefSrc, NL_ADDRESS_MAXLEN, "%s", "");
    nlRtInfo->metrics  = -1;
    nlRtInfo->table    = RT_TABLE_UNSPEC;

 out:
    _OSBASE_TRACE(3, ("--- nlCreateDefaultRtInfo() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlFreeRouteInfoList(struct nlRouteInfoList *nlRtInfoList)
{
    _OSBASE_TRACE(3, ("--- nlFreeRouteInfoList() called"));

    struct nlRouteInfoList* nlRtInfoListTmp = NULL;  // Temporary list
                                                     // container.
    int                    rc               = NL_OK; // Result handler.

    // Check received parameters.
    if(nlRtInfoList == NULL) {
        _OSBASE_TRACE(2, ("[INFO] - NULL route info list, nothing to free."));
        goto out;
    }

    // Clean.
    while(nlRtInfoList != NULL) {
        FREE_SAFE(nlRtInfoList->sptr);
        nlRtInfoListTmp = nlRtInfoList;
        nlRtInfoList = nlRtInfoList->next;
        FREE_SAFE(nlRtInfoListTmp);
    }

 out:
    _OSBASE_TRACE(3, ("--- nlFreeRouteInfoList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// LINKS
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int nlGetLinks(struct nlLinkInfoList **nlLinkInfoList,
               const struct nlLinkInfo *nlLinkInfo)
{
    _OSBASE_TRACE(3, ("--- nlGetLinks() called"));

    struct sockaddr_nl nlSockAddrKern;                  // Netlink client in
                                                        // kernel.
    struct iovec       iov;                             // Scatter-and-gather
                                                        // buffer.
    struct msghdr      msg;                             // Final message
                                                        // structure.
    char               rcvBuffer[NL_SOCK_SND_BUFF_LEN]; // Received message
                                                        // buffer.
    struct nlmsghdr   *rcvH   = NULL;                   // Received Netlink
                                                        // message header.
    struct nlmsgerr   *errmsg = NULL;                   // Netlink errors
                                                        // container.
    int                status = 0;                      // Send/Receive message
                                                        // status.
    int                rc     = NL_OK;                  // Result handler.

    // Buffer to hold the RTNETLINK request.
    struct {
        struct nlmsghdr  h; // Netlink message header.
        struct ifinfomsg m; // Message.
    } req;

    // Check received parameters.
    if(nlLinkInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters (filter)."));
        rc = NL_FAIL;
        goto out;
    }

    // Generate link filter.
    if(nlGenLinkFilter(nlLinkInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Problem generating link filter."));
        rc = NL_FAIL;
        goto out;
    }

    // Fill request message.
    memset(&req, 0, sizeof(req));
    // Length of message including header.
    req.h.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    // Receive information about a link.
    req.h.nlmsg_type = RTM_GETLINK;
    // Return the complete table matching criteria passed in message content.
    req.h.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    // Sequence number (opaque for netlink core).
    req.h.nlmsg_seq = time(NULL);
    // PID of the sending process (opaque for netlink core).
    req.h.nlmsg_pid = getpid();
    // Always AF_UNSPEC.
    req.m.ifi_family = AF_UNSPEC;
    // Link type - ARPHRD_*.
    req.m.ifi_type = ARPHRD_VOID;
    // Unique interface index.
    req.m.ifi_index = 0;
    // Device flags (see netdevice(7)).
    req.m.ifi_flags = 0;
    // Reserved for future use.
    req.m.ifi_change = 0xFFFFFFFF;

    // Setup kernel address. Address where we will send messages.
    memset(&nlSockAddrKern, 0, sizeof(nlSockAddrKern));
    nlSockAddrKern.nl_family = AF_NETLINK; // Netlink family.
    nlSockAddrKern.nl_pid    = 0;          // Destination kernel.
    nlSockAddrKern.nl_groups = 0;          // Not multicast will be received.

    // Init iovec structure.
    iov.iov_base = (void*)&(req.h);
    iov.iov_len  = req.h.nlmsg_len;

    // Init final message structure.
    msg.msg_name       = &nlSockAddrKern;        // Socket name.
    msg.msg_namelen    = sizeof(nlSockAddrKern); // Length of name.
    msg.msg_iov        = &iov;                   // Data blocks.
    msg.msg_iovlen     = 1;                      // Number of blocks.
    msg.msg_control    = NULL;                   // Ancillary data.
    msg.msg_controllen = 0;                      // Ancillary data buffer len.
    msg.msg_flags      = 0;                      // Flags on received message.

    // Send request message to kernel.
    if(sendmsg(nlSH.fd, &msg, 0) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot talk to rtnetlink. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Receive kernel messages (link).
    memset(rcvBuffer, 0, NL_SOCK_SND_BUFF_LEN*sizeof(*rcvBuffer));
    iov.iov_base = rcvBuffer;
    while(1) {
        rcvH        = NULL; // Received Netlink message header.
        iov.iov_len = sizeof(rcvBuffer);
        status      = recvmsg(nlSH.fd, &msg, 0);

        // Check if received message is correct.
        if(status < 0) {
            // EINTR  = Was interrupted by a signal before any data was
            //          available.
            // EAGAIN = The socket's file descriptor is marked O_NONBLOCK
            //          and no data is waiting to be received.
            if(errno == EINTR || errno == EAGAIN) {
                continue;
            }
            _OSBASE_TRACE(1, ("[ERROR] - Netlink receive error - %s (%d).",
                              strerror(errno), errno));
            rc = NL_FAIL;
            goto out;
        }
        if(status == 0) {
            _OSBASE_TRACE(1, ("[ERROR] - No messages are available to be "
                              "received. %s", strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }

        // Parse message.
        rcvH = (struct nlmsghdr*)rcvBuffer;
        while(NLMSG_OK(rcvH, status)) {
            // Check pid and seq.
            if((nlSockAddrKern.nl_pid != 0) ||
               (rcvH->nlmsg_pid != nlSH.local.nl_pid) ||
               (rcvH->nlmsg_seq != req.h.nlmsg_seq)) {
                rcvH = NLMSG_NEXT(rcvH, status);
                continue;
            }

            // If done, exit.
            if(rcvH->nlmsg_type == NLMSG_DONE) {
                goto out;
            }

            // Error received.
            if(rcvH->nlmsg_type == NLMSG_ERROR) {
                errmsg = (struct nlmsgerr*)NLMSG_DATA(rcvH);
                if(rcvH->nlmsg_len < NLMSG_LENGTH(sizeof(*errmsg))) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated."));
                } else {
                    _OSBASE_TRACE(1, ("[ERROR] - RTNETLINK answers."));
                }
                rc = NL_FAIL;
                goto out;
            }

            // We need to pass link info to filter links because
            // NLM_F_MATCH is not implemented yet (in kernel), so we are
            // receiving all links.
            if(nlAddLinkToList(rcvH, nlLinkInfoList, nlLinkInfo) == NL_FAIL) {
                rc = NL_FAIL;
                goto out;
            }

            // Next link.
            rcvH = NLMSG_NEXT(rcvH, status);
        }

        // MSG_TRUNC = Normal data truncated.
        if(msg.msg_flags & MSG_TRUNC) {
            _OSBASE_TRACE(2, ("[INFO] - Message truncated."));
            continue;
        }
        if(status) {
            _OSBASE_TRACE(1, ("[ERROR] - Remnant of size %d.", status));
            rc = NL_FAIL;
            goto out;
        }
    }

 out:
    if(rc != NL_OK) {
        nlFreeLinkInfoList(*nlLinkInfoList);
    }

    _OSBASE_TRACE(3, ("--- nlGetLink() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlModifyLink(const struct nlLinkInfo *nlLinkInfo, int hType,
                 unsigned int hFlags)
{
    _OSBASE_TRACE(3, ("--- nlModifyLink() called"));

    struct sockaddr_nl nlSockAddrKern;                  // Netlink client in
                                                        // kernel.
    struct iovec       iov;                             // Scatter-and-gather
                                                        // buffer.
    struct msghdr      msg;                             // Final message
                                                        // structure.
    char               rcvBuffer[NL_SOCK_SND_BUFF_LEN]; // Received message
                                                        // buffer.
    struct nlmsghdr   *rcvH   = NULL;                   // Received Netlink
                                                        // message header.
    struct nlmsgerr   *errmsg = NULL;                   // Netlink errors
                                                        // container.
    uint8_t            attrBytelen;                     // Attribute byte
                                                        // length.
    int                status = 0;                      // Send/Receive message
                                                        // status.
    int                rc     = NL_OK;                  // Result handler.

    // Buffer to hold the RTNETLINK request.
    struct {
        struct nlmsghdr  h;         // Netlink message header.
        struct ifinfomsg m;         // Message.
        char             buf[1024]; // Attributes buffer.
    } req;

    // Check received parameters.
    if(nlLinkInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid received parameters."));
        rc = NL_FAIL;
        goto out;
    }

    // Check hType.
    if((hType != RTM_NEWLINK) && (hType != RTM_DELLINK)) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid message type."));
        rc = NL_FAIL;
        goto out;
    }

    // Generate Link filter.
    if(nlGenLinkFilter(nlLinkInfo) != NL_OK) {
        _OSBASE_TRACE(1, ("[ERROR] - Problem generating filter."));
        rc = NL_FAIL;
        goto out;
    }

    // Fill request message.
    memset(&req, 0, sizeof(req));
    // Length of message including header.
    req.h.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    // Type of message content.
    req.h.nlmsg_type = hType;
    // Flags.
    req.h.nlmsg_flags = NLM_F_ACK | NLM_F_REQUEST | hFlags;
    // Sequence number (opaque for netlink core).
    req.h.nlmsg_seq = time(NULL);
    // PID of the sending process (opaque for netlink core).
    req.h.nlmsg_pid = getpid();
    // Always AF_UNSPEC.
    req.m.ifi_family = nlLinkInfo->family;
    // Link type - ARPHRD_*.
    req.m.ifi_type = nlLinkInfo->type;
    // Unique interface index.
    req.m.ifi_index = nlLinkInfo->index;
    // Device flags (see netdevice(7)).
    req.m.ifi_flags = nlLinkInfo->flags;
    // Reserved for future use.
    req.m.ifi_change = nlLinkInfo->change;

    // IFLA_IFNAME.
    if(linkFlt.ifname) {
        attrBytelen = strlen(nlLinkInfo->ifname) + 1;
        if(attrBytelen == 1) {
            _OSBASE_TRACE(1, ("[ERROR] - Not a valid device identifier."));
            rc = NL_FAIL;
            goto out;
        }
        if(attrBytelen > IFNAMSIZ) {
            _OSBASE_TRACE(1, ("[ERROR] - Device identifier too long."));
            rc = NL_FAIL;
            goto out;
        }
        nlAddAttrToMsg(&req.h, sizeof(req), IFLA_IFNAME,
                       nlLinkInfo->ifname, attrBytelen);
    }

    // IFLA_OPERSTATE.
    if(linkFlt.operstate) {
        nlAddAttrToMsg32(&req.h, sizeof(req), IFLA_OPERSTATE,
                         (uint32_t)(nlLinkInfo->operstate));
    }

    // Setup kernel address. Address where we will send messages.
    memset(&nlSockAddrKern, 0, sizeof(nlSockAddrKern));
    nlSockAddrKern.nl_family = AF_NETLINK; // Netlink family.
    nlSockAddrKern.nl_pid    = 0;          // Destination kernel.
    nlSockAddrKern.nl_groups = 0;          // Not multicast will be received.

    // Init iovec structure.
    iov.iov_base = (void*)&(req.h);
    iov.iov_len  = req.h.nlmsg_len;

    // Init final message structure.
    msg.msg_name       = &nlSockAddrKern;        // Socket name.
    msg.msg_namelen    = sizeof(nlSockAddrKern); // Length of name.
    msg.msg_iov        = &iov;                   // Data blocks.
    msg.msg_iovlen     = 1;                      // Number of blocks.
    msg.msg_control    = NULL;                   // Ancillary data.
    msg.msg_controllen = 0;                      // Ancillary data buffer len.
    msg.msg_flags      = 0;                      // Flags on received message.

    // Send request message to kernel.
    if(sendmsg(nlSH.fd, &msg, 0) < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Cannot talk to rtnetlink. %s",
                          strerror(errno)));
        rc = NL_FAIL;
        goto out;
    }

    // Receive kernel messages.
    memset(rcvBuffer, 0, NL_SOCK_SND_BUFF_LEN*sizeof(*rcvBuffer));
    iov.iov_base = rcvBuffer;
    while(1) {
        rcvH = NULL; // Received Netlink message header.
        iov.iov_len = sizeof(rcvBuffer);
        status = recvmsg(nlSH.fd, &msg, 0);

        // Check if received message is correct.
        if(status < 0) {
            // EINTR  = Was interrupted by a signal before any data was
            //          available.
            // EAGAIN = The socket's file descriptor is marked O_NONBLOCK
            //          and no data is waiting to be received.
            if(errno == EINTR || errno == EAGAIN) {
                continue;
            }
            _OSBASE_TRACE(1, ("[ERROR] - Netlink receive error - %s (%d).",
                              strerror(errno), errno));
            rc = NL_FAIL;
            goto out;
        }
        if(status == 0) {
            _OSBASE_TRACE(1, ("[ERROR] - No messages are available to be "
                              "received. %s", strerror(errno)));
            rc = NL_FAIL;
            goto out;
        }

        // Check sender address.
        if(msg.msg_namelen != sizeof(nlSockAddrKern)) {
            _OSBASE_TRACE(1, ("[ERROR] - Wrong sender address length == %d.",
                              msg.msg_namelen));
            rc = NL_FAIL;
            goto out;
        }

        // Parse receive message.
        for(rcvH=(struct nlmsghdr*)rcvBuffer ; status>=sizeof(*rcvH) ; ) {
            int len = rcvH->nlmsg_len;
            int l = len - sizeof(*rcvH);

            // Check message length.
            if((l<0) || (len>status)) {
                // MSG_TRUNC = Normal data truncated.
                if(msg.msg_flags & MSG_TRUNC) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated message."));
                    rc = NL_FAIL;
                    goto out;
                }
                _OSBASE_TRACE(1, ("[ERROR] - Malformed message: len=%d.", len));
                rc = NL_FAIL;
                goto out;
            }

            // Check pid and seq.
            if((nlSockAddrKern.nl_pid != 0) ||
               (rcvH->nlmsg_pid != req.h.nlmsg_pid) ||
               (rcvH->nlmsg_seq != req.h.nlmsg_seq)) {
                // Don't forget to skip that message.
                status -= NLMSG_ALIGN(len);
                rcvH = (struct nlmsghdr*)((char*)rcvH + NLMSG_ALIGN(len));
                continue;
            }

            // Error / ACK received.
            if(rcvH->nlmsg_type == NLMSG_ERROR) {
                errmsg = (struct nlmsgerr*)NLMSG_DATA(rcvH);
                if(l < sizeof(struct nlmsgerr)) {
                    _OSBASE_TRACE(1, ("[ERROR] - Truncated message."));
                    rc = NL_FAIL;
                    goto out;
                } else { // ACK
                    errno = -errmsg->error;
                    goto out;
                }
            }

            _OSBASE_TRACE(1, ("[ERROR] - Unexpected reply!!!"));
            status -= NLMSG_ALIGN(len);
            rcvH = (struct nlmsghdr*)((char*)rcvH + NLMSG_ALIGN(len));
        }

        // MSG_TRUNC = Normal data truncated.
        if(msg.msg_flags & MSG_TRUNC) {
            _OSBASE_TRACE(2, ("[INFO] - Message truncated."));
            continue;
        }
        if(status) {
            _OSBASE_TRACE(1, ("[ERROR] - Remnant of size %d.", status));
            rc = NL_FAIL;
            goto out;
        }
    }

 out:
    _OSBASE_TRACE(3, ("--- nlModifyLink() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlAddLinkToList(const struct nlmsghdr *rcvH,
                    struct nlLinkInfoList **nlLinkInfoList,
                    const struct nlLinkInfo *nlLinkInfo)
{
    _OSBASE_TRACE(3, ("--- nlAddLinkToList() called"));

    struct nlLinkInfoList *nlLinkInfoListCurr = NULL;  // Current list position.
    struct ifinfomsg      *rMsg               = NULL;  // Message content.
    struct rtattr         *rAttr              = NULL;  // Message attributes.
    int                    rAttrCount         = 0;     // Number of message
                                                       // attributes.
    char                   convAddrNew[32];            // Converted address
                                                       // (new).
    char                   convAddrOld[32];            // Converted address
                                                       // (old).
    int                    rc                 = NL_OK; // Result handler.

    // Check netlink message header.
    if(rcvH == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid netlink message."));
        rc = NL_FAIL;
        goto out;
    }

    // Check Link filter info.
    if(nlLinkInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid link filter info."));
        rc = NL_FAIL;
        goto out;
    }

    // Get message content.
    rMsg = (struct ifinfomsg*)NLMSG_DATA(rcvH);
    if(rMsg == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not get message content."));
        rc = NL_FAIL;
        goto out;
    }

    // Create link list entry.
    nlLinkInfoListCurr = malloc(sizeof(*nlLinkInfoListCurr));
    if(nlLinkInfoListCurr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for "       \
                          "\'nlLinkInfoListCurr\'."));
        rc = NL_FAIL;
        goto out;
    }
    memset(nlLinkInfoListCurr, 0, sizeof(*nlLinkInfoListCurr));

    // Alloc first entry.
    nlLinkInfoListCurr->sptr = malloc(sizeof(*(nlLinkInfoListCurr->sptr)));
    if(nlLinkInfoListCurr->sptr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not alloc memory for "       \
                          "\'nlLinkInfoListCurr->sptr\'."));
        rc = NL_FAIL;
        goto out;
    }
    memset(nlLinkInfoListCurr->sptr, 0, sizeof(*(nlLinkInfoListCurr->sptr)));

    // Link family (should be AF_UNSPEC).
    if((linkFlt.family) && (rMsg->ifi_family != nlLinkInfo->family)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Link family\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlLinkInfoListCurr->sptr->family = rMsg->ifi_family;

    // Link type (ARPHRD_*).
    if((linkFlt.type) && (rMsg->ifi_type != nlLinkInfo->type)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Link type\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlLinkInfoListCurr->sptr->type = rMsg->ifi_type;

    // Link index.
    if((linkFlt.index) && (rMsg->ifi_index != nlLinkInfo->index)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Link index\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlLinkInfoListCurr->sptr->index = rMsg->ifi_index;

    // Link flags (see netdevice(7)).
    // Here we check all flags.
    if((linkFlt.flags) && (rMsg->ifi_flags != nlLinkInfo->flags)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Link flags\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlLinkInfoListCurr->sptr->flags = rMsg->ifi_flags;

    // Link change (should be 0xFFFFFFFF).
    if((linkFlt.change) && (rMsg->ifi_change != nlLinkInfo->change)) {
        _OSBASE_TRACE(2, ("[INFO] - Skip \'Link change\'."));
        rc = NL_SKIP;
        goto out;
    }
    nlLinkInfoListCurr->sptr->change = rMsg->ifi_change;

    // Parse attributes.
    rAttr = (struct rtattr*)IFLA_RTA(rMsg);
    if(rAttr == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Could not retrieve attributes."));
        rc = NL_FAIL;
        goto out;
    }
    rAttrCount = IFLA_PAYLOAD(rcvH);
    for( ; RTA_OK(rAttr, rAttrCount) ; rAttr=RTA_NEXT(rAttr, rAttrCount)) {
        switch(rAttr->rta_type) {
        case IFLA_ADDRESS: // Interface L2 address.
            nlLinkInfoListCurr->sptr->addressLen = RTA_PAYLOAD(rAttr);
            memcpy(nlLinkInfoListCurr->sptr->address, RTA_DATA(rAttr),
                   nlLinkInfoListCurr->sptr->addressLen);
            if(linkFlt.address) {
                nlAddr_n2a(nlLinkInfoListCurr->sptr->address,
                           nlLinkInfoListCurr->sptr->addressLen,
                           nlLinkInfoListCurr->sptr->type,
                           convAddrNew, 32);
                nlAddr_n2a(nlLinkInfo->address, nlLinkInfo->addressLen,
                           nlLinkInfo->type, convAddrOld, 32);
                // Compare if received address is the same established in the
                // filter.
                if(strcmp(convAddrNew, convAddrOld) != 0) {
                    _OSBASE_TRACE(2,
                                  ("[INFO] - Skip \'Interface L2 address\'."));
                    rc = NL_SKIP;
                    goto out;
                }
            }
            break;
        case IFLA_BROADCAST: // L2 broadcast address.
            nlLinkInfoListCurr->sptr->broadcastLen = RTA_PAYLOAD(rAttr);
            memcpy(nlLinkInfoListCurr->sptr->broadcast, RTA_DATA(rAttr),
                   nlLinkInfoListCurr->sptr->broadcastLen);
            if(linkFlt.broadcast) {
                nlAddr_n2a(nlLinkInfoListCurr->sptr->broadcast,
                           nlLinkInfoListCurr->sptr->broadcastLen,
                           nlLinkInfoListCurr->sptr->type,
                           convAddrNew, 32);
                nlAddr_n2a(nlLinkInfo->broadcast, nlLinkInfo->broadcastLen,
                           nlLinkInfo->type, convAddrOld, 32);
                // Compare if received address is the same established in the
                // filter.
                if(strcmp(convAddrNew, convAddrOld) != 0) {
                    _OSBASE_TRACE(2,
                                  ("[INFO] - Skip \'L2 broadcast address\'."));
                    rc = NL_SKIP;
                    goto out;
                }
            }
            break;
        case IFLA_IFNAME: // Device name.
            if((linkFlt.ifname) && (strcmp((char*)RTA_DATA(rAttr),
                                           nlLinkInfo->ifname) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Device name\'."));
                rc = NL_SKIP;
                goto out;
            }
            snprintf(nlLinkInfoListCurr->sptr->ifname, IFNAMSIZ, "%s",
                     (char*)RTA_DATA(rAttr));
            break;
        case IFLA_MTU: // MTU of the device.
            if((linkFlt.mtu) && ((*((unsigned int*)RTA_DATA(rAttr))) !=
                                 nlLinkInfo->mtu)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'MTU of the device\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlLinkInfoListCurr->sptr->mtu = *((unsigned int*)RTA_DATA(rAttr));
            break;
        case IFLA_LINK: // Link type.
            if((linkFlt.link) && ((*((int*)RTA_DATA(rAttr))) !=
                                  nlLinkInfo->link)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Link type\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlLinkInfoListCurr->sptr->link = *((int*)RTA_DATA(rAttr));
            break;
        case IFLA_QDISC: // Queueing discipline.
            if((linkFlt.qdisc) && (strcmp((char*)RTA_DATA(rAttr),
                                          nlLinkInfo->qdisc) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Queueing discipline\'."));
                rc = NL_SKIP;
                goto out;
            }
            snprintf(nlLinkInfoListCurr->sptr->qdisc, 64, "%s",
                     (char*)RTA_DATA(rAttr));
            break;
        case IFLA_STATS: // Interface statistics
            nlLinkInfoListCurr->sptr->stats =
                *((struct net_device_stats*)RTA_DATA(rAttr));
            break;
        case IFLA_COST:
            // @todo implement.
            break;
        case IFLA_PRIORITY:
            // @todo implement.
            break;
        case IFLA_MASTER:
            // @todo implement.
            break;
        case IFLA_WIRELESS:
            // @todo implement.
            break;
        case IFLA_PROTINFO:
            // @todo implement.
            break;
        case IFLA_TXQLEN: // Tx queue length.
            if((linkFlt.txqlen) && ((*((int*)RTA_DATA(rAttr))) !=
                                    nlLinkInfo->txqlen)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Tx queue length\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlLinkInfoListCurr->sptr->txqlen = *((int*)RTA_DATA(rAttr));
            break;
        case IFLA_MAP: // Device mapping structure.
            nlLinkInfoListCurr->sptr->map = *((struct ifmap*)RTA_DATA(rAttr));
            break;
        case IFLA_WEIGHT:
            // @todo implement.
            break;
        case IFLA_OPERSTATE: // Operational state.
            if((linkFlt.operstate) && ((*((unsigned char*)RTA_DATA(rAttr))) !=
                                       nlLinkInfo->operstate)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Operational state\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlLinkInfoListCurr->sptr->operstate =
                *((unsigned char*)RTA_DATA(rAttr));
            break;
        case IFLA_LINKMODE: // Link mode.
            if((linkFlt.linkmode) && ((*((unsigned char*)RTA_DATA(rAttr))) !=
                                      nlLinkInfo->linkmode)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Link mode\'."));
                rc = NL_SKIP;
                goto out;
            }
            nlLinkInfoListCurr->sptr->linkmode =
                *((unsigned char*)RTA_DATA(rAttr));
            break;
        case IFLA_LINKINFO:
            break;
        case IFLA_NET_NS_PID:
            // @todo implement.
            break;
#ifdef IFLA_IFALIAS
        case IFLA_IFALIAS: // Interface alias.
            if((linkFlt.ifalias) && (strcmp((char*)RTA_DATA(rAttr),
                                            nlLinkInfo->ifalias) != 0)) {
                _OSBASE_TRACE(2, ("[INFO] - Skip \'Interface alias\'."));
                rc = NL_SKIP;
                goto out;
            }
            snprintf(nlLinkInfoListCurr->sptr->ifalias, IFALIASZ, "%s",
                     (char*)RTA_DATA(rAttr));
            break;
#endif //IFLA_IFALIAS
        default:
            break;
        }
    }

    // Add link to list.
    nlLinkInfoListCurr->next = *nlLinkInfoList;
    *nlLinkInfoList = nlLinkInfoListCurr;

 out:
    if(rc != NL_OK) {
        nlFreeLinkInfoList(nlLinkInfoListCurr);
    }

    _OSBASE_TRACE(3, ("--- nlAddLinkToList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlGenLinkFilter(const struct nlLinkInfo *nlLinkInfo)
{
    _OSBASE_TRACE(3, ("--- nlGenLinkFilter() called"));

    int rc = NL_OK; // Result handler.

    // Reset filter.
    nlResetLinkFilter();

    // family.
    // MUST ALWAYS BE AF_UNSPEC. Check kernel documentation for more info.
    if(nlLinkInfo->family != AF_UNSPEC) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid family: %d.", nlLinkInfo->family));
        rc = NL_FAIL;
        goto out;
    }

    // type.
    if(nlLinkInfo->type <= ARPHRD_VOID) {
        if(nlLinkInfo->type != ARPHRD_VOID) {
            linkFlt.type = 1;
        }
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid type: %d.", nlLinkInfo->type));
        rc = NL_FAIL;
        goto out;
    }

    // index.
    if(nlLinkInfo->index > 0) {
        linkFlt.index = 1;
    } else if(nlLinkInfo->index < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid index: %d.", nlLinkInfo->index));
        rc = NL_FAIL;
        goto out;
    }

    // flags.
    if(nlLinkInfo->flags > 0) {
        linkFlt.flags = 1;
    }

    // change.
    if(nlLinkInfo->change != 0xFFFFFFFF) {
        linkFlt.change = 1;
    }

    // address.
    if(strlen(nlLinkInfo->address) > 0) {
        linkFlt.address = 1;
    }

    // broadcast.
    if(strlen(nlLinkInfo->broadcast) > 0) {
        linkFlt.broadcast = 1;
    }

    // ifname.
    if(strlen(nlLinkInfo->ifname) > 0) {
        linkFlt.ifname = 1;
    }

    // mtu.
    if(nlLinkInfo->mtu > 0) {
        linkFlt.mtu = 1;
    }

    // link.
    if(nlLinkInfo->link > 0) {
        linkFlt.link = 1;
    } else if(nlLinkInfo->link < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid link: %d.", nlLinkInfo->link));
        rc = NL_FAIL;
        goto out;
    }

    // qdisc.
    if(strlen(nlLinkInfo->qdisc) > 0) {
        linkFlt.qdisc = 1;
    }

    // txqlen.
    if(nlLinkInfo->txqlen > 0) {
        linkFlt.txqlen = 1;
    } else if(nlLinkInfo->txqlen < 0) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid txqlen: %d.", nlLinkInfo->txqlen));
        rc = NL_FAIL;
        goto out;
    }

    // operstate.
    if(nlLinkInfo->operstate <= IF_OPER_UP) {
        if(nlLinkInfo->operstate != IF_OPER_UNKNOWN) {
            linkFlt.operstate = 1;
        }
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid operstate: %d.",
                          nlLinkInfo->operstate));
        rc = NL_FAIL;
        goto out;
    }

    // linkmode.
    if(nlLinkInfo->linkmode <= IF_LINK_MODE_DORMANT) {
        if(nlLinkInfo->linkmode != IF_LINK_MODE_DEFAULT) {
            linkFlt.linkmode = 1;
        }
    } else {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid linkmode: %d.",
                          nlLinkInfo->linkmode));
        rc = NL_FAIL;
        goto out;
    }

#ifdef IFLA_IFALIAS
    // ifalias.
    if(strlen(nlLinkInfo->ifalias) > 0) {
        linkFlt.ifalias = 1;
    }
#endif //IFLA_IFALIAS

 out:
    _OSBASE_TRACE(3, ("--- nlGenLinkFilter() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void nlResetLinkFilter()
{
    _OSBASE_TRACE(3, ("--- nlResetLinkFilter() called"));

    linkFlt.family    = 0;
    linkFlt.type      = 0;
    linkFlt.index     = 0;
    linkFlt.flags     = 0;
    linkFlt.change    = 0;
    linkFlt.address   = 0;
    linkFlt.broadcast = 0;
    linkFlt.ifname    = 0;
    linkFlt.mtu       = 0;
    linkFlt.link      = 0;
    linkFlt.qdisc     = 0;
    linkFlt.stats     = 0;
    linkFlt.txqlen    = 0;
    linkFlt.map       = 0;
    linkFlt.operstate = 0;
    linkFlt.linkmode  = 0;
#ifdef IFLA_IFALIAS
    linkFlt.ifalias   = 0;
#endif //IFLA_IFALIAS

    _OSBASE_TRACE(3, ("--- nlResetLinkFilter() exited"));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlCreateDefaultLinkInfo(struct nlLinkInfo *nlLinkInfo)
{
    _OSBASE_TRACE(3, ("--- nlCreateDefaultLinkInfo() called"));

    int rc = NL_OK; // Result handler.

    // Check received parameters.
    if(nlLinkInfo == NULL) {
        _OSBASE_TRACE(1, ("[ERROR] - Invalid link filter struct."));
        rc = NL_FAIL;
        goto out;
    }

    nlLinkInfo->family       = AF_UNSPEC;
    nlLinkInfo->type         = ARPHRD_VOID;
    nlLinkInfo->index        = 0;
    nlLinkInfo->flags        = 0;
    nlLinkInfo->change       = 0xFFFFFFFF;
    snprintf(nlLinkInfo->address, 32, "");
    nlLinkInfo->addressLen   = 0;
    snprintf(nlLinkInfo->broadcast, 32, "");
    nlLinkInfo->broadcastLen = 0;
    snprintf(nlLinkInfo->ifname, IFNAMSIZ, "");
    nlLinkInfo->mtu          = 0;
    nlLinkInfo->link         = 0;
    snprintf(nlLinkInfo->qdisc, 64, "");
    nlLinkInfo->txqlen       = 0;
    nlLinkInfo->operstate    = IF_OPER_UNKNOWN;
    nlLinkInfo->linkmode     = IF_LINK_MODE_DEFAULT;
#ifdef IFLA_IFALIAS
    snprintf(nlLinkInfo->ifalias, IFALIASZ, "");
#endif //IFLA_IFALIAS

 out:
    _OSBASE_TRACE(3, ("--- nlCreateDefaultLinkInfo() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int nlFreeLinkInfoList(struct nlLinkInfoList *nlLinkInfoList)
{
    _OSBASE_TRACE(3, ("--- nlFreeLinkInfoList() called"));

    struct nlLinkInfoList *nlLinkInfoListTmp = NULL;  // Temporary list
                                                      // container.
    int                    rc                = NL_OK; // Result handler.

    // Check received parameters.
    if(nlLinkInfoList == NULL) {
        _OSBASE_TRACE(2, ("[INFO] - NULL link info list, nothing to free."));
        goto out;
    }

    // Clean.
    while(nlLinkInfoList != NULL) {
        FREE_SAFE(nlLinkInfoList->sptr);
        nlLinkInfoListTmp = nlLinkInfoList;
        nlLinkInfoList = nlLinkInfoList->next;
        FREE_SAFE(nlLinkInfoListTmp);
    }

 out:
    _OSBASE_TRACE(3, ("--- nlFreeLinkInfoList() exited"));
    return rc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void nlAddr_n2a(const unsigned char *addr, int alen, int type,
                char *buf, int blen)
{
    _OSBASE_TRACE(3, ("--- nlAddr_n2a() called"));

    int i = 0;
    int l = 0;

    if(alen == 4 &&
       (type == ARPHRD_TUNNEL || type == ARPHRD_SIT || type == ARPHRD_IPGRE)) {
        inet_ntop(AF_INET, addr, buf, blen);
        return;
    }
    if(alen == 16 && type == ARPHRD_TUNNEL6) {
        inet_ntop(AF_INET6, addr, buf, blen);
        return;
    }

    for(i=0 ; i<alen ; i++) {
        if(i==0) {
            snprintf(buf+l, blen, "%02x", addr[i]);
            blen -= 2;
            l += 2;
        } else {
            snprintf(buf+l, blen, ":%02x", addr[i]);
            blen -= 3;
            l += 3;
        }
    }

    _OSBASE_TRACE(3, ("--- nlAddr_n2a() exited"));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
unsigned short nlGetLinkTypePos(unsigned short devType)
{
    _OSBASE_TRACE(3, ("--- nlGetLinkTypePos() called"));

    int i;
    for(i=0 ; i<ARRAY_SIZE(linkType) ; i++) {
        if(linkType[i] == devType) {
            return i;
        }
    }

    _OSBASE_TRACE(3, ("--- nlGetLinkTypePos() exited"));
    // The last key is used by default.
    return ARRAY_SIZE(linkType) - 1;
}
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
